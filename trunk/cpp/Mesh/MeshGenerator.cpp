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
#include "MeshGenerator.h"

#include "Maths/Constants.h"
#include "Mesh/TuplesImpl.h"

#include <cmath>

template <class T>
void AWT::MeshGenerator<T>::generateTorus(typename AWT::Mesh<T>::P mesh, T majorRadius, T minorRadius, MeshIndex resU, MeshIndex resV)
{
   T vtx[3];

   const T twoPi = AWT::Constants<T>::Pi * 2;

   // Add in some texture coordinates as well
   Tuples<T>::P texCoords = TuplesImpl<T>::getInstance(2, resV*resU);
   T uv[2];

   // Create a regular grid of vertices
   for (MeshIndex yi = 0, i = 0; yi < resV; ++yi)
   {
      for (MeshIndex xi = 0; xi < resU; ++xi, ++i)
      {
         uv[0] = static_cast<T>(xi) / resU;
         uv[1] = static_cast<T>(yi) / resV;

         vtx[0] = (majorRadius + minorRadius*cos(uv[1] * twoPi)) * cos(uv[0] * twoPi);
         vtx[1] = (majorRadius + minorRadius*cos(uv[1] * twoPi)) * sin(uv[0] * twoPi);
         vtx[2] = minorRadius * sin(uv[1] * twoPi);

         mesh->setVertex(i, vtx);
         texCoords->setPoint(i, uv);
      }
   }

   MeshIndex ns[4];

   unsigned int fi = 0;
   for (unsigned int yi = 0; yi < resV; ++yi)
   {
      for (unsigned int xi = 0; xi < resU; ++xi)
      {
         ns[0] = resU*((yi+0)%resV) + ((xi+0)%resU);
         ns[1] = resU*((yi+0)%resV) + ((xi+1)%resU);
         ns[2] = resU*((yi+1)%resV) + ((xi+0)%resU);
         ns[3] = resU*((yi+1)%resV) + ((xi+1)%resU);

         mesh->setFaceIndices(fi++, ns[0], ns[1], ns[2]);
         mesh->setFaceIndices(fi++, ns[1], ns[3], ns[2]);
      }
   }

   mesh->setTextureCoords(texCoords);
}

template <class T>
void AWT::MeshGenerator<T>::generatePlane(typename AWT::Mesh<T>::P mesh, MeshIndex resU, MeshIndex resV)
{
   T vtx[3];

   const T twoPi = AWT::Constants<T>::Pi * 2;

   // Create a regular grid of vertices
   for (MeshIndex yi = 0; yi < resV; ++yi)
   {
      for (MeshIndex xi = 0; xi < resU; ++xi)
      {
         vtx[0] = static_cast<T>(xi);
         vtx[1] = static_cast<T>(yi);
         vtx[2] = 0;

         mesh->setVertex(mesh->getNumberOfVertices(), vtx);
      }
   }

   MeshIndex ns[4];

   unsigned int fi = 0;
   for (unsigned int yi = 0; yi < resV-1; ++yi)
   {
      for (unsigned int xi = 0; xi < resU-1; ++xi)
      {
         ns[0] = resU*((yi+0)%resV) + ((xi+0)%resU);
         ns[1] = resU*((yi+0)%resV) + ((xi+1)%resU);
         ns[2] = resU*((yi+1)%resV) + ((xi+0)%resU);
         ns[3] = resU*((yi+1)%resV) + ((xi+1)%resU);

         mesh->setFaceIndices(fi++, ns[0], ns[1], ns[2]);
         mesh->setFaceIndices(fi++, ns[1], ns[3], ns[2]);
      }
   }
}

template class AWT::MeshGenerator<double>;
template class AWT::MeshGenerator<float>;