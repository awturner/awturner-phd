#ifndef __CUTTINGFUNCTION_H__
#define __CUTTINGFUNCTION_H__

#include "ObjectManagement/ManagedObject.h"

namespace AWT
{
   template <class T>
   class CuttingFunction : public ManagedObject
   {
   public:
      typedef ManagedAutoPointer<CuttingFunction<T>> P;

   public:
      virtual bool accept( T* vtx ) = 0;
   };
}

#endif // __CUTTINGFUNCTION_H__