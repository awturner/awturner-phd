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
#include "Mesh/VTKMeshWriter.h"
#include "Mesh/TuplesFunctions.h"

#include "vnl/vnl_matrix.h"

using namespace AWT;

typedef double T;

int main(int argc, char** argv)
{
   vnl_matrix<T> theFlip(2,2);
   theFlip.set_identity();

   if (strcmp(argv[2], "X") == 0)
      theFlip(0,0) = -1;
   else if (strcmp(argv[2], "Y") == 0)
      theFlip(1,1) = -1;
   else if (strcmp(argv[2], "XY") == 0)
      theFlip(0,0) = theFlip(1,1) = -1;
   else
   {
      DEBUGMACRO("Unrecognised flip: must be X, Y or XY");
      DEBUGLINEANDEXIT(1);
   }

   Mesh<T>::P mesh = VTKMeshLoader<T>::load(argv[1]);
   if (!mesh->hasTextureCoords())
   {
      DEBUGMACRO("Mesh doesn't have texture coordinates to flip");
      DEBUGLINEANDEXIT(1);
   }

   std::cerr << theFlip;
   T vtx[3];
   MESH_EACHVERTEX(mesh, v)
   {
      mesh->getTextureCoords()->getPoint(v, vtx);
      vtx[0] *= theFlip(0,0);
      vtx[1] *= theFlip(1,1);
      mesh->getTextureCoords()->setPoint(v, vtx);
   }
   mesh->setTextureCoords(TuplesFunctions<T>::transform(mesh->getTextureCoords(), theFlip));
   VTKMeshWriter<T>::write(mesh, argv[1]);
}