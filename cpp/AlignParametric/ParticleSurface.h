#ifndef __PARTICLESURFACE_H__
#define __PARTICLESURFACE_H__

#include "CatesRegularizer.h"
#include "AlignParametric/SampledSurface.h"
#include "AlignParametric/TypeDefs.h"
#include "Mesh/Mesh.h"
#include "MeshSearching/FacesNearestPointSearch.h"

#include <iostream>

namespace AWT
{
   namespace AlignParametric
   {
      class ParticleSurface : public SampledSurface
      {
      public:
         typedef ManagedAutoPointer<ParticleSurface> P;

      protected:
         ParticleSurface( MeshType::P mesh, const TuplesType::P samples, const Idx ntake );
         virtual ~ParticleSurface( );

      public:
         // Get the number of parameter components describing each sample
         virtual Idx getParameterDimensionality( ) const;

         virtual void initialize( );

         // Get the number of samples on the surface
         virtual Idx getNumberOfSamples( ) const;

         // Get the current sample locations
         // NOTE: This should be in HOMOGENEOUS form, i.e. samples.rows() = Dims + 1
         // The last row should be all ones, or weird things will happen...
         virtual void getSamples( MatrixType& samples ) const;

         // How far can samples be moved in order to avoid truncation error?
         virtual T getMaxMove( ) const;

         // Get the number of parameters which control this sampling
         virtual Idx getNumberOfParameters( ) const;

         // Get the current set of control values
         virtual void getParameters( MatrixType& controls ) const;

         // Update the current set of control values
         virtual void setParameters( MatrixType& controls );

         // Calculate the regularization cost
         virtual T regularizationCost( ) = 0;

         // Calculate the regularization update
         virtual void regularizationGradient( MatrixType& reg, const Transformation& trans ) = 0;

         // Distance to the closest neighbour particle
         virtual T getMinDistance( const Idx p ) = 0;

         // Calculates the jacobian matrix of the l-th sample location w.r.t. the p-th parameters
         // Note that the jacobian must be in HOMOGENEOUS form, i.e. matrix.rows() = Dims + 1
         // The last row of the jacobian must be zero, or weird things will happen...
         virtual void jacobian( const Idx l, const Idx p, MatrixType& matrix ) const;

         virtual void splitParticle( const Idx p, const T* theShift );

         virtual void refine( );

         // Iterator functions - allows you to skip all the zero jacobians
         // Takes the internal iterator back to the start
         virtual void resetIterator( );

         // Advances to the next non-zero jacobian pair
         virtual bool next( Idx& l, Idx& p );

         MeshType::P getMesh( );
         TuplesType::P getSamples( );

      protected:
         TuplesType::P samples;

         Idx iterPtr;

         std::vector< Idx > particleFace;

         T maxMove;

         MeshType::P mesh;
         TuplesType::P faceNormals;

         virtual int updatePointLocation( const Idx i, const T* vtx, FacesNearestPointSearch<T>::P searcher );
         
         void calculateFaceNormals( );
         void setPointLocationsInitial( );
      };
   }
}

#endif // __PARTICLESURFACE_H__