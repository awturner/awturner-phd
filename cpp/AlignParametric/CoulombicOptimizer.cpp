#include "CoulombicOptimizer.h"

#include "Useful/MatlabEngine.h"

#include "Mesh/MeshImpl.h"
#include "Mesh/MeshFunctions.h"
#include "Mesh/VNLTuples.h"

using namespace AWT;
using namespace AWT::AlignParametric;

AWT::AlignParametric::CoulombicOptimizer::CoulombicOptimizer( MeshIndex N )
: ParticleOptimizer( N )
{
}

AWT::AlignParametric::CoulombicOptimizer::~CoulombicOptimizer( )
{
}

AWT::AlignParametric::CoulombicOptimizer::P AWT::AlignParametric::CoulombicOptimizer::getInstance( MeshIndex N )
{
   AUTOGETINSTANCE( AWT::AlignParametric::CoulombicOptimizer, ( N ) );
}

GETNAMEMACRO( AWT::AlignParametric::CoulombicOptimizer );

ParticleSurface::P AWT::AlignParametric::CoulombicOptimizer::addMesh( MeshType::P mesh, TuplesType::P particles )
{
   checkNumberOfPoints( particles );
   
   CoulombicParticleSurface::P surf = CoulombicParticleSurface::getInstance( mesh, particles, getNumberOfParticlesPerSurface() );
   this->_add( surf );

   return surf;
}
