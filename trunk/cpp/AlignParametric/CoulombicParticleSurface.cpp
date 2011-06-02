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
