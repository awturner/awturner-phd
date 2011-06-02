#ifndef __AWTSTACK_H__
#define __AWTSTACK_H__

namespace AWT
{
   namespace Container
   {
      template <class T>
      class Iterator;

      template <class T>
      class Stack
      {
      public:
         static Stack<T>* New( );

         virtual const bool isEmpty( )        = 0;
         virtual const int  size( )           = 0;
         virtual const T    top( )            = 0;

         virtual void    push( const T data ) = 0;
         virtual const T pop( )               = 0;

         virtual Iterator<T>* iterator( )     = 0;
      };
   }
}

#endif // __AWTSTACK_H__