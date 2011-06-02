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
#ifndef __AWTMATRICES_H__
#define __AWTMATRICES_H__

#include "AWTUseful/Complex.h"

namespace AWT
{
   template <class T>
   class Matrix
   {
   public:
      static T determ3x3(T* in_matrix);
      static AWT::Complex<T> determ3x3(AWT::Complex<T>* in_matrix);

      static void multiply(T* in_m1, int rows1, int cols1, T* in_m2, int rows2, int cols2, T* out_res);
   };

   template <class T, unsigned int N>
   class Vector
   {
   public:
      static void add(T* in_p1, T* in_p2, T* out_p0);
      static void add(T* io_p1, T* in_p2);
      static void scale(T* in_p1, T in_scale);

      static void normalize(T* io_p1);

      static T length(T* in_p1);
      static T dotProduct(T* in_p1, T* in_p2);
      static void crossProduct(T* in_p1, T* in_p2, T* out_cross);
   };
}

template <class T>
T AWT::Matrix<T>::determ3x3(T* m)
{
   // Doesn't matter if matrix is in row- or column- major order.

   T det = m[0] * (m[4]*m[8] - m[5]*m[7])
         - m[3] * (m[1]*m[8] - m[2]*m[7])
         + m[6] * (m[1]*m[5] - m[2]*m[4]);

   return det;
}

template <class T>
AWT::Complex<T> AWT::Matrix<T>::determ3x3(AWT::Complex<T>* m)
{
   // Doesn't matter if matrix is in row- or column- major order.

   AWT::Complex<T> det = m[0] * (m[4]*m[8] - m[5]*m[7])
         - m[3] * (m[1]*m[8] - m[2]*m[7])
         + m[6] * (m[1]*m[5] - m[2]*m[4]);

   return det;
}

template <class T>
void AWT::Matrix<T>::multiply(T* in_m1, int rows1, int cols1, T* in_m2, int rows2, int cols2, T* out_res)
{
   if (cols1 != rows2)
      throw "Matrices are not compatible!";

   int r, c, j, idx;

   for (r = 0; r < rows1; r++)
   {
      for (c = 0; c < cols2; c++)
      {
         idx = rows1*r + c;
         out_res[ idx ] = 0;

         for (j = 0; j < cols1; j++)
            out_res[ idx ] += in_m1[ j*rows1 + r ] * in_m2[ c*rows2 + j ];
      }
   }
}

template <class T, unsigned int N>
void AWT::Vector<T,N>::add(T* in_p1, T* in_p2, T* out_p0)
{
   for (int i = 0; i < N; i++)
      out_p0[i] = in_p1[i] + in_p2[i];
}

template <class T, unsigned int N>
void AWT::Vector<T,N>::add(T* io_p1, T* in_p2)
{
   for (int i = 0; i < N; i++)
      io_p1[i] += in_p2[i];
}

template <class T, unsigned int N>
void AWT::Vector<T,N>::scale(T* io_p1, T scale)
{
   for (int i = 0; i < N; i++)
      io_p1[i] *= scale;
}

template <class T, unsigned int N>
T AWT::Vector<T,N>::length(T* in_p1)
{
   T ret = static_cast<T>(sqrt(static_cast<double>(dotProduct(in_p1, in_p1))));

   return ret;
}

template <class T, unsigned int N>
void AWT::Vector<T,N>::normalize(T* io_p1)
{
   T len = length(io_p1);
   scale(io_p1, 1.0 / len);
}

template <class T, unsigned int N>
T AWT::Vector<T,N>::dotProduct(T* in_p1, T* in_p2)
{
   T ret = 0;

   for (int i = 0; i < N; i++)
      ret += in_p1[i]*in_p2[i];

   return ret;
}

template <class T, unsigned int N>
void AWT::Vector<T,N>::crossProduct(T* in_p1, T* in_p2, T* out_cross)
{
   if (N != 3)
      throw "Only defined for 3-vectors!";

   out_cross[0] = in_p1[1]*in_p2[2] - in_p1[2]*in_p2[1];
   out_cross[1] = in_p2[0]*in_p1[2] - in_p2[2]*in_p1[0];
   out_cross[2] = in_p1[0]*in_p2[1] - in_p1[1]*in_p2[0];
}

#endif // #ifndef __AWTMATRICES_H__