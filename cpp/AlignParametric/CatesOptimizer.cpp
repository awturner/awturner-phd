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
#include "CatesOptimizer.h"

using namespace AWT;
using namespace AWT::AlignParametric;

AWT::AlignParametric::CatesOptimizer::CatesOptimizer( MeshIndex N, ValueRange<T> _sigmaRange )
: ParticleOptimizer( N ), sigmaRange( _sigmaRange ), distanceType( CatesRegularizer::DT_EUCLIDEAN )
{
   DEBUGMACRO( sigmaRange.lower << "\t" << sigmaRange.upper );
}

AWT::AlignParametric::CatesOptimizer::~CatesOptimizer( )
{
}

AWT::AlignParametric::CatesOptimizer::P AWT::AlignParametric::CatesOptimizer::getInstance( MeshIndex N, ValueRange<T> _sigmaRange )
{
   AUTOGETINSTANCE( AWT::AlignParametric::CatesOptimizer, ( N, _sigmaRange ) );
}

GETNAMEMACRO( AWT::AlignParametric::CatesOptimizer );

void AWT::AlignParametric::CatesOptimizer::setSigmaRange( ValueRange<T> range )
{
   if ( range.lower != sigmaRange.lower || range.upper != sigmaRange.upper )
   {
      sigmaRange = range;
      modified();
   }
}

ValueRange<T> AWT::AlignParametric::CatesOptimizer::getSigmaRange( ) const
{
   return sigmaRange;
}

ParticleSurface::P AWT::AlignParametric::CatesOptimizer::addMesh( MeshType::P mesh, TuplesType::P particles )
{
   checkNumberOfPoints( particles );
   
   CatesParticleSurface::P surf = CatesParticleSurface::getInstance( mesh, particles, getNumberOfParticlesPerSurface(), sigmaRange );
   this->_add( surf );

   surf->getRegularizer()->setDistanceType(distanceType);

   return surf;
}

void AWT::AlignParametric::CatesOptimizer::setDistanceType(const CatesRegularizer::DistanceType dt)
{
   if (dt != distanceType)
   {
      distanceType = dt;
      modified();
   }
}

AWT::AlignParametric::CatesRegularizer::DistanceType AWT::AlignParametric::CatesOptimizer::getDistanceType() const
{
   return distanceType;
}
