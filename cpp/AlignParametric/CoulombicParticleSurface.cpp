#include "AlignParametric/CoulombicParticleSurface.h"

#include "AlignParametric/CoulombicRegularizer.h"

#include "Mesh/TuplesImpl.h"
#include "Mesh/MeshFunctions.h"
#include "Mesh/TuplesFunctions.h"
#include "MeshSearching/FacesNearestPointSearch.h"
#include "Useful/Noise.h"

#include <vector>
#include <algorithm>
#include "vnl/vnl_matrix_fixed.h"
#include "Useful/ArrayVectorFunctions.h"

using namespace AWT;
using namespace AWT::AlignParametric;

AWT::AlignParametric::CoulombicParticleSurface::CoulombicParticleSurface( MeshType::P mesh, const TuplesType::P samples, const Idx ntake )
: ParticleSurface( mesh, samples, ntake )
{
   // Construct the Cates regularizer
   regularizer = CoulombicRegularizer::getInstance( this );
   regularizer->setCutoffDistance( 0.005 );
   regularizer->setThreshold( 0.005 );
}

AWT::AlignParametric::CoulombicParticleSurface::~CoulombicParticleSurface( )
{
}

AWT::AlignParametric::CoulombicParticleSurface::P AWT::AlignParametric::CoulombicParticleSurface::getInstance( MeshType::P mesh, const TuplesType::P samples, const Idx ntake )
{
   AUTOGETINSTANCE( AWT::AlignParametric::CoulombicParticleSurface, ( mesh, samples, ntake ) );
}

GETNAMEMACRO( AWT::AlignParametric::CoulombicParticleSurface );

// Calculate a regularization term
T AWT::AlignParametric::CoulombicParticleSurface::regularizationCost( )
{
   const T ret = regularizer->calculateCost( );
   return ret;
}

// Calculate a regularization term
void AWT::AlignParametric::CoulombicParticleSurface::regularizationGradient( MatrixType& reg, const Transformation& trans )
{
   const Idx N = samples->getNumberOfPoints();

   // I need to implement the entropy surface sampling here!
   if ( reg.rows() != N || reg.cols() != 3 )
      reg.set_size( N, 3 );
   reg.fill( 0 );
   
   regularizer->calculateUpdate( reg );
}

T AWT::AlignParametric::CoulombicParticleSurface::getMinDistance(const AWT::AlignParametric::Idx p)
{
   return regularizer->getMinDistance( p );
}

CoulombicRegularizer::P AWT::AlignParametric::CoulombicParticleSurface::getRegularizer( )
{
   return regularizer;
}

int AWT::AlignParametric::CoulombicParticleSurface::updatePointLocation( const Idx i, const T* vtx, FacesNearestPointSearch<T>::P searcher )
{
   const int np = ParticleSurface::updatePointLocation(i,vtx, searcher);
   regularizer->associateSampleToFace( i, np );
   return np;
}
