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
/*! \file
\brief Checks the manifoldness of a mesh

Looks at every edge in the mesh, and checks to see if more than two faces
are using the edge.  If so, the mesh is non-manifold, and so cannot be
represented by a half-edge data structure.

A list of the edge vertices and faces using the edge is written to standard
output:-

<pre>
vertex1  vertex 2:   face1,face2,...,faceN
</pre>

*/

#include "Mesh/Mesh.h"
#include "Mesh/MeshConnectivity.h"
#include "Mesh/VTKMeshLoader.h"

#include <set>

using namespace AWT;

typedef double T;

int main(int argc, char** argv)
{
   std::cerr << "Loading model " << argv[1] << "..." << std::endl;
   Mesh<T>::P mesh = VTKMeshLoader<T>::load(argv[1], false);
   std::cerr << "Done." << std::endl;

   MeshConnectivity<T>::P conn = MeshConnectivity<T>::getInstance(mesh);

   MeshIndex vs[100];
   MeshIndex vi[3];
   MeshIndex nadj;

   std::set< std::pair<MeshIndex,MeshIndex> > foundPairs;

   //for (MeshIndex f = 0; f < mesh->getNumberOfFaces(); ++f)
   MESH_EACHFACE(mesh, f)
   {
      mesh->getFaceIndices(f, vi);

      for (int i = 0; i < 3; ++i)
         for (int j = i+1; j < 3; ++j)
         {
            if (vi[i] < vi[j])
            {
               nadj = conn->getFacesUsingEdge(vi[i], vi[j], vs);
               if (nadj > 2)
               {
                  std::pair<MeshIndex,MeshIndex> pr(vi[i], vi[j]);

                  if (foundPairs.find(pr) == foundPairs.end())
                  {
                     std::cout << vi[i] << "\t" << vi[j] << ": ";
                     for (unsigned int a = 0; a < nadj; ++a)
                     {
                        if (a > 0)
                           std::cout << ",";
                        std::cout << vs[a];
                     }
                     std::cout << std::endl;

                     foundPairs.insert(pr);
                  }
               }
            }
         }
   }
   //DEBUGMACRO("Finished manifold check");
}