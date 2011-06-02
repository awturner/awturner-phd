#ifndef __ANDCUTTINGFUNCTION_H__
#define __ANDCUTTINGFUNCTION_H__

#include "CuttingFunction.h"

namespace AWT
{
   template <class T>
   class AndCuttingFunction : public CuttingFunction<T>
   {
   public:
      typedef ManagedAutoPointer<AndCuttingFunction<T>> P;

   protected:
      AndCuttingFunction( CuttingFunction<T>::P funcA, CuttingFunction<T>::P funcB );
      virtual ~AndCuttingFunction( );

   public:
      static P getInstance( CuttingFunction<T>::P funcA, CuttingFunction<T>::P funcB );
      virtual std::string getClassName( ) const;

      virtual bool accept( T* vtx );

   protected:
      struct D;
      D* m_D;

   };
}

#endif // __ANDCUTTINGFUNCTION_H__