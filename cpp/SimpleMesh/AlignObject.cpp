#include "AlignObject.h"

using namespace AWT::SimpleMesh;

AWT::SimpleMesh::AlignObject::AlignObject( const Index samples )
: nsamples( samples )
{
}

Vector AWT::SimpleMesh::AlignObject::getParameters( ) const
{
   return params;
}

void AWT::SimpleMesh::AlignObject::incrementParameters( const Vector& inc )
{
   setParameters( params + inc );
}

void AWT::SimpleMesh::AlignObject::setParameters( const Vector& params )
{
   this->params.update( params );
}

Mesh::P AWT::SimpleMesh::AlignObject::getMesh( )
{
   return mesh;
}

void AWT::SimpleMesh::AlignObject::resample( )
{
   samples.clear( );

   for ( Index i = 0; i < nsamples; ++i )
      samples.push_back( sampler->sample( ) );
}

PIWs& AWT::SimpleMesh::AlignObject::getSamples( )
{
   return samples;
}