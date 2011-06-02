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
#include "Mesh/MeshImpl.h"
#include "Mesh/VTKMeshLoader.h"
#include "Mesh/VTKMeshWriter.h"

#include "MeshBreaker/FindGenerators.h"
#include "MeshBreaker/FlattenMesh.h"
#include "MeshBreaker/PelvicGeneratorDirection.h"

#include "Useful/PrintMacros.h"
#include "Mesh/MeshFunctions.h"

#include "cimg.h"
#include <sstream>

using namespace AWT;
using namespace cimg_library;
typedef double T;

int main(int argc, char** argv)
{
   const int nsamp = atoi(argv[1]);

   for (int i = 2; i < argc; ++i)
   {
      Mesh<T>::P mesh = VTKMeshLoader<T>::load(argv[i]);

      const int euler = MeshFunctions<T>::getEulerCharacteristic(mesh);

      if (euler != 0)
      {
         DEBUGMACRO("This mesh is not a closed 1-torus!  (Euler characteristic = " << euler << ")");
         continue;
      }

      FindGenerators<T>::P findGenerators = FindGenerators<T>::getInstance(mesh);
      PelvicGeneratorDirection<T>::P pgd = PelvicGeneratorDirection<T>::getInstance(findGenerators, true);
      mesh = pgd->getMesh();

      FlattenMesh<T>::P flatten = FlattenMesh<T>::getInstance(pgd);

      const unsigned int w = nsamp;
      const unsigned int h = nsamp;
      CImg<T> shapeImage(w, h, 1, 3);

      T flatCoords[2], meshCoords[3];

      cimg_forXY(shapeImage, ix, iy)
      {
         flatCoords[0] = static_cast<T>(ix) / static_cast<T>(nsamp);
         flatCoords[1] = static_cast<T>(iy) / static_cast<T>(nsamp);

         flatten->mapFlattenedToMesh(flatCoords, meshCoords);

         cimg_forV(shapeImage, v)
            shapeImage(ix, iy, 0, v) = meshCoords[v];
      }

      // Try to reconstruct the mesh from the shape image
      Mesh<T>::P recon = MeshImpl<T>::getInstance(w*h, w*h*2);

      // Set the vertices
      unsigned int v = 0;
      cimg_forXY(shapeImage, ix, iy)
         recon->setVertex(v++, shapeImage(ix,iy,0,0), shapeImage(ix,iy,0,1), shapeImage(ix,iy,0,2));

      // Connect into faces
      unsigned int f = 0;
      cimg_forXY(shapeImage, ix, iy)
      {
         MeshIndex here      = ((iy+0)%h)*w + (ix+0)%w;
         MeshIndex east      = ((iy+0)%h)*w + (ix+1)%w;
         MeshIndex south     = ((iy+1)%h)*w + (ix+0)%w;
         MeshIndex southeast = ((iy+1)%h)*w + (ix+1)%w;

         recon->setFaceIndices(f++, here, east, south);
         recon->setFaceIndices(f++, east, southeast, south);
      }

      const int reuler = MeshFunctions<T>::getEulerCharacteristic(mesh);
      PRINTVBL(reuler);

      std::stringstream sstr;
      sstr << argv[i] << ".vtk";
      VTKMeshWriter<T>::write(recon, sstr.str().c_str());
   }
}