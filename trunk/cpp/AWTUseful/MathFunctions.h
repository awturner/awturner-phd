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
#ifndef __MathFunctions_H__
#define __MathFunctions_H__

#include <cmath>

namespace AWT
{
   template <class T>
   class MathFunctions
   {
   public:
      static const T pi;

      static T smearedHeaviside( T a, T b, T x);
   };
}

template <class T>
const T AWT::MathFunctions<T>::pi = static_cast<T>( atan(1.0) * 4.0 );

template <class T>
T AWT::MathFunctions<T>::smearedHeaviside( T a, T b, T x )
{
   if ( x < a )
      return 0;

   if ( x > b )
      return 1;

   T middle = ( a + b ) / 2.0;
   T s = ( x - middle ) / ( b - middle );

   return ( 1 + s + sin( s * pi ) / pi ) / 2;
}
#endif // __MathFunctions_H__
