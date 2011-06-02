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
#include "MeshSlicer.h"

#include "CuttingFunction.h"
#include "Mesh/MeshImpl.h"
#include "MeshSlicedPair.h"
#include <limits>

template <class T>
struct AWT::MeshSlicer<T>::D
{
   typename CuttingFunction<T>::P m_CuttingFunction;
};

template <class T>
AWT::MeshSlicer<T>::MeshSlicer()
{
   m_D = new D;
}

template <class T>
AWT::MeshSlicer<T>::~MeshSlicer()
{
   delete m_D;
}

template <class T>
typename AWT::MeshSlicer<T>::P AWT::MeshSlicer<T>::getInstance()
{
   AUTOGETINSTANCE(AWT::MeshSlicer<T>, ());
}

template <class T>
GETNAMEMACRO(AWT::MeshSlicer<T>);

template <class T>
void AWT::MeshSlicer<T>::setCuttingFunction(typename CuttingFunction<T>::P cutF)
{
   m_D->m_CuttingFunction = cutF;
   modified();
}

template <class T>
typename AWT::CuttingFunction<T>::P AWT::MeshSlicer<T>::getCuttingFunction()
{
   return m_D->m_CuttingFunction;
}

template <class T>
typename AWT::MeshSlicedPair<T>::P AWT::MeshSlicer<T>::sliceMesh(typename AWT::Mesh<T>::P mesh)
{
   bool*      included   = new bool[ mesh->getNumberOfVertices() ];

   // Go through and work out which vertices are included and excluded
   // Also, go through and work out the new vertex indices
   T vtx[3];
   //for (MeshIndex v = 0; v < mesh->getNumberOfVertices(); ++v)
   MESH_EACHVERTEX(mesh, v)
   {
      mesh->getVertex(v, vtx);

      included[v] = m_D->m_CuttingFunction->accept(vtx);
   }

   MeshIndex* newIndices = new MeshIndex[ mesh->getNumberOfVertices() ];
   const MeshIndex unassigned = std::numeric_limits<MeshIndex>::max();

   MeshIndex vi[3];

   typename Mesh<T>::P meshes[2];

   for (int m = 0; m < 2; ++m)
   {
      bool flip = (m == 0);

      meshes[m] = MeshImpl<T>::getInstance(mesh->getNumberOfVertices(), mesh->getNumberOfFaces());

      MeshIndex lastAssignedFace   = 0;
      MeshIndex lastAssignedVertex = 0;

      // Mark all vertices as being unassigned
      //for (MeshIndex v = 0; v < mesh->getNumberOfVertices(); ++v)
      MESH_EACHVERTEX(mesh, v)
         newIndices[v] = unassigned;

      //for (MeshIndex f = 0; f < mesh->getNumberOfFaces(); ++f)
      MESH_EACHFACE(mesh, f)
      {
         mesh->getFaceIndices(f, vi);

         // flip enables us to get both the included and excluded with the same code
         if (flip ^ (included[ vi[0] ] && included[ vi[1] ] && included[ vi[2] ]))
         {
            for (int i = 0; i < 3; ++i)
            {
               if (newIndices[ vi[i] ] == unassigned)
               {
                  // Get the vertex from the old mesh and set its position in the new
                  mesh->getVertex(vi[i], vtx);
                  meshes[m]->setVertex(lastAssignedVertex, vtx);

                  newIndices[ vi[i] ] = lastAssignedVertex++;
               }

               vi[i] = newIndices[ vi[i] ];
            }

            meshes[m]->setFaceIndices(lastAssignedFace++, vi);
         }
      }
   }

   MeshSlicedPair<T>::P ret = MeshSlicedPair<T>::getInstance(meshes[0], meshes[1]);

   delete[] newIndices;
   delete[] included;

   return ret;
}

template class AWT::MeshSlicer<double>;
template class AWT::MeshSlicer<float>;