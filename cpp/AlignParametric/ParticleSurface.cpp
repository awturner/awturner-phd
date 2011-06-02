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
#include "AlignParametric/ParticleSurface.h"

#include "Mesh/TuplesImpl.h"
#include "Mesh/MeshFunctions.h"
#include "Mesh/TuplesFunctions.h"
#include "MeshSearching/FacesNearestPointSearch.h"
#include "Useful/Noise.h"
#include "Useful/ColouredConsole.h"

#include <vector>
#include <algorithm>
#include "vnl/vnl_matrix_fixed.h"
#include "Useful/ArrayVectorFunctions.h"

using namespace AWT;
using namespace AWT::AlignParametric;

AWT::AlignParametric::ParticleSurface::ParticleSurface( MeshType::P mesh, const TuplesType::P samples, const Idx _ntake )
{
   // Keep a hold of the mesh
   this->mesh = mesh;

   calculateFaceNormals( );

   const Idx ntake = std::min<Idx>( _ntake, samples->getNumberOfPoints() );
   DEBUGMACRO( "Only taking " << ntake );
   // Take just the first 10 samples
   this->samples = TuplesImpl<T>::getInstance( 3, ntake );
   for ( Idx i = 0; i < ntake; ++i )
   {
      T vtx[3];
      samples->getPoint( i, vtx );
      this->samples->setPoint( i, vtx );
   }
   
   // Calculate the 5th-%ile triangle area, use this to avoid truncation error
   std::vector<T> areas;
   MESH_EACHFACE( mesh, f )
      areas.push_back( MeshFunctions<T>::getFaceArea( mesh, f ) );

   sort( areas.begin(), areas.end() );

   maxMove = sqrt( areas[ areas.size() *  5 / 100 ] ) / 2;
   PRINTVBL( maxMove );
}

void AWT::AlignParametric::ParticleSurface::initialize( )
{  
   DEBUGLINE;
   setPointLocationsInitial( );
}

AWT::AlignParametric::ParticleSurface::~ParticleSurface( )
{
}

// Get the number of parameter components describing each sample
Idx AWT::AlignParametric::ParticleSurface::getParameterDimensionality( ) const
{
   return 3;
}

// Get the number of samples on the surface
Idx AWT::AlignParametric::ParticleSurface::getNumberOfSamples( ) const
{
   return samples->getNumberOfPoints();
}

// Get the current sample locations
// NOTE: This should be in HOMOGENEOUS form, i.e. samples.rows() = Dims + 1
// The last row should be all ones, or weird things will happen...
void AWT::AlignParametric::ParticleSurface::getSamples( MatrixType& samples ) const
{
   if ( samples.rows() != 4 || samples.cols() != getNumberOfSamples() )
      samples.set_size( 4, getNumberOfSamples() );

   T vtx[4];
   for ( Idx i = 0, imax = getNumberOfSamples(); i < imax; ++i )
   {
      this->samples->getPoint( i, vtx );
      vtx[3] = 1;
      
      samples.set_column( i, vtx );
   }
}

T AWT::AlignParametric::ParticleSurface::getMaxMove( ) const
{
   return maxMove;
}

// Get the number of parameters which control this sampling
Idx AWT::AlignParametric::ParticleSurface::getNumberOfParameters( ) const
{
   return samples->getNumberOfPoints();
}

// Get the current set of control values
void AWT::AlignParametric::ParticleSurface::getParameters( MatrixType& controls ) const
{
   if ( controls.rows() != getNumberOfParameters() || controls.cols() != 3 )
      controls.set_size( getNumberOfParameters(), 3 );

   T vtx[3];
   for ( Idx i = 0, imax = getNumberOfParameters(); i < imax; ++i )
   {
      samples->getPoint(i,vtx);
      
      controls.set_row( i, vtx );
   }
}

void AWT::AlignParametric::ParticleSurface::jacobian( const Idx l, const Idx p, MatrixType& matrix ) const
{
   // Nice and easy Jacobian...
   if ( l == p )
   {
      matrix.set_identity( );
      return;

      T nml[3];
      faceNormals->getPoint( particleFace[l], nml );

      for ( Idx r = 0; r < 3; ++r )
         for ( Idx c = 0; c < 3; ++c )
            matrix(r,c) -= nml[r]*nml[c];
   }
   else
   {
      matrix.fill( 0 );
   }
}

void AWT::AlignParametric::ParticleSurface::splitParticle( const Idx p, const T* theShift )
{
   ColouredConsole cons( ColouredConsole::COL_YELLOW );

   FacesNearestPointSearch<T>::P searcher = FacesNearestPointSearch<T>::getInstance( );
   T vtx[4];

   const T mults[] = { -0.5, 0.5 };
   const Idx idxs[] = { p, samples->getNumberOfPoints() };

   // Push back a face into the list (will be overwritten soon enough)
   particleFace.push_back( INVALID_INDEX );

   for ( Idx i = 0; i < 2; ++i )
   {
      // Get the sample location
      samples->getPoint( p, vtx );

      FOREACHAXIS( ax )
         vtx[ax] += mults[i]*theShift[ax];
      vtx[3] = 1;

      updatePointLocation( idxs[i], vtx, searcher );

      samples->getPoint( idxs[i], vtx );
      //PRINTVBL( idxs[i] );
      //PRINTVEC( vtx, 4 );
   }

   modified();
}

void AWT::AlignParametric::ParticleSurface::refine( )
{
}

// Iterator functions - allows you to skip all the zero jacobians
// Takes the internal iterPtr back to the start
void AWT::AlignParametric::ParticleSurface::resetIterator( )
{
   iterPtr = 0;
}

// Advances to the next non-zero jacobian pair
bool AWT::AlignParametric::ParticleSurface::next( Idx& l, Idx& p )
{
   bool ret = iterPtr < samples->getNumberOfPoints();

   if ( ret )
      l = p = iterPtr++;
   else
      l = p = INVALID_INDEX;

   return ret;
}

MeshType::P AWT::AlignParametric::ParticleSurface::getMesh( )
{
   return mesh;
}

TuplesType::P AWT::AlignParametric::ParticleSurface::getSamples( )
{
   return samples;
}

// Projects the point to the closest point on the mesh surface
int AWT::AlignParametric::ParticleSurface::updatePointLocation( const Idx i, const T* vtx, FacesNearestPointSearch<T>::P searcher )
{
   searcher->reset( );
   searcher->setTestPoint( vtx );
   mesh->searchFaces( searcher );

   T vtxNearest[3];
   int np = searcher->getNearestPoint( vtxNearest );
   particleFace[i] = np;

   samples->setPoint( i, vtxNearest );

   return np;
}

void AWT::AlignParametric::ParticleSurface::calculateFaceNormals( )
{
   T vtx[3][3];

   faceNormals = TuplesImpl<T>::getInstance( 3, mesh->getNumberOfFaces() );

   MESH_EACHFACE( mesh, f )
   {
      mesh->getFace( f, vtx[0], vtx[1], vtx[2] );

      FOREACHAXIS( ax )
      {
         vtx[2][ax] -= vtx[0][ax];
         vtx[1][ax] -= vtx[0][ax];
      }
      cross<T>( vtx[1], vtx[2], vtx[0] );
      normalize<T>( vtx[0], 3 );

      faceNormals->setPoint( f, vtx[0] );
   }
}

void AWT::AlignParametric::ParticleSurface::setPointLocationsInitial( )
{
   FacesNearestPointSearch<T>::P searcher = FacesNearestPointSearch<T>::getInstance( );
   T vtx[4];

   // Make sure that the list of particleFaces is allocated
   particleFace.clear();

   for ( Idx i = 0, imax = samples->getNumberOfPoints(); i < imax; ++i )
   {
      // Get the sample location
      samples->getPoint( i, vtx );
      vtx[3] = 1;

      // Push back a face into the list (will be overwritten soon enough)
      particleFace.push_back( INVALID_INDEX );

      updatePointLocation( i, vtx, searcher );
   }
}

void AWT::AlignParametric::ParticleSurface::setParameters( MatrixType& controls )
{
   FacesNearestPointSearch<T>::P searcher = FacesNearestPointSearch<T>::getInstance( );
   T vtx[3];

   for ( Idx i = 0, imax = samples->getNumberOfPoints(); i < imax; ++i )
   {
      vtx[0] = controls(i,0);
      vtx[1] = controls(i,1);
      vtx[2] = controls(i,2);

      updatePointLocation( i, vtx, searcher );
   }
}