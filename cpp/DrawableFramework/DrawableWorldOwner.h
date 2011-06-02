#ifndef __DRAWABLEWORLDOWNER_H__
#define __DRAWABLEWORLDOWNER_H__

#include "ObjectManagement/ManagedObject.h"

namespace AWT
{
   class DrawableWorld;

   class DrawableWorldOwner : public ManagedObject
   {
   public:
      typedef ManagedAutoPointer<DrawableWorldOwner> P;

   public:
      virtual DrawableWorld* getDrawableWorld( ) = 0;
   };
}

#endif // __DRAWABLEWORLDOWNER_H__