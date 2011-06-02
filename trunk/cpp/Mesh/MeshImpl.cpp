/*
* ################################################################################
* ### MIT License
* ################################################################################
* 
* Copyright (c) 2006-2011 Andy Turner
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/
#include "MeshImpl.h"

#include "MeshSearching/FacesTreeData.h"

#include "MeshSearching/FacesSearch.h"
#include "MeshSearching/VerticesNearestVertexSearch.h"

#include "ObjectEKDTree/OEKDTree.h"



#include "Tuples.h"
#include "TuplesImpl.h"

#include "Useful/ArrayVectorFunctions.h"
#include "Useful/PrintMacros.h"
#include "Useful/ProfTimer.h"

#include "MeshData.h"

#include "MeshSearching/VerticesTreeData.h"
#include <limits>
#include <vector>


template <class T>
struct AWT::MeshImpl<T>::D
{
   typename Tuples<T>::P               m_Vertices;
   typename Tuples<T>::P               m_VertexNormals;
   typename Tuples<T>::P               m_TextureCoords;

   typename Tuples<MeshIndex>::P       m_Faces;
   typename Tuples<T>::P               m_FaceNormals;

   std::vector<VertexUsedCount>        m_VertexUsedCount;

   static const MeshIndex              ms_UnusedFaceVertexIndex;

   typename OEKDTree::OEKDTree<T,3>::P m_VerticesTree;
   typename OEKDTree::OEKDTree<T,3>::P m_FacesTree;

   typename MeshData<T>::P             m_MeshData;

   void*                               m_LockObj;
};

template <class T>
const AWT::MeshIndex AWT::MeshImpl<T>::D::ms_UnusedFaceVertexIndex = std::numeric_limits<MeshIndex>::max();

using namespace AWT;

template <class T>
AWT::MeshImpl<T>::MeshImpl(const MeshIndex in_NumberOfVertices = 0, const MeshIndex in_NumberOfFaces = 0)
{
   m_D = new D;
   m_D->m_LockObj = 0;

   m_D->m_MeshData = MeshData<T>::getInstance(this);

   m_D->m_Vertices = TuplesImpl<T>::getInstance(3, in_NumberOfVertices);
   
   m_D->m_VertexNormals.set(0);

   MeshIndex defaultFace[] = { m_D->ms_UnusedFaceVertexIndex, m_D->ms_UnusedFaceVertexIndex, m_D->ms_UnusedFaceVertexIndex };

   m_D->m_Faces = TuplesImpl<MeshIndex>::getInstance(3, &defaultFace[0], in_NumberOfFaces);
   
   m_D->m_FaceNormals.set(0);

   // These are the two Kd trees
   m_D->m_FacesTree.set(0);
   m_D->m_VerticesTree.set(0);

   m_D->m_VertexUsedCount.reserve(in_NumberOfVertices);
}
template <class T>
AWT::MeshImpl<T>::~MeshImpl()
{
   delete m_D;
}

template <class T>
typename AWT::MeshImpl<T>::P AWT::MeshImpl<T>::getInstance(const typename AWT::Mesh<T>::P mesh)
{
   typename AWT::MeshImpl<T>::P ret = MeshImpl<T>::getInstance(mesh->getNumberOfVertices(), mesh->getNumberOfFaces());

   if (*mesh->getVertices() != 0)
   {
      ret->setVertices(TuplesImpl<T>::getInstance(mesh->getVertices()));
   }

   if (*mesh->getVertexNormals() != 0)
   {
      ret->setVertexNormals(TuplesImpl<T>::getInstance(mesh->getVertexNormals()));
   }

   if (*mesh->getFaces() != 0)
   {
      ret->setFaces(TuplesImpl<MeshIndex>::getInstance(mesh->getFaces()));
   }

   if (*mesh->getFaceNormals() != 0)
   {
      ret->setFaceNormals(TuplesImpl<T>::getInstance(mesh->getFaceNormals()));
   }

   if (*mesh->getTextureCoords() != 0)
   {
      ret->setTextureCoords(TuplesImpl<T>::getInstance(mesh->getTextureCoords()));
   }

   MeshIndex vs[3];
   //for (MeshIndex f = 0; f < mesh->getNumberOfFaces(); ++f)
   MESH_EACHFACE(mesh, f)
   {
      mesh->getFaceIndices(f, vs);

      for (int i = 0; i < 3; ++i)
      {
         while (ret->m_D->m_VertexUsedCount.size() <= vs[i])
            ret->m_D->m_VertexUsedCount.push_back(0);

         ++ret->m_D->m_VertexUsedCount[vs[i]];
      }
   }

   return ret;
}



template <class T>
typename AWT::MeshImpl<T>::P AWT::MeshImpl<T>::getInstance(const MeshIndex in_NumberOfVertices = 0, const MeshIndex in_NumberOfFaces = 0)
{
   AUTOGETINSTANCE(MeshImpl<T>, (in_NumberOfVertices, in_NumberOfFaces));
}

template <class T>
GETNAMEMACRO(AWT::MeshImpl<T>);

template <class T>
MeshIndex AWT::MeshImpl<T>::getNumberOfVertices() const
{
   return m_D->m_Vertices->getNumberOfPoints();
}

template <class T>
void AWT::MeshImpl<T>::getVertex(const MeshIndex in_Index, T* out_Vertex) const
{
   m_D->m_Vertices->getPoint(in_Index, out_Vertex);
}

template <class T>
void AWT::MeshImpl<T>::setVertex(const MeshIndex in_Index, const T* in_Vertex)
{
   // Make sure that we have a vertex used counter for this vertex
   while (m_D->m_VertexUsedCount.size() <= in_Index)
      m_D->m_VertexUsedCount.push_back(0);

   m_D->m_Vertices->setPoint(in_Index, in_Vertex);

   if (this->hasVertexNormals() != 0)
   {
      // Make sure that the vertices and vertex normals have the same number of elements
      m_D->m_Vertices->ensureSize(m_D->m_VertexNormals->getNumberOfPoints());
      m_D->m_VertexNormals->ensureSize(m_D->m_Vertices->getNumberOfPoints());
   }

   this->modified();
}

template <class T>
void AWT::MeshImpl<T>::setVertex(const MeshIndex in_Index, const T x, const T y, const T z)
{
   T arr[3];

   arr[0] = x;
   arr[1] = y;
   arr[2] = z;

   setVertex(in_Index, &arr[0]);
}

template <class T>
bool AWT::MeshImpl<T>::hasVertexNormals() const
{
   return *m_D->m_VertexNormals != 0;
}

template <class T>
typename AWT::Tuples<T>::P AWT::MeshImpl<T>::getVertexNormals() const
{
   return m_D->m_VertexNormals;
}

template <class T>
void AWT::MeshImpl<T>::setVertexNormals(typename Tuples<T>::P in_VertexNormals)
{
   if (in_VertexNormals->getTupleSize() != 3)
      AWTEXCEPTIONTHROW("Tuple must have 3 elements!");

   if (in_VertexNormals->getNumberOfPoints() > m_D->m_Vertices->getNumberOfPoints())
      AWTEXCEPTIONTHROW("Too many vertex normals!");

   m_D->m_VertexNormals = in_VertexNormals;
}

template <class T>
bool AWT::MeshImpl<T>::hasTextureCoords() const
{
   return *m_D->m_TextureCoords != 0;
}

template <class T>
typename AWT::Tuples<T>::P AWT::MeshImpl<T>::getTextureCoords() const
{
   return m_D->m_TextureCoords;
}

template <class T>
void AWT::MeshImpl<T>::setTextureCoords(typename Tuples<T>::P in_TextureCoords)
{
   if (in_TextureCoords->getNumberOfPoints() > m_D->m_Vertices->getNumberOfPoints())
   {
      PRINTVBL(in_TextureCoords->getNumberOfPoints());
      PRINTVBL(m_D->m_Vertices->getNumberOfPoints());
      AWTEXCEPTIONTHROW("Too many texture coordinates!");
   }

   m_D->m_TextureCoords = in_TextureCoords;
}

template <class T>
bool AWT::MeshImpl<T>::hasFaces() const
{
   return m_D->m_Faces->getNumberOfPoints() != 0;
}

template <class T>
MeshIndex AWT::MeshImpl<T>::getNumberOfFaces() const
{
   return m_D->m_Faces->getNumberOfPoints();
}

template <class T>
void AWT::MeshImpl<T>::getFace(const MeshIndex in_Index, T* out_VertexA, T* out_VertexB, T* out_VertexC) const
{
   MeshIndex indices[3];

   getFaceIndices(in_Index, &indices[0]);

   getVertex(indices[0], out_VertexA);
   getVertex(indices[1], out_VertexB);
   getVertex(indices[2], out_VertexC);
}

template <class T>
void AWT::MeshImpl<T>::getFaceIndices(const MeshIndex in_Index, MeshIndex* out_VertexIndices) const
{
   if (in_Index < m_D->m_Faces->getNumberOfPoints())
   {
      m_D->m_Faces->getPoint(in_Index, &out_VertexIndices[0]);
   }
   else
   {
      out_VertexIndices[0] = out_VertexIndices[1] = out_VertexIndices[2] = m_D->ms_UnusedFaceVertexIndex;
   }
}

template <class T>
void AWT::MeshImpl<T>::setFaceIndices(const MeshIndex in_Index, const MeshIndex a, const MeshIndex b, const MeshIndex c)
{
   MeshIndex is[3];

   is[0] = a;
   is[1] = b;
   is[2] = c;

   setFaceIndices(in_Index, &is[0]);
}

template <class T>
void AWT::MeshImpl<T>::setFaceIndices(const MeshIndex in_Index, const MeshIndex* in_VertexIndices)
{
   MeshIndex currentIndices[3];

   for (TupleIndex i = 0; i < 3; ++i)
   {
      if (in_VertexIndices[i] >= m_D->m_Vertices->getNumberOfPoints())
         AWTEXCEPTIONTHROW("Vertex index out of bounds!");
   }

   if (in_Index+1 < m_D->m_Faces->getNumberOfPoints())
   {
      getFaceIndices(in_Index, &currentIndices[0]);

      // Decrement the use counters
      for (TupleIndex i = 0; i < 3; ++i)
      {
         if (currentIndices[i] != m_D->ms_UnusedFaceVertexIndex)
         {
            --m_D->m_VertexUsedCount[ currentIndices[i] ];
         }
      }
   }

   m_D->m_Faces->setPoint(in_Index, in_VertexIndices);

   // Increment the use counters
   for (TupleIndex i = 0; i < 3; ++i)
   {
      ++m_D->m_VertexUsedCount[ in_VertexIndices[i] ];
   }

   if (this->hasFaceNormals() != 0)
   {
      // Make sure that the faces and faces normals have the same number of elements
      m_D->m_Faces->ensureSize(m_D->m_FaceNormals->getNumberOfPoints());
      m_D->m_FaceNormals->ensureSize(m_D->m_Faces->getNumberOfPoints());
   }
}

template <class T>
bool AWT::MeshImpl<T>::hasFaceNormals() const
{
   return *m_D->m_FaceNormals != 0;
}

template <class T>
typename AWT::Tuples<T>::P AWT::MeshImpl<T>::getFaceNormals() const
{
   return m_D->m_FaceNormals;
}  

template <class T>
void AWT::MeshImpl<T>::setFaceNormals(typename Tuples<T>::P in_FaceNormals)
{
   if (in_FaceNormals->getTupleSize() != 3)
      AWTEXCEPTIONTHROW("Tuple must have 3 elements!");

   if (in_FaceNormals->getNumberOfPoints() > m_D->m_Faces->getNumberOfPoints())
      AWTEXCEPTIONTHROW("Too many face normals!");

   m_D->m_FaceNormals = in_FaceNormals;
}



template <class T>
T AWT::MeshImpl<T>::normalize(T* vec)
{
   T len = vec[0]*vec[0] + vec[1]*vec[1] + vec[2]*vec[2];
   if (len != 0)
   {
      len = sqrt(len);
      vec[0] /= len;
      vec[1] /= len;
      vec[2] /= len;
   }

   return len;
}

template <class T>
void AWT::MeshImpl<T>::getFaceNormalUnnormalized(const AWT::MeshIndex in_Index, T* out_Normal) const
{
   T point[3][3];
   getFace(in_Index, &point[0][0], &point[1][0], &point[2][0]);

   // Get the directions of the edges relative to the zero-th point
   for (MeshIndex j = 0; j < 3; ++j)
   {
      point[1][j] -= point[0][j];
      point[2][j] -= point[0][j];
   }

   cross(point[1], point[2], out_Normal);
}



template <class T>
void AWT::MeshImpl<T>::search(typename MeshSearch<T>::P searcher)
{
   if (dynamic_cast<VerticesSearch<T>*>(*searcher) != 0)
      searchVertices(VerticesSearch<T>::P(dynamic_cast<VerticesSearch<T>*>(*searcher)));
   if (dynamic_cast<FacesSearch<T>*>(*searcher) != 0)
      searchFaces(FacesSearch<T>::P(dynamic_cast<FacesSearch<T>*>(*searcher)));
}

template <class T>
void AWT::MeshImpl<T>::prepareToSearchVertices(bool forceRebuild)
{
   if (*m_D->m_VerticesTree == 0 || forceRebuild)
   {
      VerticesTreeData<T>::P wrapPointData = VerticesTreeData<T>::getInstance(m_D->m_Vertices);
      m_D->m_VerticesTree = OEKDTree::OEKDTree<T,3>::getInstance(wrapPointData);
   }

   if (m_D->m_VerticesTree->getModifiedTime() < m_D->m_Vertices->getModifiedTime() || forceRebuild)
   {
      m_D->m_VerticesTree->build();
   }
}

template <class T>
void AWT::MeshImpl<T>::searchVertices(typename VerticesSearch<T>::P searcher)
{
   prepareToSearchVertices();
   searcher->setData(m_D->m_VerticesTree->getData());
   searcher->searchTree(*m_D->m_VerticesTree);
}

template <class T>
void AWT::MeshImpl<T>::prepareToSearchFaces(bool forceRebuild)
{
   if (*m_D->m_FacesTree == 0 || forceRebuild)
   {
      FacesTreeData<T>::P wrapFaceData = FacesTreeData<T>::getInstance(m_D->m_Faces, m_D->m_Vertices);
      m_D->m_FacesTree = OEKDTree::OEKDTree<T,3>::getInstance(wrapFaceData);
   }

   if (m_D->m_FacesTree->getModifiedTime() < std::max<ModifiedTime>(m_D->m_Faces->getModifiedTime(), m_D->m_Vertices->getModifiedTime()) || forceRebuild)
   {
      m_D->m_FacesTree->build();
      m_D->m_FacesTree->modified();
   }
}

template <class T>
void AWT::MeshImpl<T>::searchFaces(typename AWT::FacesSearch<T>::P searcher)
{
   prepareToSearchFaces();

   searcher->setData(m_D->m_FacesTree->getData());
   searcher->searchTree(*m_D->m_FacesTree);
}

template <class T>
typename AWT::Tuples<T>::P AWT::MeshImpl<T>::getVertices() const
{
   return m_D->m_Vertices;
}

template <class T>
typename AWT::Tuples<AWT::MeshIndex>::P AWT::MeshImpl<T>::getFaces() const
{
   return m_D->m_Faces;
}

template <class T>
void AWT::MeshImpl<T>::setVertices(typename AWT::Tuples<T>::P in_Vertices)
{
   m_D->m_Vertices = in_Vertices;
   modified();
}

template <class T>
void AWT::MeshImpl<T>::setFaces(typename AWT::Tuples<MeshIndex>::P in_Faces)
{
   m_D->m_Faces = in_Faces;
   modified();
}

template <class T>
AWT::ModifiedTime AWT::MeshImpl<T>::getModifiedTime() const
{
   ModifiedTime ret = Mesh<T>::getModifiedTime();

   if (*m_D->m_Vertices != 0) ret = std::max(ret, m_D->m_Vertices->getModifiedTime());
   if (*m_D->m_Faces    != 0) ret = std::max(ret, m_D->m_Faces->getModifiedTime());

   if (*m_D->m_TextureCoords != 0) ret = std::max(ret, m_D->m_TextureCoords->getModifiedTime());
   if (*m_D->m_VertexNormals != 0) ret = std::max(ret, m_D->m_VertexNormals->getModifiedTime());
   if (*m_D->m_FaceNormals   != 0) ret = std::max(ret, m_D->m_FaceNormals->getModifiedTime());

   return ret;
}

template <class T>
typename AWT::MeshData<T>::P AWT::MeshImpl<T>::getMeshData()
{
   return m_D->m_MeshData;
}

template <class T>
void AWT::MeshImpl<T>::lock(void* obj)
{
   if (*m_D->m_TextureCoords != 0) m_D->m_TextureCoords->lock(this);

   if (*m_D->m_Vertices != 0)      m_D->m_Vertices->lock(this);
   if (*m_D->m_VertexNormals != 0) m_D->m_VertexNormals->lock(this);

   if (*m_D->m_Faces != 0)         m_D->m_Faces->lock(this);
   if (*m_D->m_FaceNormals != 0)   m_D->m_FaceNormals->lock(this);
}

template <class T>
void AWT::MeshImpl<T>::unlock(void* obj)
{
   if (*m_D->m_Vertices != 0)      m_D->m_Vertices->unlock(this);
   if (*m_D->m_VertexNormals != 0) m_D->m_VertexNormals->unlock(this);

   if (*m_D->m_Faces != 0)         m_D->m_Faces->unlock(this);
   if (*m_D->m_FaceNormals != 0)   m_D->m_FaceNormals->unlock(this);

   if (*m_D->m_TextureCoords != 0) m_D->m_TextureCoords->lock(this);
}

template class AWT::MeshImpl<double>;
template class AWT::MeshImpl<float>;