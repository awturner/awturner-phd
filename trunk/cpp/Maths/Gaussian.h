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