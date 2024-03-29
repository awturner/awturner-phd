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
#ifndef __SAMPLEDMESH_H__
#define __SAMPLEDMESH_H__

#include "Mesh.h"
#include "Tuples.h"
#include "SamplePoints.h"

namespace AWT
{
   class SamplePoints;

   template <class T>
   class SampledMesh : public Mesh<T>
   {
   public:
      typedef ManagedAutoPointer<SampledMesh<T> > P;

   protected:
      SampledMesh(typename Mesh<T>::P mesh, SamplePoints::P sampPts);
      virtual ~SampledMesh();

   public:
      static typename SampledMesh<T>::P getInstance(typename Mesh<T>::P mesh, SamplePoints::P sampPts);

      virtual void                 setVertexNormals(typename AWT::Tuples<T>::P /*in_VertexNormals*/);

      virtual bool                 hasFaces() const;

      virtual MeshIndex            getNumberOfVertices() const;

      virtual void                 setVertices(typename AWT::Tuples<T>::P verts);

      virtual typename Tuples<T>::P  getVertices() const;

      virtual void                 getVertex(const MeshIndex in_Index, T* out_Vertex) const;

      virtual void                 setVertex(const MeshIndex in_Index, const T* in_Vertex);

      virtual void                 setVertex(const MeshIndex in_Index, const T a, const T b, const T c);

      virtual bool                 hasVertexNormals() const;

      virtual typename Tuples<T>::P getVertexNormals() const;

      virtual bool                  hasTextureCoords() const;
      virtual typename Tuples<T>::P getTextureCoords() const;
      virtual void                  setTextureCoords(typename AWT::Tuples<T>::P in_TexCoords);

      virtual MeshIndex            getNumberOfFaces() const;

      virtual typename Tuples<MeshIndex>::P  getFaces() const;

      virtual void                 setFaces(typename Tuples<MeshIndex>::P);

      virtual void                 getFace(const MeshIndex /*in_Index*/, T* /*out_VertexA*/, T* /*out_VertexB*/, T* /*out_VertexC */) const;
      
      virtual void                 getFaceIndices(const MeshIndex /*in_Index*/, MeshIndex* /*out_VertexIndices*/) const;

      virtual void                 setFaceIndices(const MeshIndex /*in_Index*/, const MeshIndex /*a*/, const MeshIndex /*b*/, const MeshIndex /*c*/);

      virtual void                 setFaceIndices(const MeshIndex /*in_Index*/, const MeshIndex* /*in_VertexIndices*/);

      virtual bool                 hasFaceNormals() const;

      virtual typename Tuples<T>::P getFaceNormals() const;

      virtual void                 setFaceNormals(typename AWT::Tuples<T>::P /*in_VertexNormals*/);

      virtual void                 prepareToSearchFaces(bool forceRebuild = false);

      virtual void                 searchFaces(typename FacesSearch<T>::P /*searcher*/);

      virtual void                 lock(void* obj);

      virtual void                 unlock(void* obj);

      virtual void                 prepareToSearchVertices(bool forceRebuild = false);

      virtual void                 search(typename AWT::MeshSearch<T>::P search);

      virtual void                 searchVertices(typename AWT::VerticesSearch<T>::P search);

      virtual typename MeshData<T>::P getMeshData();

      virtual std::string          getClassName() const;
   protected:
      struct D;
      D* m_D;
   };
}

#endif // __SAMPLEDMESH_H__