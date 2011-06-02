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
#include "DeformableMesh/DeformableMeshLoader.h"

#include "Mesh/VTKMeshLoader.h"
#include "Mesh/VTKMeshWriter.h"

#include "Useful/PrintMacros.h"
#include "Useful/Noise.h"

#include "MeshSlicer/MeshSlicer.h"
#include "MeshSlicer/PlanarCut.h"

#include <algorithm>

using namespace AWT;
typedef double T;

int main(int argc, char** argv)
{
   if (argc < 8)
   {
      std::cerr << "Must provide at least input, 2 output filenames and plane normal";
      exit(1);
   }

   Mesh<T>::P mesh = VTKMeshLoader<T>::load(argv[1], false);
   PlanarCut<T>::P plane = PlanarCut<T>::getInstance();
   MeshSlicer<T>::P slicer = MeshSlicer<T>::getInstance();

   T planeNormal[4];
   for (int i = 0; i < 4; ++i)
      planeNormal[i] = atof(argv[i+4]);

   PRINTVEC(planeNormal, 4);

   plane->setPlane(planeNormal);

   slicer->setCuttingFunction(plane);

   MeshSlicedPair<T>::P sliced = slicer->sliceMesh(mesh);

   VTKMeshWriter<T>::write(sliced->getIncluded(), argv[2]);
   VTKMeshWriter<T>::write(sliced->getExcluded(), argv[3]);
}