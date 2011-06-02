#ifndef __COMMONMETHODS_H__
#define __COMMONMETHODS_H__

#include "Useful/XmlRpcDefines.h"
#include "AlignParametric/ModelEigenvalueOptimizer.h"

#include "AlignParametric/DeterminantOfCovariance.h"
#include "AlignParametric/MinimumDescriptionLength.h"
#include "XmlRpc.h"

namespace AWT
{
   namespace AlignParametric
   {
      extern ModelEigenvalueOptimizer::P eigOpt;
      extern std::vector<SampledSurface::P> sampledSurfaces;

      void addMethodsToServer( XmlRpcServer& s, AWT::RemoteMethods& methods );

      namespace CommonMethods
      {
         void reset( );

         void remoteMethod( getScaleNormalization );
         void remoteMethod( setScaleNormalization );
         void remoteMethod( setCostFunction );
         void remoteMethod( getMeanSamples );
         void remoteMethod( getSamples );
         void remoteMethod( getParameters );
         void remoteMethod( getNumberOfSamplesPerSurface );
         void remoteMethod( getNumberOfSampledSurfaces );
         void remoteMethod( calculateNow );
         void remoteMethod( calculateMeasures );
         void remoteMethod( saveModel );
         void remoteMethod( saveMatlab );
         void remoteMethod( calculateGradients );
         void remoteMethod( applyUpdates );
         void remoteMethod( getEigenvalues );
         void remoteMethod( getSampleConvergenceThreshold );
         void remoteMethod( setSampleConvergenceThreshold );
         void remoteMethod( getModelWeight );
         void remoteMethod( setModelWeight );
         void remoteMethod( getRegularizationWeight );
         void remoteMethod( setRegularizationWeight );
         void remoteMethod( setFirstShapeFixed );
         void remoteMethod( isFirstShapeFixed );
         void remoteMethod( setPermittedAlignments );
         void remoteMethod( getPermittedAlignments );
         void remoteMethod( refine );
         void remoteMethod( pause );

#if WITH_MATLAB
         void remoteMethod( showInMatlab );
#endif

         void addMethods( AWT::RemoteMethods& methods );
	  }
   }
}

#endif // __COMMONMETHODS_H__