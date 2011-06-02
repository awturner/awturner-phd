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
