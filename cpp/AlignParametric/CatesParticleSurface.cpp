#include "AlignParametric/CatesParticleSurface.h"

#include "AlignParametric/CatesRegularizer.h"

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

AWT::AlignParametric::CatesParticleSurface::CatesParticleSurface( MeshType::P mesh, const TuplesType::P samples, const Idx ntake, const ValueRange<T> _sigmaRange )
: ParticleSurface( mesh, samples, ntake )
{
   // Construct the Cates regularizer
   regularizer = CatesRegularizer::getInstance( this, _sigmaRange );
}

AWT::AlignParametric::CatesParticleSurface::~CatesParticleSurface( )
{
}

AWT::AlignParametric::CatesParticleSurface::P AWT::AlignParametric::CatesParticleSurface::getInstance( MeshType::P mesh, const TuplesType::P samples, const Idx ntake, const ValueRange<T> _sigmaRange )
{
   AUTOGETINSTANCE( AWT::AlignParametric::CatesParticleSurface, ( mesh, samples, ntake, _sigmaRange ) );
}

GETNAMEMACRO( AWT::AlignParametric::CatesParticleSurface );

// Calculate a regularization term
T AWT::AlignParametric::CatesParticleSurface::regularizationCost( )
{
   const T ret = regularizer->calculateCost( );
   PRINTVBL2( "reg cost", ret );

   return ret;
}

// Calculate a regularization term
void AWT::AlignParametric::CatesParticleSurface::regularizationGradient( MatrixType& reg, const Transformation& trans )
{
   const Idx N = samples->getNumberOfPoints();

   if ( reg.rows() != N || reg.cols() != 3 )
      reg.set_size( N, 3 );
   reg.fill( 0 );
   
   regularizer->calculateUpdate( reg );

   const double minSig = regularizer->getMinSigma();

   PRINTVBL( minSig*minSig );

   return;

   // I want to do this numerically also...

   // Take a copy of the samples so that I can return here later
   TuplesType::P samplesCopy = TuplesImpl<T>::getInstance(samples);

   const T twiddles[] = { 1e-6 };

   for ( Idx i = 0; i < samples->getNumberOfPoints(); ++i )
   {
      for ( Idx ax = 0; ax < 3; ++ax )
      {
         for ( Idx t = 0; t < 1; ++t )
         {
            const T twiddle = twiddles[t]; //1e-6;
            // Wiggle each part around
            samples->setPointElement(i,ax,samplesCopy->getPointElement(i,ax) + twiddle);
            const T Ep = regularizer->calculateNegativeEntropy();

            samples->setPointElement(i,ax,samplesCopy->getPointElement(i,ax) - twiddle);
            const T Em = regularizer->calculateNegativeEntropy();

            const T gradient = (Ep-Em) / (2*twiddle);

            DEBUGMACRO( "Numerical:" << gradient << "; " << "Calculated:" << reg(i,ax) );

            // Put it back where you found it
            samples->setPointElement(i,ax,samplesCopy->getPointElement(i,ax));
         }
      }
   }

   // Put the samples back
   T vtx[3];
   for ( Idx i = 0; i < samples->getNumberOfPoints(); ++i )
   {
      samplesCopy->getPoint(i,vtx);
      samples->setPoint(i,vtx);
   }
}

T AWT::AlignParametric::CatesParticleSurface::getMinDistance(const AWT::AlignParametric::Idx p)
{
   return regularizer->getSigma( p );
}

CatesRegularizer::P AWT::AlignParametric::CatesParticleSurface::getRegularizer( )
{
   return regularizer;
}

int AWT::AlignParametric::CatesParticleSurface::updatePointLocation( const Idx i, const T* vtx, FacesNearestPointSearch<T>::P searcher )
{
   const int np = ParticleSurface::updatePointLocation(i,vtx, searcher);
   regularizer->associateSampleToFace( i, np );
   return np;
}

void AWT::AlignParametric::CatesParticleSurface::fixSigma(const bool state)
{
   regularizer->fixSigma(state);
}