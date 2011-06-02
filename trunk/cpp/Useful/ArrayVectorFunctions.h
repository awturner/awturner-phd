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
#ifndef __ARRAYVECTORFUNCTIONS_H__
#define __ARRAYVECTORFUNCTIONS_H__

#include <cmath>

namespace AWT
{
   template <class T>
   void assign(const T* a, T* b, const unsigned int n)
   {
      for (unsigned int i = 0; i < n; ++i)
         b[i] = a[i];
   }

   template <class T>
   void scale(T* a, const T scale, const unsigned int n)
   {
      for (unsigned int i = 0; i < n; ++i)
         a[i] *= scale;
   }

   template <class T>
   T dot(const T* a, const T* b, const unsigned int n)
   {
      T ret = 0;

      for (unsigned int i = 0; i < n; ++i)
         ret += a[i]*b[i];

      return ret;
   }

   template <class T>
   T norm(const T* a, const unsigned int n)
   {
      return sqrt(dot(a, a, n));
   }

   template <class T>
   T deltaNorm(const T* a, const T* b, const unsigned int n)
   {
      return sqrt(deltaNormSquared(a, b, n));
   }

   template <class T>
   T deltaNormSquared(const T* a, const T* b, const unsigned int n)
   {
      T ret = 0;

      for (unsigned int i = 0; i < n; ++i)
      {
         T tmp = a[i] - b[i];

         ret += tmp * tmp;
      }

      return ret;
   }

   template <class T>
   T normalize(T* a, const unsigned int n)
   {
      T lengthSquared = dot(a, a, n);

      if (lengthSquared != 0)
      {
         T length = sqrt(lengthSquared);
         //if (length == 0)
         //   DEBUGMACRO("Warning: normalizing a vector with length zero");

         for (unsigned int i = 0; i < n; ++i)
            a[i] /= length;

         return length;
      }

      return lengthSquared;
   }

   template <class T>
   void cross(const T* a, const T* b, T* out_c)
   {
      out_c[0] = a[1]*b[2] - a[2]*b[1];
      out_c[1] = a[2]*b[0] - a[0]*b[2];
      out_c[2] = a[0]*b[1] - a[1]*b[0];
   }

   template <class T>
   void quaternionMultiply(const T* p, const T* quat, T* out_p)
   {
      out_p[0] = (quat[0]*quat[0] + quat[1]*quat[1] - quat[2]*quat[2] - quat[3]*quat[3])*p[0] +                                   2*(quat[1]*quat[2] + quat[0]*quat[3])*p[1] +                                   2*(quat[1]*quat[3] - quat[0]*quat[2])*p[2];
      out_p[1] =                                   2*(quat[1]*quat[2] - quat[0]*quat[3])*p[0] + (quat[0]*quat[0] - quat[1]*quat[1] + quat[2]*quat[2] - quat[3]*quat[3])*p[1] +                                   2*(quat[2]*quat[3] + quat[0]*quat[1])*p[2];
      out_p[2] =                                   2*(quat[1]*quat[3] + quat[0]*quat[2])*p[0] +                                   2*(quat[2]*quat[3] - quat[0]*quat[3])*p[1] + (quat[0]*quat[0] - quat[1]*quat[1] - quat[2]*quat[2] + quat[3]*quat[3])*p[2];
   }

   template <class T>
   void rotate(const T x[3], const T R[3][3], T b[3])
   {
      FOREACHAXIS(ax)
         b[ax] = R[0][ax]*x[0] + R[1][ax]*x[1] + R[2][ax]*x[2];
   }

   template <class T>
   void unrotate(const T x[3], const T R[3][3], T b[3])
   {
      T Ri[3][3];

      FOREACHAXIS(i) 
         FOREACHAXIS(j)
            Ri[i][j] = R[i][j];

      transpose3x3(Ri);

      rotate(x, Ri, b);
   }

   template <class T>
   void project(T x[3], const T R[3][3], const T t[3])
   {
      T pnt[3];
      project(x, R, t, pnt);

      assign(pnt, x, 3);
   }

   template <class T>
   void project(const T x[3], const T R[3][3], const T t[3], T b[3])
   {
      FOREACHAXIS(ax)
         b[ax] = R[0][ax]*x[0] + R[1][ax]*x[1] + R[2][ax]*x[2] + t[ax];
   }

   template <class T>
   void unproject(const T x[3], const T R[3][3], const T t[3], T b[3])
   {
      T Ri[3][3];
      T ti[3];

      unprojectParameters(R, t, Ri, ti);

      project(x, Ri, ti, b);
   }

   template <class T>
   void unprojectParameters(const T R[3][3], const T t[3], T Ri[3][3], T ti[3])
   {
      // First, invert the rotation
      FOREACHAXIS(i) 
         FOREACHAXIS(j)
            Ri[i][j] = R[j][i];

      // Second, invert the translation
      rotate(t, Ri, ti);
      FOREACHAXIS(ax)
         ti[ax] = -ti[ax];
   }

   template <class T>
   T det3x3(const T m[3][3])
   {
      T ret = 0;

      for (int i = 0; i < 3; ++i)
         ret += m[i][0] * (m[(i+1)%3][1]*m[(i+2)%3][2] - m[(i+1)%3][2]*m[(i+2)%3][1]);

      return ret;
   }

   template <class T>
   void pseudoInvert3x2(const T a[3][2], T d[2][3])
   {
      T det = a[0][0]*a[0][0]*a[1][1]*a[1][1]+a[0][0]*a[0][0]*a[2][1]*a[2][1]+a[1][0]*a[1][0]*a[0][1]*a[0][1]+a[1][0]*a[1][0]*a[2][1]*a[2][1]+a[2][0]*a[2][0]*a[0][1]*a[0][1]+a[2][0]*a[2][0]*a[1][1]*a[1][1]-a[0][0]*a[0][1]*a[1][1]*a[1][0]-a[0][0]*a[0][1]*a[2][1]*a[2][0]-a[1][0]*a[1][1]*a[0][1]*a[0][0]-a[1][0]*a[1][1]*a[2][1]*a[2][0]-a[2][0]*a[2][1]*a[0][1]*a[0][0]-a[2][0]*a[2][1]*a[1][1]*a[1][0];

      d[0][0] = ((a[0][1]*a[0][1]+a[1][1]*a[1][1]+a[2][1]*a[2][1])*a[0][0]-(a[0][0]*a[0][1]+a[1][0]*a[1][1]+a[2][0]*a[2][1])*a[0][1])/det;
      d[0][1] = ((a[0][1]*a[0][1]+a[1][1]*a[1][1]+a[2][1]*a[2][1])*a[1][0]-(a[0][0]*a[0][1]+a[1][0]*a[1][1]+a[2][0]*a[2][1])*a[1][1])/det;
      d[0][2] = ((a[0][1]*a[0][1]+a[1][1]*a[1][1]+a[2][1]*a[2][1])*a[2][0]-(a[0][0]*a[0][1]+a[1][0]*a[1][1]+a[2][0]*a[2][1])*a[2][1])/det;

      d[1][0] = (-(a[0][1]*a[0][0]+a[1][1]*a[1][0]+a[2][1]*a[2][0])*a[0][0]+(a[0][0]*a[0][0]+a[1][0]*a[1][0]+a[2][0]*a[2][0])*a[0][1])/det;
      d[1][1] = (-(a[0][1]*a[0][0]+a[1][1]*a[1][0]+a[2][1]*a[2][0])*a[1][0]+(a[0][0]*a[0][0]+a[1][0]*a[1][0]+a[2][0]*a[2][0])*a[1][1])/det;
      d[1][2] = (-(a[0][1]*a[0][0]+a[1][1]*a[1][0]+a[2][1]*a[2][0])*a[2][0]+(a[0][0]*a[0][0]+a[1][0]*a[1][0]+a[2][0]*a[2][0])*a[2][1])/det;
   }

   template <class T>
   void fill(T* a, const T val, const unsigned int n)
   {
      for (unsigned int i = 0; i < n; ++i)
         a[i] = val;
   }

   namespace HeapSortAuxilliaries
   {
      template <class T>
      void swap(T& a, T& b)
      {
         T tmp = a;
         a = b;
         b = tmp;
      }

      template <class T>
      void heapify(T* a, unsigned int count)
      {
         unsigned int start = (count - 2) / 2;

         while (true)
         {
            siftDown(a, start, count-1);

            if (start == 0)
               return;

            --start;
         }
      }

      template <class T>
      void siftDown(T* a, unsigned int start, unsigned int end)
      {
         unsigned int root = start;

         unsigned int child;
         while ((child = 2*root + 1) <= end)
         {
            if (child+1 <= end && a[child] < a[child+1])
            {
               ++child;
            }

            if (a[root] < a[child])
            {
               swap(a[root], a[child]);
               root = child;
            }
            else
            {
               return;
            }
         }
      }
   }

   template <class T>
   void heapSort(T* a, unsigned int count)
   {
      using namespace HeapSortAuxilliaries;

      heapify(a, count);

      unsigned int end = count - 1;

      while (end > 0)
      {
         swap(a[end], a[0]);
         --end;
         siftDown(a, 0, end);
      }
   }
}

#include <vnl/vnl_vector.h>
#include "Useful/Exception.h"

template <class T>
vnl_vector<T> vectorslice(const vnl_vector<T>& vec, const unsigned int start, const unsigned int len)
{
   if (vec.size() < (start+len))
      AWTEXCEPTIONTHROW("Vector is too small");

   vnl_vector<T> ret(len);
   for (unsigned int i = 0; i < len; ++i)
      ret(i) = vec(i+start);

   return ret;
}

template <class T>
void setvectorslice(vnl_vector<T>& vec, const vnl_vector<T>& data, const unsigned int start)
{
   if (vec.size() < (start+data.size()))
      AWTEXCEPTIONTHROW("Vector is too small");

   for (unsigned int i = 0; i < data.size(); ++i)
      vec(i+start) = data(i);
}

#endif // __ARRAYVECTORFUNCTIONS_H__