#ifndef __MANAGEDOBJECT_H__
#define __MANAGEDOBJECT_H__

#include "ModifiedTimeObject.h"
#include "ReferenceCountedObject.h"
#include "ManagedAutoPointer.h"
#include <string>

namespace AWT
{
   class ManagedObject : public virtual ModifiedTimeObject, public virtual ReferenceCountedObject
   {
   public:
      typedef ManagedAutoPointer<ManagedObject> P;
   };
}

#endif // __MANAGEDOBJECT_H__
