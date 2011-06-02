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
#include "TriangleIntersection.h"
#include "TriangleIntersection_Moller.h"

template <class T>
bool AWT::TriangleIntersection<T>::intersects( T u0[3], T u1[3], T u2[3],
         T v0[3], T v1[3], T v2[3],
         T* intersection0, T* intersection1 )
{
   int coplanar;
   int result = MollerTriangleIntersection<T>::tri_tri_intersect_with_isectline( u0, u1, u2, v0, v1, v2, &coplanar, intersection0, intersection1 );

   return result != 0;
}

template class AWT::TriangleIntersection<double>;
template class AWT::TriangleIntersection<float>;