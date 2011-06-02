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
#include "AWTUseful/LevelSets.h"
#include "AWTUseful/SimpleDerivatives.h"

template <class T>
T AWT::LevelSets<T>::meanCurvatureTimesGradientMagnitude(T *in_values, T *in_spacing)
{
   int i;
   T grad[3];
   T gradSq[3];
   T denom = 0;

   grad[0] = AWT::SimpleDerivatives<T>::firstDerivative( in_values[12], in_values[14], 2*in_spacing[0] );
   grad[1] = AWT::SimpleDerivatives<T>::firstDerivative( in_values[10], in_values[16], 2*in_spacing[1] );
   grad[2] = AWT::SimpleDerivatives<T>::firstDerivative( in_values[ 4], in_values[22], 2*in_spacing[2] );

   for ( i = 0; i < 3; i++ )
   {
      gradSq[i] = grad[i] * grad[i];
      denom += gradSq[i];
   }

   T H = 0;

   if ( denom != 0 )
   {
      T grad2_x = AWT::SimpleDerivatives<T>::secondDerivative( 
         in_values[12], in_values[13], in_values[14], in_spacing[0] );
      T grad2_y = AWT::SimpleDerivatives<T>::secondDerivative( 
         in_values[10], in_values[13], in_values[16], in_spacing[1] );
      T grad2_z = AWT::SimpleDerivatives<T>::secondDerivative( 
         in_values[ 4], in_values[13], in_values[22], in_spacing[2] );

      T grad_xy = AWT::SimpleDerivatives<T>::crossDerivative( in_values[ 9], 
         in_values[11], in_values[15], in_values[17], 2*in_spacing[0], 2*in_spacing[1] );
      T grad_xz = AWT::SimpleDerivatives<T>::crossDerivative( in_values[ 3], 
         in_values[ 5], in_values[21], in_values[23], 2*in_spacing[0], 2*in_spacing[2] );
      T grad_yz = AWT::SimpleDerivatives<T>::crossDerivative( in_values[ 1], 
         in_values[ 7], in_values[19], in_values[25], 2*in_spacing[1], 2*in_spacing[2] );

      H = ( ( gradSq[1] + gradSq[2] ) * grad2_x +
            ( gradSq[0] + gradSq[2] ) * grad2_y +
            ( gradSq[0] + gradSq[1] ) * grad2_z -
            2 * ( grad[0] * grad[1] * grad_xy +
                  grad[0] * grad[2] * grad_xz +
                  grad[1] * grad[2] * grad_yz ) ) / denom;
   }

   return H;
}

template AWT::LevelSets<double>;
template AWT::LevelSets<float>;
template AWT::LevelSets<long>;
template AWT::LevelSets<int>;
template AWT::LevelSets<unsigned char>;