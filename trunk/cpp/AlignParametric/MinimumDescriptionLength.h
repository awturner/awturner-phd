#ifndef __MINIMUMDESCRIPTIONLENGTH_H__
#define __MINIMUMDESCRIPTIONLENGTH_H__

#include "EigenvalueCostFunction.h"

namespace AWT
{
   namespace AlignParametric
   {
      class MinimumDescriptionLength : public EigenvalueCostFunction
      {
      public:
         typedef ManagedAutoPointer<MinimumDescriptionLength> P;

      protected:
         MinimumDescriptionLength( );
         virtual ~MinimumDescriptionLength( );

      public:
         static P getInstance( );
         virtual std::string getClassName( ) const;

         virtual T getLambdaCut( ) const;
         virtual void setLambdaCut( const T v );

         virtual T calculate( const VectorType& eigenvalues, VectorType& E_gradients );

      protected:
         struct D;
         D* m_D;

      };
   }
}

#endif // __MINIMUMDESCRIPTIONLENGTH_H__