#ifndef __ORCUTTINGFUNCTION_H__
#define __ORCUTTINGFUNCTION_H__

#include "CuttingFunction.h"

namespace AWT
{
   template <class T>
   class OrCuttingFunction : public CuttingFunction<T>
   {
   public:
      typedef ManagedAutoPointer<OrCuttingFunction<T>> P;

   protected:
      OrCuttingFunction( CuttingFunction<T>::P funcA, CuttingFunction<T>::P funcB );
      virtual ~OrCuttingFunction( );

   public:
      static P getInstance( CuttingFunction<T>::P funcA, CuttingFunction<T>::P funcB );
      virtual std::string getClassName( ) const;

      virtual bool accept( T* vtx );

   protected:
      struct D;
      D* m_D;

   };
}

#endif // __ORCUTTINGFUNCTION_H__