#ifndef __OPTIMIZATIONFUNCTION_H__
#define __OPTIMIZATIONFUNCTION_H__

#include "ObjectManagement/ManagedObject.h"

#include "vnl/vnl_matrix.h"

namespace AWT
{
   template <class T>
   class OptimizationFunction : public ManagedObject
   {
   public:
      typedef ManagedAutoPointer<OptimizationFunction<T>> P;

      virtual unsigned int getNumberOfVariables( ) const = 0;

      virtual T evaluate( const vnl_matrix<T>& vs ) const = 0;
   };
}

#endif // __OPTIMIZATIONFUNCTION_H__