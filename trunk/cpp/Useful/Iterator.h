#ifndef __AWT_ITERATOR_H__
#define __AWT_ITERATOR_H__

#include "ObjectManagement/ManagedObject.h"

namespace AWT
{
   namespace Container
   {
      template <class T>
      class Iterator : public virtual ManagedObject
      {
      public:
         typedef ManagedAutoPointer<Iterator<T>> P;

      public:
         virtual bool hasNext( ) = 0;
         virtual T next( ) = 0;
      };
   }

}

#endif // __AWT_ITERATOR_H__