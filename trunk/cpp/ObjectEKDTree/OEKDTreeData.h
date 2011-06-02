#ifndef __OBJECTEKDTREEDATA_H__
#define __OBJECTEKDTREEDATA_H__

#include "ObjectManagement/ManagedObject.h"

#include "OEKDTypeDefs.h"

namespace AWT
{
   namespace OEKDTree
   {
      template <class T, AxisIndex K>
      class OEKDTreeData : public virtual ManagedObject
      {
      public:
         typedef ManagedAutoPointer< OEKDTreeData<T,K> > P;

      public:
         virtual ObjectIndex getNumberOfObjects( ) const = 0;

         virtual T getCoordinate( ObjectIndex objectIndex, AxisIndex axis ) const = 0;
         virtual void getPosition( const ObjectIndex objectIndex, T* pos ) const = 0;

         virtual T getMinimumBound( ObjectIndex objectIndex, AxisIndex axis ) const = 0;
         virtual T getMaximumBound( ObjectIndex objectIndex, AxisIndex axis ) const = 0;
      };
   }
}

#endif __OBJECTEKDTREEDATA_H__
