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
#ifndef __GAUSSIAN_H__
#define __GAUSSIAN_H__

#include <limits.h>

namespace AWT
{
   template <class T>
   static T deltaIsotropicGaussian( const T vtxA[3], const T vtxB[3], const T sigma, const unsigned int dims )
   {
      return truncatedDeltaGaussian( vtxA, vtxB, sigma, std::numeric_limits<T>::infinity() );
   }

   template <class T>
   static T truncatedDeltaIsotropicGaussian( const T* vtxA, const T* vtxB, const T sigma, const T cutoff, const unsigned int dims )
   {
      const T dns = deltaNormSquared<T>( vtxA, vtxB, dims );

      return truncatedIsotropicGaussian( dns, sigma, cutoff, dims );
   }

   template <class T>
   static T truncatedIsotropicGaussian( const T dns, const T sigma, const T cutoff, const unsigned int dims )
   {
      const T SQRT_TWOPI = sqrt( 2 * 4*atan(1.0) );

      // Truncate to make exactly zero over most space
      if ( dns > cutoff*cutoff )
         return 0;

      const T div = pow( SQRT_TWOPI*sigma, static_cast<double>(dims) );
      const T ret = exp( -dns / (2*sigma*sigma) ) / div;

      return ret;
   }
}

#endif // __GAUSSIAN_H__