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
#include "MeshBoundaryFinder.h"

#include "Mesh/MeshConnectivity.h"
#include "Mesh/TuplesImpl.h"

template <class T>
typename AWT::Tuples<T>::P AWT::MeshBoundaryFinder<T>::findBoundaryVertices(typename AWT::Mesh<T>::P mesh)
{
      MeshConnectivity<T>::P conn = MeshConnectivity<T>::getInstance(mesh);

      MeshIndex fs[3];
      MeshIndex vs[3];
      T vtx[3];

      Tuples<T>::P borderPoints = TuplesImpl<T>::getInstance(3, 1000);

      MESH_EACHFACE(mesh, f)
      {
         if (conn->getAdjacentFaces(f, fs) != 3)
         {
            mesh->getFaceIndices(f, vs);

            for (int i = 0; i < 3; ++i)
            {
               if (conn->isVertexOnBoundary(vs[i]) && conn->isVertexOnBoundary(vs[(i+1)%3]))
               {
                  DEBUGMACRO((vs[i]) << " " << (vs[(i+1)%3]));

                  mesh->getVertex(vs[i], vtx);
                  borderPoints->addPoint(vtx);

                  mesh->getVertex(vs[(i+1)%3], vtx);
                  borderPoints->addPoint(vtx);
               }
            }
         }
      }

      return borderPoints;
}

template class AWT::MeshBoundaryFinder<double>;
template class AWT::MeshBoundaryFinder<float>;