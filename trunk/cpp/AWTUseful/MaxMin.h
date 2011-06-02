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