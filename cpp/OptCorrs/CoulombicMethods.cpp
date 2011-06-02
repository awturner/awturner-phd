#include "CoulombicMethods.h"

#include "CommonMethods.h"
#include "AlignParametric/ParticleSurfaceRefinement.h"
#include "AlignParametric/CoulombicOptimizer.h"
#include "AlignParametric/DeterminantOfCovariance.h"
#include "AlignParametric/MinimumDescriptionLength.h"
#include "Mesh/VTKMeshLoader.h"

using namespace AWT;
using namespace AWT::AlignParametric;
using namespace XmlRpc;

void remoteMethod( AWT::AlignParametric::CoulombicParticles::initialize )
{
   // Clear any existing state
   CommonMethods::reset( );

   if ( params.size() != 2 )
      throw XmlRpcException( "Need exactly 2 arguments" );

   const int         nparticles       = params[0];
   XmlRpcValue       sigmaRangev      = params[1];

   DEBUGMACRO( "Creating an optimizer" );
   CoulombicOptimizer::P sto = CoulombicOptimizer::getInstance( nparticles );

   // Store in the general pointer
   eigOpt = sto;
}

void remoteMethod( AWT::AlignParametric::CoulombicParticles::addMesh )
{
   const std::string filename = params[0];

   DEBUGMACRO( "Adding surface from " << filename );

   MeshType::P modelMesh = VTKMeshLoader<T>::load( filename.c_str(), false );

   TuplesType::P samples;

   if ( params.size() == 1 )
   {
      DEBUGMACRO( "Using mesh's vertices as samples" );
      samples = modelMesh->getVertices();
   }
   else
   {
      DEBUGMACRO( "Loading samples from file" );
      const std::string sampleFilename = params[1];
      MeshType::P sampleMesh = VTKMeshLoader<T>::load( sampleFilename.c_str(), false );
      samples = sampleMesh->getVertices();
   }

   CoulombicOptimizer* sto = dynamic_cast<CoulombicOptimizer*>( *eigOpt );

   DEBUGMACRO( "Adding to optimizer" );
   ParticleSurface::P surf = sto->addMesh( modelMesh, samples );
   sampledSurfaces.push_back( surf );

   RefinementStrategy::P refStrat = sto->getRefinementStrategy();
   if (*refStrat != 0)
   {
      ParticleSurfaceRefinement* psr = dynamic_cast<ParticleSurfaceRefinement*>(*refStrat);
      psr->addParticleSurface(*surf);
   }

   DEBUGMACRO( "Done." );
}


void remoteMethod( AWT::AlignParametric::CoulombicParticles::refine )
{
   for ( Idx i = 0; i < sampledSurfaces.size(); ++i )
      sampledSurfaces[i]->refine();
}

void remoteMethod( AWT::AlignParametric::CoulombicParticles::configureRefinement )
{
   CoulombicOptimizer* sto = dynamic_cast<CoulombicOptimizer*>( *eigOpt );

   RefinementStrategy::P refStrat = sto->getRefinementStrategy();
   if (*refStrat != 0)
   {
      ParticleSurfaceRefinement* psr = dynamic_cast<ParticleSurfaceRefinement*>(*refStrat);

      psr->setMaxParticles( (int)params[0] );
      psr->setSpreadBlockTime( (int)params[1] );
      psr->setParticleMaturityAge( (int)params[2] );
      psr->setMinSplitSeparation( (double)params[3] );
   }
}

void AWT::AlignParametric::CoulombicParticles::addMethods( AWT::RemoteMethods& methods )
{
   methods.push_back( RemoteMethod( "coulombic.initialize", initialize ) );
   methods.push_back( RemoteMethod( "coulombic.addMesh", addMesh ) );
   methods.push_back( RemoteMethod( "coulombic.refine", refine ) );
   methods.push_back( RemoteMethod( "coulombic.configureRefinement", refine ) );
}