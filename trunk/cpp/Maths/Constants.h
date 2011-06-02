#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

namespace AWT
{
   template <class T>
   class Constants
   {
   public:
      static const T Pi;

      static const T DegreesToRadians;

      static const T RadiansToDegrees;
   };

   template <class T>
   const T AWT::Constants<T>::Pi = atan( static_cast<T>( 1 ) ) * 4;

   template <class T>
   const T AWT::Constants<T>::DegreesToRadians = ( atan( static_cast<T>( 1 ) ) * 4 ) / 180;

   template <class T>
   const T AWT::Constants<T>::RadiansToDegrees = 180 / ( atan( static_cast<T>( 1 ) ) * 4 );
}

#endif // __CONSTANTS_H__