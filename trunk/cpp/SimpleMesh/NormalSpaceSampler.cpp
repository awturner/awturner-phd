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
#include "NormalSpaceSampler.h"

#include "Useful/ColouredConsole.h"
#include "Useful/Noise.h"

#include "MeshFunctions.h"

using namespace AWT::SimpleMesh;

AWT::SimpleMesh::NormalSpaceSampler::NormalSpaceSampler( Mesh::P mesh, const unsigned int _latdivs )
: AWT::SimpleMesh::Sampler( mesh ), latdivs( _latdivs )
{
   calculateCumAreas( );
}

AWT::SimpleMesh::NormalSpaceSampler::~NormalSpaceSampler( )
{
}

AWT::SimpleMesh::NormalSpaceSampler::P AWT::SimpleMesh::NormalSpaceSampler::getInstance( Mesh::P mesh, const unsigned int _latdivs )
{
   AUTOGETINSTANCE( AWT::SimpleMesh::NormalSpaceSampler, ( mesh, _latdivs ) );
}

GETNAMEMACRO( AWT::SimpleMesh::NormalSpaceSampler );

void AWT::SimpleMesh::NormalSpaceSampler::calculateCumAreas()
{
   unsigned int* faceBin    = new unsigned int[ mesh->nf ];
   double* surfaceAreaInBin = new double[ latdivs ];

   for ( Index s = 0; s < latdivs; ++s )
      surfaceAreaInBin[s] = 0;

   const double PI = 4*atan(1.0);

   for ( Index f = 0; f < mesh->nf; ++f )
   {
      Point nml = calculateFaceNormal( mesh, f );

      // Calculate the "latitude" of this normal
      double lat = asin( nml[2] );

      // Calculate which bin this normal falls into
      faceBin[f] = static_cast<unsigned int>( floor( ( lat + PI/2 ) / PI * latdivs ) );
      faceBin[f] = std::min<Index>( latdivs-1, faceBin[f] );

      surfaceAreaInBin[ faceBin[f] ] += ( cumAreas[f] = calculateFaceArea( mesh, f ) );
   }

   // Also need to weight the surfaceAreaInBin for latitude area variation
   // (so that polar regions are fairly sampled)
   const double delta = PI / latdivs;
   for ( Index i = 0; i < latdivs; ++i )
      surfaceAreaInBin[i] *= 0.5 * ( sin( (i+1)*delta - PI/2 ) - sin( i*delta - PI/2 ) );

   // Weight the face areas
   for ( Index f = 0; f < mesh->nf; ++f )
      cumAreas[f] /= surfaceAreaInBin[ faceBin[f] ];

   delete [] surfaceAreaInBin;
   delete [] faceBin;

   double totalArea = 0;

   // Calculate and accumulate face areas
   for ( Index f = 0; f < mesh->nf; ++f )
   {
      const double thisFaceArea = cumAreas[f];
      cumAreas[f] = totalArea;
      totalArea += thisFaceArea;
   }

   // Normalize so that the total area is unity
   for ( Index f = 0; f < mesh->nf; ++f )
      cumAreas[f] /= totalArea;
}

AWT::SimpleMesh::NormalSpaceSamplerFactory::NormalSpaceSamplerFactory( const unsigned int _latdivs )
: latdivs( _latdivs )
{}

AWT::SimpleMesh::NormalSpaceSamplerFactory::~NormalSpaceSamplerFactory( ) {}

Sampler::P AWT::SimpleMesh::NormalSpaceSamplerFactory::createInstance( Mesh::P mesh )
{
   return NormalSpaceSampler::getInstance(mesh, latdivs);
}

AWT::SimpleMesh::NormalSpaceSamplerFactory::P AWT::SimpleMesh::NormalSpaceSamplerFactory::getInstance( const unsigned int _latdivs )
{
   AUTOGETINSTANCE( AWT::SimpleMesh::NormalSpaceSamplerFactory, ( _latdivs ) );
}

GETNAMEMACRO( AWT::SimpleMesh::NormalSpaceSamplerFactory );