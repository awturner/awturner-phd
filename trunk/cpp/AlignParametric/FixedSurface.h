#ifndef __FIXEDSURFACE_H__
#define __FIXEDSURFACE_H__

#include "TypeDefs.h"

#include "SampledSurface.h"
#include "Mesh/Mesh.h"

#include <iostream>

namespace AWT
{
   namespace AlignParametric
   {
      class FixedSurface : public SampledSurface
      {
      public:
         typedef ManagedAutoPointer<FixedSurface> P;

      protected:
         FixedSurface( MeshType::P mesh );
         virtual ~FixedSurface( );

      public:
         static P getInstance( MeshType::P mesh );
         virtual std::string getClassName( ) const;

         // Get the number of samples on the surface
         virtual Idx getNumberOfSamples( ) const;

         // Get the current sample locations on the 3D mesh
         // NOTE: This should be in HOMOGENEOUS form, i.e. samples.rows() = Dims + 1
         // The last row should be all ones, or weird things will happen...
         virtual void getSamples( MatrixType& samples ) const;

         // How far can samples be moved in order to avoid truncation error?
         virtual T getMaxMove( ) const;

         // Get the number of parameter components describing each sample
         virtual Idx getParameterDimensionality( ) const;

         // Get the number of parameters which control this sampling
         virtual Idx getNumberOfParameters( ) const;

         // Get the current set of control values
         virtual void getParameters( MatrixType& controls ) const;

         // Update the current set of control values
         virtual void setParameters( MatrixType& controls );

         // Calculate the regularization cost
         virtual T regularizationCost( );

         // Calculate a regularization term
         virtual void regularizationGradient( MatrixType& reg, const Transformation& trans );

         // Calculates the jacobian matrix of the l-th sample location w.r.t. the p-th parameters
         // Note that the jacobian must be in HOMOGENEOUS form, i.e. matrix.rows() = Dims + 1
         // The last row of the jacobian must be zero, or weird things will happen...
         virtual void jacobian( const Idx l, const Idx p, MatrixType& matrix ) const;

         virtual void refine( );

         // Iterator functions - allows you to skip all the zero jacobians
         // Takes the internal iterator back to the start
         virtual void resetIterator( );

         // Advances to the next non-zero jacobian pair
         virtual bool next( Idx& l, Idx& p );

      protected:
         struct D;
         D* m_D;

      };
   }
}

#endif // __FIXEDSURFACE_H__