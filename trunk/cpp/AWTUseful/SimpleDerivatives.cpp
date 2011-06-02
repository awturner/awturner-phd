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
#include "AWTUseful/SimpleDerivatives.h"

template <class T>
T AWT::SimpleDerivatives<T>::firstDerivative( T in_v1, T in_v2 )
{
   return in_v2 - in_v1;
}

template <class T>
T AWT::SimpleDerivatives<T>::firstDerivative( T in_v1, T in_v2, T in_spacing )
{
   return firstDerivative( in_v1, in_v2 ) / in_spacing;
}

template <class T>
T AWT::SimpleDerivatives<T>::secondDerivative( T in_v1, T in_v2, T in_v3 )
{
   return in_v1 - 2*in_v2 + in_v3;
}

template <class T>
T AWT::SimpleDerivatives<T>::secondDerivative( T in_v1, T in_v2, T in_v3, T in_spacing )
{
   return secondDerivative( in_v1, in_v2, in_v3 ) / ( in_spacing * in_spacing );
}

template <class T>
T AWT::SimpleDerivatives<T>::crossDerivative(T in_v11, T in_v12, T in_v21, T in_v22, T in_spacing1, T in_spacing2 )
{
   return crossDerivative( in_v11, in_v12, in_v21, in_v22 ) / ( in_spacing1 * in_spacing2 );
}

template <class T>
T AWT::SimpleDerivatives<T>::crossDerivative(T in_v11, T in_v12, T in_v21, T in_v22 )
{
   return (( in_v22 - in_v21 ) - ( in_v12 - in_v11 ));
}

template AWT::SimpleDerivatives<double>;
template AWT::SimpleDerivatives<float>;
template AWT::SimpleDerivatives<long>;
template AWT::SimpleDerivatives<int>;
template AWT::SimpleDerivatives<unsigned char>;