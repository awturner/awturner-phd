#ifndef __NOTCUTTINGFUNCTION_H__
#define __NOTCUTTINGFUNCTION_H__

#include "CuttingFunction.h"

namespace AWT
{
   template <class T>
   class NotCuttingFunction : public CuttingFunction<T>
   {
   public:
      typedef ManagedAutoPointer<NotCuttingFunction<T>> P;

   protected:
      NotCuttingFunction( CuttingFunction<T>::P func );
      virtual ~NotCuttingFunction( );

   public:
      static P getInstance( CuttingFunction<T>::P func );
      virtual std::string getClassName( ) const;

      virtual bool accept( T* vtx );

   protected:
      struct D;
      D* m_D;

   };
}

#endif // __NOTCUTTINGFUNCTION_H__