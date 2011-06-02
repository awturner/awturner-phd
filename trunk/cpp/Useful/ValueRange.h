#ifndef __VALUERANGE_H__
#define __VALUERANGE_H__

#include <algorithm>

namespace AWT
{
   template <class T>
   struct ValueRange
   {
      T lower;
      T upper;

      const T clamp( const T& v ) const
      {
         return std::max<T>( lower, std::min<T>( upper, v ) );
      }
   };
}

#endif // __VALUERANGE_H__