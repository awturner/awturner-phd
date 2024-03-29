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
#include "NormalCalculator.h"

#include "Mesh.h"
#include "Tuples.h"
#include "TuplesImpl.h"

template <class T>
void AWT::NormalCalculator<T>::calculateNormalsAndSet(typename AWT::Mesh<T>::P mesh, typename Tuples<T>::P vtxNormals, typename Tuples<T>::P faceNormals)
{
   bool createVtx  = false;
   bool createFace = false;

   T def[] = { 0, 0, 0 };

   if (*vtxNormals == 0)
   {
      createVtx = true;
      vtxNormals = TuplesImpl<T>::getInstance(3, def, mesh->getNumberOfVertices());
   }

   if (*faceNormals == 0)
   {
      createFace = true;
      faceNormals = TuplesImpl<T>::getInstance(3, def, mesh->getNumberOfVertices());
   }

   calculateNormals(mesh, vtxNormals, faceNormals);

   mesh->setVertexNormals(vtxNormals);
   mesh->setFaceNormals(faceNormals);
}

template class AWT::NormalCalculator<double>;
template class AWT::NormalCalculator<float>;