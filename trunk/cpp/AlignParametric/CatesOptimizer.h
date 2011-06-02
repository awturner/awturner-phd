#ifndef __CatesOptimizer_H__
#define __CatesOptimizer_H__

#include "ModelEigenvalueOptimizer.h"
#include "Useful/ValueRange.h"
#include "CatesParticleSurface.h"
#include "ParticleOptimizer.h"

namespace AWT
{
   namespace AlignParametric
   {
      class CatesOptimizer : public ParticleOptimizer
      {
      public:
         typedef ManagedAutoPointer<CatesOptimizer> P;

      protected:
         CatesOptimizer( MeshIndex N, ValueRange<T> _sigmaRange );
         virtual ~CatesOptimizer( );

      public:
         static P getInstance( MeshIndex N, ValueRange<T> _sigmaRange );
         virtual std::string getClassName( ) const;

         virtual void setSigmaRange( ValueRange<T> range );
         virtual ValueRange<T> getSigmaRange( ) const;

         virtual void setDistanceType(const CatesRegularizer::DistanceType dt);
         virtual CatesRegularizer::DistanceType getDistanceType() const;

         virtual ParticleSurface::P addMesh( MeshType::P mesh, TuplesType::P particles );

      protected:
         ValueRange<T> sigmaRange;
         CatesRegularizer::DistanceType distanceType;
      };
   }
}

#endif // __CatesOptimizer_H__