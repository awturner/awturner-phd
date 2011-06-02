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
#ifndef __MAXMIN_H__
#define __MAXMIN_H__

namespace AWT
{
   template <class T>
   class MaxMin
   {
   public:
      static T minimum( T a, T b );
      static T maximum( T a, T b );

      static T minimum( T* in_values, int in_cnt );
      static T maximum( T* in_values, int in_cnt );
   };
}

template <class T>
T AWT::MaxMin<T>::minimum( T a, T b )
{
   return ( a < b ) ? a : b;
}

template <class T>
T AWT::MaxMin<T>::maximum( T a, T b )
{
   return ( a > b ) ? a : b;
}

template <class T>
T AWT::MaxMin<T>::minimum( T* in_value, int in_cnt )
{
   int idxMin = 0;

   for ( int i = 1; i < in_cnt; i++ )
   {
      if ( in_value[i] < in_value[idxMin] )
         idxMin = i;
   }

   return in_value[idxMin];
}

template <class T>
T AWT::MaxMin<T>::maximum( T* in_value, int in_cnt )
{
   int idxMax = 0;

   for ( int i = 1; i < in_cnt; i++ )
   {
      if ( in_value[i] > in_value[idxMax] )
         idxMax = i;
   }

   return in_value[idxMax];
}

#endif // __MAXMIN_H__