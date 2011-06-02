#ifndef __NONECOSTFUNCTION_H__
#define __NONECOSTFUNCTION_H__

#include "EigenvalueCostFunction.h"

namespace AWT
{
   namespace AlignParametric
   {
      class NoneCostFunction : public EigenvalueCostFunction
      {
      public:
         typedef ManagedAutoPointer<NoneCostFunction> P;

      protected:
         NoneCostFunction( );
         virtual ~NoneCostFunction( );

      public:
         static P getInstance( );
         virtual std::string getClassName( ) const;

         virtual T calculate( const VectorType& eigenvalues, VectorType& E_gradients );
      };
   }
}

#endif // __NONECOSTFUNCTION_H__