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