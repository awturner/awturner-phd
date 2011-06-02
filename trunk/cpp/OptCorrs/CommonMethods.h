/*
* ################################################################################
* ### MIT License
* ################################################################################
* 
* Copyright (c) 2006-2011 Andy Turner
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/
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