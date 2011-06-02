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
#include "TriangleProjection.h"

#include "Useful/ArrayVectorFunctions.h"

template <class T>
struct AWT::TriangleProjection<T>::D
{
};

template <class T>
AWT::TriangleProjection<T>::TriangleProjection()
{
   m_D = new D;
}

template <class T>
AWT::TriangleProjection<T>::~TriangleProjection()
{
   delete m_D;
}

template <class T>
GETNAMEMACRO(AWT::TriangleProjection<T>);

template <class T>
void AWT::TriangleProjection<T>::projectFromTriangle(const typename AWT::Mesh<T>::P in_Mesh, const AWT::MeshIndex in_Index, const T* vertex2d, T* vertex3d)
{
   T vtxOrigin[3], vtxA[3], vtxB[3];

   in_Mesh->getFace(in_Index, vtxOrigin, vtxA, vtxB);

   projectFromTriangle(vtxOrigin, vtxA, vtxB, vertex2d, vertex3d);
}

template <class T>
void AWT::TriangleProjection<T>::projectFromTriangle(const T* vtxOrigin, const T* vtxX, const T* vtxY, const T* vertex2d, T* vertex3d)
{
   FOREACHAXIS(ax)
      vertex3d[ax] = vtxOrigin[ax] + (vtxX[ax] - vtxOrigin[ax])*vertex2d[0] + (vtxY[ax] - vtxOrigin[ax])*vertex2d[1];
}

template <class T>
void AWT::TriangleProjection<T>::projectOntoTriangle(const typename AWT::Mesh<T>::P in_Mesh, const AWT::MeshIndex in_Index, const T* vertex3d, T* vertex2d)
{
   T vtxOrigin[3], vtxA[3], vtxB[3];

   in_Mesh->getFace(in_Index, vtxOrigin, vtxA, vtxB);

   projectOntoTriangle(vtxOrigin, vtxA, vtxB, vertex3d, vertex2d);
}

template <class T>
void AWT::TriangleProjection<T>::projectOntoTriangle(const T* vtxOrigin, const T* vtxX, const T* vtxY, const T* vertex3d, T* vertex2d)
{
   T vtxA[3], vtxB[3], vtxC[3];

   FOREACHAXIS(ax)
   {
      vtxA[ax] = vtxX[ax] - vtxOrigin[ax];
      vtxB[ax] = vtxY[ax] - vtxOrigin[ax];
   }

   cross<T>(vtxA, vtxB, vtxC);

   T det =   vtxA[0] * (vtxB[1]*vtxC[2] - vtxC[1]*vtxB[2])
           - vtxA[1] * (vtxB[0]*vtxC[2] - vtxC[0]*vtxB[2])
           + vtxA[2] * (vtxB[0]*vtxC[1] - vtxC[0]*vtxB[1]);

   // Invert [ vtxA vtxB vtxC ]
   vertex2d[0] = ((vtxB[1]*vtxC[2]-vtxC[1]*vtxB[2])*(vertex3d[0] - vtxOrigin[0]) + -(vtxB[0]*vtxC[2]-vtxC[0]*vtxB[2])*(vertex3d[1] - vtxOrigin[1]) +  (vtxB[0]*vtxC[1]-vtxC[0]*vtxB[1])*(vertex3d[2] - vtxOrigin[2])) / det;
   vertex2d[1] = (-(vtxA[1]*vtxC[2]-vtxC[1]*vtxA[2])*(vertex3d[0] - vtxOrigin[0]) +  (vtxA[0]*vtxC[2]-vtxC[0]*vtxA[2])*(vertex3d[1] - vtxOrigin[1]) + -(vtxA[0]*vtxC[1]-vtxC[0]*vtxA[1])*(vertex3d[2] - vtxOrigin[2])) / det;
}

template class AWT::TriangleProjection<double>;
template class AWT::TriangleProjection<float>;