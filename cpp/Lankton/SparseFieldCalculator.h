#ifndef __SPARSEFIELDCALCULATOR_H__
#define __SPARSEFIELDCALCULATOR_H__

#include "ObjectManagement/ManagedObject.h"

#include "SparseFieldSolver.h"

namespace AWT
{
   class SparseFieldCalculator : public ManagedObject
   {
   public:
      typedef ManagedAutoPointer<SparseFieldCalculator> P;

   protected:
      SparseFieldCalculator( );
      virtual ~SparseFieldCalculator( );

   public:
      virtual PhiType calculateForce( const SegLabel label, const SubIndexType& pos ) = 0;
   };
}

#endif // __SPARSEFIELDCALCULATOR_H__