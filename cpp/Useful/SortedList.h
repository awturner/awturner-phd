#ifndef __SortedList_H__
#define __SortedList_H__

#include "Useful/Iterable.h"
#include "Useful/Iterator.h"

namespace AWT
{
   namespace Container
   {
      template <class T>
      class SortedList : public Iterable<T>
      {
      public:
         virtual const bool isEmpty( )     = 0;
         virtual const int size( )         = 0;

         virtual void push( const T data ) = 0;
         virtual T    pop( )               = 0;
      };
   }
}

#endif // __SortedList_H__
