#ifndef __PARTICLEOPTIMIZER_H__
#define __PARTICLEOPTIMIZER_H__

#include "TypeDefs.h"
#include "ModelEigenvalueOptimizer.h"
#include "Useful/ValueRange.h"
#include "CatesParticleSurface.h"

namespace AWT
{
   namespace AlignParametric
   {
      class ParticleOptimizer : public ModelEigenvalueOptimizer
      {
      public:
         typedef ManagedAutoPointer<ParticleOptimizer> P;

         virtual MeshIndex getNumberOfParticlesPerSurface( ) const;

         virtual ParticleSurface::P addMesh( MeshType::P mesh, TuplesType::P particles ) = 0;

      protected:
         ParticleOptimizer( MeshIndex N );

         void checkNumberOfPoints( TuplesType::P particles );

         virtual void calculateSampleWeights( const MatrixType& meanShape, VectorType& sampleAreas );
         virtual FaceType::P getFaces( );

         MeshIndex N;
         Tuples<MeshIndex>::P faces;
      };
   }
}


#endif // __PARTICLEOPTIMIZER_H__