#ifndef __REFINEMENTSTRATEGY_H__
#define __REFINEMENTSTRATEGY_H__

#include "ObjectManagement/ManagedObject.h"
#include "TypeDefs.h"
#include "Useful/Iterator.h"

namespace AWT
{
   namespace AlignParametric
   {
      class RefinementStrategy : public ManagedObject
      {
      public:
         typedef ManagedAutoPointer<RefinementStrategy> P;
         virtual bool refine( AWT::Container::Iterator<Idx>::P iter ) = 0;
      };
   }
}

#endif // __REFINEMENTSTRATEGY_H__