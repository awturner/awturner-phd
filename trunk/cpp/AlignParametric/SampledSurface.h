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
         virtual Idx getParameterDimensionality( ) const = 0;

         // Get the number of samples on the surface
         virtual Idx getNumberOfSamples( ) const = 0;

         // Get the current sample locations
         // NOTE: This should be in HOMOGENEOUS form, i.e. samples.rows() = Dims + 1
         // The last row should be all ones, or weird things will happen...
         virtual void getSamples( MatrixType& samples ) const = 0;

         // How far can samples be moved in order to avoid truncation error?
         virtual T getMaxMove( ) const = 0;

         // Get the number of parameters which control this sampling
         virtual Idx getNumberOfParameters( ) const = 0;

         // Get the current set of control values
         virtual void getParameters( MatrixType& controls ) const = 0;

         // Update the current set of control values
         virtual void setParameters( MatrixType& controls ) = 0;

         virtual T regularizationCost( ) = 0;

         // Calculate a regularization term
         virtual void regularizationGradient( MatrixType& reg, const Transformation& trans ) = 0;

         // Calculates the jacobian matrix of the l-th sample location w.r.t. the p-th parameters
         // Note that the jacobian must be in HOMOGENEOUS form, i.e. matrix.rows() = Dims + 1
         // The last row of the jacobian must be zero, or weird things will happen...
         virtual void jacobian( const Idx l, const Idx p, MatrixType& matrix ) const = 0;

         // When things have got a bit too comfortable, refine the parametrization
         virtual void refine( ) = 0;

         // Iterator functions - allows you to skip all the zero jacobians
         // Takes the internal iterator back to the start
         virtual void resetIterator( ) = 0;

         // Advances to the next non-zero jacobian pair
         virtual bool next( Idx& l, Idx& p ) = 0;
      };
   }
}

#endif // __SAMPLEDSURFACE_H__