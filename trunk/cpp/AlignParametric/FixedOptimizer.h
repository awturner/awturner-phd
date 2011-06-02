#ifndef __FIXEDOPTIMIZER_H__
#define __FIXEDOPTIMIZER_H__

#include "ModelEigenvalueOptimizer.h"
#include "FixedSurface.h"

namespace AWT
{
   namespace AlignParametric
   {
      class FixedOptimizer : public ModelEigenvalueOptimizer
      {
      public:
         typedef ManagedAutoPointer<FixedOptimizer> P;

      protected:
         FixedOptimizer( );
         virtual ~FixedOptimizer( );

      public:
         static P getInstance( );
         virtual std::string getClassName( ) const;

         virtual FixedSurface::P addMesh( MeshType::P mesh );

      protected:
         virtual void calculateSampleWeights( const MatrixType& meanShape, VectorType& sampleAreas );
         virtual FaceType::P getFaces( );

         FaceType::P faces;
      };
   }
}

#endif // __FIXEDOPTIMIZER_H__