#ifndef __AWT_LIST_H__
#define __AWT_LIST_H__

#include "AWTUseful/Iterable.h"

namespace AWT
{
   namespace Container
   {
      template <class T>
      class List : public Iterable<T>
      {
      public:
         static List* New( );

         // Information methods
         virtual const bool isEmpty( )    = 0;
         virtual const int  size( )       = 0;
         virtual const T    get( int )    = 0;

         // List structure changing methods
         virtual void add( const T )      = 0;
         virtual void add( int, const T ) = 0;
         virtual T    remove( int )       = 0;
         virtual void clear( )            = 0;

         // For the iterable interface
         virtual Iterator<T>* iterator( ) = 0;
      };
   }
}

#endif // __AWT_LIST_H__