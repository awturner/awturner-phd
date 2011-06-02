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
#include "Mesh/Mesh.h"
#include "Mesh/MeshBoundaryFinder.h"

#include "Mesh/VTKMeshLoader.h"

using namespace AWT;
typedef double T;

int main(int argc, char** argv)
{
   for (int i = 1; i < argc; ++i)
   {
      Mesh<T>::P mesh = VTKMeshLoader<T>::load(argv[i]);
      Tuples<T>::P borderPoints = MeshBoundaryFinder<T>::findBoundaryVertices(mesh);

      std::cout << argv[i] << "\t" << borderPoints->getNumberOfPoints()/2 << std::endl;

      for (MeshIndex v = 0, vmax = borderPoints->getNumberOfPoints(); v < vmax; ++v)
      {
         T vtx[3];
         borderPoints->getPoint(v, vtx);
         std::cerr << "\t\t" << vtx[0] << " " << vtx[1] << " " << vtx[2] << std::endl;
      }
   }
}