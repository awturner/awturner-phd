#include "UniformSampler.h"

#include "Useful/Noise.h"

#include "MeshFunctions.h"

using namespace AWT::SimpleMesh;

AWT::SimpleMesh::UniformSampler::UniformSampler( Mesh::P mesh )
: AWT::SimpleMesh::Sampler( mesh )
{
   calculateCumAreas( );
}

AWT::SimpleMesh::UniformSampler::~UniformSampler( )
{
}

AWT::SimpleMesh::UniformSampler::P AWT::SimpleMesh::UniformSampler::getInstance( Mesh::P mesh )
{
   AUTOGETINSTANCE( AWT::SimpleMesh::UniformSampler, ( mesh ) );
}

GETNAMEMACRO( AWT::SimpleMesh::UniformSampler );

void AWT::SimpleMesh::UniformSampler::calculateCumAreas()
{
   double totalArea = 0;

   // Calculate and accumulate face areas
   for ( Index f = 0; f < mesh->nf; ++f )
      cumAreas[f] = ( totalArea += calculateFaceArea(mesh,f) );

   // Normalize so that the total area is unity
   for ( Index f = 0; f < mesh->nf; ++f )
      cumAreas[f] /= totalArea;
}

AWT::SimpleMesh::UniformSamplerFactory::UniformSamplerFactory( ) {}
AWT::SimpleMesh::UniformSamplerFactory::~UniformSamplerFactory( ) {}

AWT::SimpleMesh::UniformSamplerFactory::P AWT::SimpleMesh::UniformSamplerFactory::getInstance( )
{
   AUTOGETINSTANCE( AWT::SimpleMesh::UniformSamplerFactory, ( ) );
}

Sampler::P AWT::SimpleMesh::UniformSamplerFactory::createInstance( Mesh::P mesh )
{
   return UniformSampler::getInstance(mesh);
}

GETNAMEMACRO( AWT::SimpleMesh::UniformSamplerFactory );