#include "PointQueries.h"

template <class T>
T AWT::PointQueries<T>::getDistanceSquared( const T* in_A, const T* in_B, const T in_FastBreak )
{
   T dist2 = 0;
   T tmp;

   for ( int i = 0; i < 3; ++i )
   {
      tmp = in_A[i] - in_B[i];
      dist2 += tmp*tmp;

      if ( dist2 >= in_FastBreak )
         break;
   }

   return dist2;
}

template class AWT::PointQueries<double>;
template class AWT::PointQueries<float>;