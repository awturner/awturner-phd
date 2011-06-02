#include "ParticleOptimizer.h"

#define WITH_MATLAB 0

#if WITH_MATLAB
//#include "Useful/MatlabEngine.h"
#endif

#include "Mesh/MeshImpl.h"
#include "Mesh/MeshFunctions.h"
#include "Mesh/VNLTuples.h"

#include "ParticleSurfaceRefinement.h"

using namespace AWT;
using namespace AWT::AlignParametric;

AWT::AlignParametric::ParticleOptimizer::ParticleOptimizer( MeshIndex _N )
: N(_N)
{
   // Put in a default refinement strategy
   ParticleSurfaceRefinement::P psr = ParticleSurfaceRefinement::getInstance( );

   setRefinementStrategy( psr );

   faces = TuplesImpl<MeshIndex>::getInstance(3, 0);
}

MeshIndex AWT::AlignParametric::ParticleOptimizer::getNumberOfParticlesPerSurface( ) const
{
   return N;
}

void AWT::AlignParametric::ParticleOptimizer::calculateSampleWeights( const MatrixType& meanShape, VectorType& weights )
{
   const Idx N = getNumberOfSamplesPerSurface();
   weights.set_size( N );
   weights.fill( 1.0 / N );
}

void AWT::AlignParametric::ParticleOptimizer::checkNumberOfPoints( TuplesType::P particles )
{
   // Make sure that there are the correct number of particles
   if ( particles->getNumberOfPoints() != getNumberOfParticlesPerSurface() )
   {
      PRINTVBL( getNumberOfParticlesPerSurface() );
      PRINTVBL( particles->getNumberOfPoints() );
      throw "Wrong number of particles specified";
   }
}

FaceType::P AWT::AlignParametric::ParticleOptimizer::getFaces( )
{
   return faces;
}
