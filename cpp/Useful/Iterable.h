#ifndef __ITERABLE_H__
#define __ITERABLE_H__

namespace AWT
{
   template <class T>
   class Iterator;

   namespace Container
   {
      template <class T>
      class Iterable
      {
      public:
         virtual Iterator<T>* iterator( ) = 0;
      };
   }
}
#endif // __ITERABLE_H__