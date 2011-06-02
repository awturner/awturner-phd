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
