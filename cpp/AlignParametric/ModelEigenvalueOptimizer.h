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
#ifndef __SAMPLEOPTIMIZER_H__
#define __SAMPLEOPTIMIZER_H__

#include "ObjectManagement/ManagedObject.h"

#include "EigenvalueCostFunction.h"
#include "SampledSurface.h"
#include "RefinementStrategy.h"
#include "SampledSurfaceFactory.h"

#include <ostream>

namespace AWT
{
   namespace AlignParametric
   {
      class ModelEigenvalueOptimizer : public ManagedObject
      {
      public:
         enum ScaleNormalizationType {
            NRML_NONE,
            NRML_HYPERSPHERE,
            NRML_HYPERPLANE
         };

         typedef ManagedAutoPointer<ModelEigenvalueOptimizer> P;
         typedef SampledSurface SurfaceType;

      protected:
         ModelEigenvalueOptimizer();
         virtual ~ModelEigenvalueOptimizer();

      public:
         //static P getInstance();
         virtual std::string getClassName() const;

         // What is the cost function that we're working with?
         virtual EigenvalueCostFunction::P getCostFunction();
         virtual void setCostFunction(EigenvalueCostFunction::P func);

         virtual ScaleNormalizationType getScaleNormalization() const;
         virtual void setScaleNormalization(const ScaleNormalizationType v);

         virtual RefinementStrategy::P getRefinementStrategy() const;
         virtual void setRefinementStrategy(RefinementStrategy::P v);

         // What are the alignments that we are allowed?
         virtual bool isRotationAllowed() const;
         virtual void setRotationAllowed(const bool v);

         virtual bool isTranslationAllowed() const;
         virtual void setTranslationAllowed(const bool v);

         // How many surfaces are we trying to optimize on?
         virtual Idx getNumberOfSampledSurfaces() const;

         // Hold the zero-th shape fixed?
         virtual bool isFirstShapeFixed() const;
         virtual void setFirstShapeFixed(const bool v);

         // Add in another one
         //virtual SampledSurface::P addSampledSurface(MeshType::P mesh, SampledSurfaceFactory::P fact);

         // Get the current estimate of the mean mesh
         virtual TuplesType::P getMeanSamples() const;

         virtual TuplesType::P getMode(const Idx m) const;

         // Get the i-th of the sampled surfaces
         virtual SurfaceType::P       getSampledSurface(const Idx i);

         virtual TuplesType::P        getSamples(const Idx i) const;

         virtual Transformation getTransformation(const Idx i) const;

         // We have to have the same number of samples on each surface
         virtual Idx getNumberOfSamplesPerSurface() const;

         // Calculate the current value of the cost function
         virtual T calculateNow();

         // Calculate the generalization, specificity, compactness and leave-one-out
         virtual void calculateMeasures();

         // Save the model
         virtual void saveModel(const std::string& filename);
         virtual void saveMatlab(const std::string& filename);

         // Return the eigenvalues used to calculate the cost function
         virtual const VectorType& getEigenvalues() const;

         // Calculate the update to each of the surface samplings
         // Return the maximum recommended time step
         virtual T calculateGradients();

         // Get the calculated update for the i-th surface
         virtual const MatrixType& getSampledSurfaceUpdate(const Idx i) const;

         virtual void applyUpdates(const T deltaT);

         virtual void setSampleConvergenceThreshold(const T v);
         virtual T getSampleConvergenceThreshold() const;

         virtual void setModelWeight(const T v);
         virtual T getModelWeight() const;

         virtual void setRegularizationWeight(const T v);
         virtual T getRegularizationWeight() const;

         static void debugGradients(ModelEigenvalueOptimizer::P so);

      protected:
         void _add(SurfaceType::P surf);

         virtual void        calculateSampleWeights(const MatrixType& meanShape, VectorType& sampleAreas) = 0;
         virtual FaceType::P getFaces() = 0;

         struct D;
         D* m_D;

      };
   }
}

#endif // __SAMPLEOPTIMIZER_H__