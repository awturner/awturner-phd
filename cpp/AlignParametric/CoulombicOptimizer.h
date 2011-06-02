#ifndef __CoulombicOptimizer_H__
#define __CoulombicOptimizer_H__

#include "ModelEigenvalueOptimizer.h"
#include "Useful/ValueRange.h"
#include "CoulombicParticleSurface.h"

#include "ParticleOptimizer.h"

namespace AWT
{
   namespace AlignParametric
   {
      class CoulombicOptimizer : public ParticleOptimizer
      {
      public:
         typedef ManagedAutoPointer<CoulombicOptimizer> P;

      protected:
         CoulombicOptimizer( MeshIndex N );
         virtual ~CoulombicOptimizer( );

      public:
         static P getInstance( MeshIndex N );
         virtual std::string getClassName( ) const;

         virtual ParticleSurface::P addMesh( MeshType::P mesh, TuplesType::P particles );
      };
   }
}

#endif // __CoulombicOptimizer_H__