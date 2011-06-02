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
#ifndef __ALIGNER_H__
#define __ALIGNER_H__

#include "ObjectManagement/ManagedObject.h"

#include "TypeDefs.h"
#include "ShapeModel.h"
#include "Mesh.h"

#include "AlignObject.h"
#include "FragmentObject.h"
#include "ModelObject.h"
#include "AllSearchFilters.h"

namespace AWT
{
   namespace SimpleMesh
   {
      class Aligner : public ManagedObject
      {
      public:
         typedef ManagedAutoPointer<Aligner> P;
         enum CostFunctionType { CF_POINTTOPOINT, CF_POINTTOPLANE };

      protected:
         Aligner(CostFunctionType cft);
         virtual ~Aligner();

      public:
         static P getInstance(CostFunctionType cft);
         virtual std::string getClassName() const;

         CostFunctionType getCostFunctionType() const;
         void setCostFunctionType(const CostFunctionType& v);

         void setModel(ShapeModel::P model, SamplerFactory::P fact, const Index nsamples, Vector params);
         void addFragment(Mesh::P frag, Sampler::P samp, const Index nsamples, Vector params);

         void setFragmentNoise(const double v);
         double getFragmentNoise() const;

         const Vector getModelParameters() const;
         void setModelParameters(const Vector& vec);

         Index getNumberOfFragments() const;

         const Vector getFragmentPose(const Index i) const;
         void setFragmentPose(const Index i, const Vector& vec);

         const Transformation getFragmentTransformation(const Index i) const;

         const Vector getTargetRegistrationError(const Index i, const Index j) const;

         LameckerMeasures getLameckerMeasures();

         void resample();

         void writeModels(const std::string& saveFormat);

         Index findCorrespondences();

         double stepUpdate();

         void setNumberOfModes(const Index i);
         void activateAllModes();

         Index getNumberOfModes() const;

         double getTrimFactor() const;
         void setTrimFactor(const double v);

      protected:
         struct D;
         D* m_D;

      };
   }
}

#endif // __ALIGNER_H__