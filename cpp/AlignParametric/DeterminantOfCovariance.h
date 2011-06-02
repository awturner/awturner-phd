#ifndef __DETERMINANTOFCOVARIANCE_H__
#define __DETERMINANTOFCOVARIANCE_H__

#include "EigenvalueCostFunction.h"

namespace AWT
{
   namespace AlignParametric
   {
      class DeterminantOfCovariance : public EigenvalueCostFunction
      {
      public:
         typedef ManagedAutoPointer<DeterminantOfCovariance> P;

      protected:
         DeterminantOfCovariance( );
         virtual ~DeterminantOfCovariance( );

      public:
         static P getInstance( );
         virtual std::string getClassName( ) const;

         virtual T getEpsilon( ) const;
         virtual void setEpsilon( const double v );

         virtual T calculate( const VectorType& eigenvalues, VectorType& E_gradients );

      protected:
         struct D;
         D* m_D;

      };
   }
}

#endif // __DETERMINANTOFCOVARIANCE_H__