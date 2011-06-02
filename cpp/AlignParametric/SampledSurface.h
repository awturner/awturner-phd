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
#ifndef __SAMPLEDSURFACE_H__
#define __SAMPLEDSURFACE_H__

#include "ObjectManagement/ManagedObject.h"

#include "TypeDefs.h"

#include <iostream>

namespace AWT
{
   namespace AlignParametric
   {
      class SampledSurface : public ManagedObject
      {
      public:
         typedef ManagedAutoPointer<SampledSurface> P;

         // Get the number of parameter components describing each sample
         virtual Idx getParameterDimensionality() const = 0;

         // Get the number of samples on the surface
         virtual Idx getNumberOfSamples() const = 0;

         // Get the current sample locations
         // NOTE: This should be in HOMOGENEOUS form, i.e. samples.rows() = Dims + 1
         // The last row should be all ones, or weird things will happen...
         virtual void getSamples(MatrixType& samples) const = 0;

         // How far can samples be moved in order to avoid truncation error?
         virtual T getMaxMove() const = 0;

         // Get the number of parameters which control this sampling
         virtual Idx getNumberOfParameters() const = 0;

         // Get the current set of control values
         virtual void getParameters(MatrixType& controls) const = 0;

         // Update the current set of control values
         virtual void setParameters(MatrixType& controls) = 0;

         virtual T regularizationCost() = 0;

         // Calculate a regularization term
         virtual void regularizationGradient(MatrixType& reg, const Transformation& trans) = 0;

         // Calculates the jacobian matrix of the l-th sample location w.r.t. the p-th parameters
         // Note that the jacobian must be in HOMOGENEOUS form, i.e. matrix.rows() = Dims + 1
         // The last row of the jacobian must be zero, or weird things will happen...
         virtual void jacobian(const Idx l, const Idx p, MatrixType& matrix) const = 0;

         // When things have got a bit too comfortable, refine the parametrization
         virtual void refine() = 0;

         // Iterator functions - allows you to skip all the zero jacobians
         // Takes the internal iterator back to the start
         virtual void resetIterator() = 0;

         // Advances to the next non-zero jacobian pair
         virtual bool next(Idx& l, Idx& p) = 0;
      };
   }
}

#endif // __SAMPLEDSURFACE_H__