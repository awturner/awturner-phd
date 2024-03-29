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

#include "Mesh/VTKMeshLoader.h"
#include "MeshBreaker/FindGenerators.h"
#include "MeshBreaker/PelvicGeneratorDirection.h"
#include "Mesh/MeshImpl.h"
#include "MeshBreaker/ShapeImage.h"

#include "Useful/PrintMacros.h"
#include <fstream>

using namespace AWT;

typedef double T;

int main(int argc, char** argv)
{
   if (argc < 4)
   {
      std::cerr << "Syntax: InnominateShapeImage <mesh filename> <resolution> <output filename>" << std::endl;
      DEBUGLINEANDEXIT(1);
   }

   const unsigned int res = atoi(argv[2]);

   if (res == 0 || res > 128)
   {
      DEBUGMACRO("resolution must be in range 1-128");
      DEBUGLINEANDEXIT(1);
   }

   Mesh<T>::P mesh = VTKMeshLoader<T>::load(argv[1], false);

   FindGenerators<T>::P findGenerators = FindGenerators<T>::getInstance(mesh);
   PelvicGeneratorDirection<T>::P pgd = PelvicGeneratorDirection<T>::getInstance(findGenerators, true);
   mesh = pgd->getMesh();

   FlattenMesh<T>::P flattenMesh = FlattenMesh<T>::getInstance(pgd);
   
   std::ofstream os(argv[3]);
   os << "-- Shape Image" << std::endl;
   os << "shapeimage = {" << std::endl;

   T coords2d[2];
   T coords3d[3];
   for (unsigned int y = 0; y < res; ++y)
   {
      coords2d[1] = static_cast<T>(y) / res;

      for (unsigned int x = 0; x < res; ++x)
      {
         coords2d[0] = static_cast<T>(x) / res;

         flattenMesh->mapFlattenedToMesh(coords2d, coords3d);

         // Write out the coordinates as a pair ((s,t), (x,y,z))
         os << "    {";
         os << " { " << coords2d[0] << ", " << coords2d[1] << " },";
         os << " { " << coords3d[0] << ", " << coords3d[1] << ", " << coords3d[2] << "}";
         os << " }," << std::endl;
      }
   }

   os << "};" << std::endl;
   os.close();

   DEBUGMACRO("Shape image written to " << argv[3]);
}