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
#include "ParticleMethods.h"

#include "CommonMethods.h"

#include "AlignParametric/ParticleSurfaceRefinement.h"
#include "AlignParametric/CatesOptimizer.h"
#include "AlignParametric/CoulombicOptimizer.h"
#include "Mesh/VTKMeshLoader.h"
#include "Mesh/TuplesImpl.h"

using namespace AWT;
using namespace AWT::AlignParametric;
using namespace XmlRpc;

void remoteMethod( AWT::AlignParametric::Particles::initialize )
{
   // Clear any existing state
   CommonMethods::reset( );

   if ( params.size() != 4 )
      throw XmlRpcException( "Need exactly 4 arguments" );

   const std::string type             = params[0];
   const int         nparticles       = params[1];
   XmlRpcValue       sigmaRangev      = params[2];
   const std::string distancetype     = params[3];

   ValueRange<T> sigmaRange = { sigmaRangev[0], sigmaRangev[1] };

   DEBUGMACRO( sigmaRange.lower << "\t" << sigmaRange.upper );

   DEBUGMACRO( "Creating an optimizer" );
   if ( type == "cates" )
   {
      DEBUGMACRO( "Creating CatesOptimizer" );
      CatesOptimizer::P catesOpt = CatesOptimizer::getInstance( nparticles, sigmaRange );
      
      if (strcmp(distancetype.c_str(),"approxgeodesic") == 0)
      {
         DEBUGMACRO( "Using approximate geodesic distance measure" );
         catesOpt->setDistanceType( CatesRegularizer::DT_APPROXGEODESIC );
      }
      else
      {
         DEBUGMACRO( "Using euclidean distance measure" );
         catesOpt->setDistanceType( CatesRegularizer::DT_EUCLIDEAN );
      }

      eigOpt = catesOpt;
   }
   else
   {
      throw XmlRpcException( "Invalid type!" );
   }
}

void remoteMethod( AWT::AlignParametric::Particles::addMesh )
{
   const std::string filename = params[0];

   DEBUGMACRO( "Adding surface from " << filename );

   MeshType::P modelMesh = VTKMeshLoader<T>::load( filename.c_str(), false );
   ParticleOptimizer* sto = dynamic_cast<ParticleOptimizer*>( *eigOpt );

   TuplesType::P samples;

   if ( params.size() == 1 )
   {
      DEBUGMACRO( "Using mesh's vertices as samples" );

      const Idx N = sto->getNumberOfParticlesPerSurface();
      double vtx[3];
      samples = TuplesImpl<double>::getInstance( 3, N );
      for ( Idx i = 0; i < N; ++i )
      {
         modelMesh->getVertex(i,vtx);
         samples->setPoint(i,vtx);
      }
   }
   else
   {
      DEBUGMACRO( "Loading samples from file" );
      const std::string sampleFilename = params[1];
      MeshType::P sampleMesh = VTKMeshLoader<T>::load( sampleFilename.c_str(), false );
      samples = sampleMesh->getVertices();
   }

   DEBUGMACRO( "Adding to optimizer" );
   ParticleSurface::P surf = sto->addMesh( modelMesh, samples );
   surf->initialize( );
   sampledSurfaces.push_back( surf );

   RefinementStrategy::P refStrat = sto->getRefinementStrategy();
   if (*refStrat != 0)
   {
      ParticleSurfaceRefinement* psr = dynamic_cast<ParticleSurfaceRefinement*>(*refStrat);
      psr->addParticleSurface(*surf);
   }

   DEBUGMACRO( "Done." );
}

void remoteMethod( AWT::AlignParametric::Particles::configureRefinement )
{
   ParticleOptimizer* sto = dynamic_cast<ParticleOptimizer*>( *eigOpt );

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

void AWT::AlignParametric::Particles::addMethods( AWT::RemoteMethods& methods )
{
   methods.push_back( RemoteMethod( "particles.initialize", initialize ) );
   methods.push_back( RemoteMethod( "particles.addMesh", addMesh ) );
   methods.push_back( RemoteMethod( "particles.configureRefinement", configureRefinement ) );
}