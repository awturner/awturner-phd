#ifndef __SAMPLEDTORUSOPTIMIZER_H__
#define __SAMPLEDTORUSOPTIMIZER_H__

#include "ModelEigenvalueOptimizer.h"
#include "SampledTorusMesh.h"

namespace AWT
{
   namespace AlignParametric
   {
      class SampledTorusOptimizer : public ModelEigenvalueOptimizer
      {
      public:
         typedef ManagedAutoPointer<SampledTorusOptimizer> P;

      protected:
         SampledTorusOptimizer( );
         virtual ~SampledTorusOptimizer( );

      public:
         static P getInstance( );
         virtual std::string getClassName( ) const;

         virtual void setSamplingMesh( MeshType::P mesh );
         MeshType::P getSamplingMesh( );

         virtual void setNumberOfKernels( const MeshIndex nk );
         virtual void setNumberOfKernels( const MeshIndex nk[2] );
         virtual void getNumberOfKernels( MeshIndex nk[2] );

         virtual SampledTorusMesh::P addMesh( MeshType::P mesh );

      protected:
         virtual void calculateSampleWeights( const MatrixType& meanShape, VectorType& sampleAreas );
         virtual FaceType::P getFaces( );

         MeshType::P samplingMesh;
         MeshIndex nk[2];
      };
   }
}

#endif // __SAMPLEDTORUSOPTIMIZER_H__