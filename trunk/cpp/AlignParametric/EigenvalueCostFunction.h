#ifndef __EIGENVALUECOSTFUNCTION_H__
#define __EIGENVALUECOSTFUNCTION_H__

#include "ObjectManagement/ManagedObject.h"

#include "TypeDefs.h"

namespace AWT
{
   namespace AlignParametric
   {
      class EigenvalueCostFunction : public ManagedObject
      {
      public:
         typedef ManagedAutoPointer<EigenvalueCostFunction> P;

         virtual T calculate( const VectorType& eigenvalues, VectorType& E_gradients ) = 0;
      };
   }
}

#endif // __EIGENVALUECOSTFUNCTION_H__