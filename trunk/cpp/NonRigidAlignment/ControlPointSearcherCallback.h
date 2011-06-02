#ifndef __CONTROLPOINTSEARCHERCALLBACK_H__
#define __CONTROLPOINTSEARCHERCALLBACK_H__

#include "ObjectManagement/ManagedObject.h"
#include "Mesh/MeshTypeDefs.h"

namespace AWT
{
   template <class T>
   class ControlPointSearcherCallback : public virtual ManagedObject
   {
   public:
      typedef ManagedAutoPointer<ControlPointSearcherCallback<T>> P;

      virtual void controlPointFound( const MeshIndex index ) = 0;
   };
}

#endif __CONTROLPOINTSEARCHERCALLBACK_H__