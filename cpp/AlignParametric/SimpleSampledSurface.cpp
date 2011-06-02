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
#if 0
#include "SimpleSampledSurface.h"

#include <limits>

template <unsigned char Dims>
struct AWT::AlignParametric::SimpleSampledSurface<Dims>::D
{
   TuplesType::P points;
   Idx iteratorPtr;
};

template <unsigned char Dims>
AWT::AlignParametric::SimpleSampledSurface<Dims>::SimpleSampledSurface( TuplesType::P points )
{
   if ( points->getTupleSize( ) != Dims )
      throw "Wrong number of components in tuple!";

   m_D = new D;

   m_D->points = points;
}

template <unsigned char Dims>
AWT::AlignParametric::SimpleSampledSurface<Dims>::~SimpleSampledSurface( )
{
   delete m_D;
}

template <unsigned char Dims>
typename AWT::AlignParametric::SimpleSampledSurface<Dims>::P AWT::AlignParametric::SimpleSampledSurface<Dims>::getInstance( TuplesType::P points )
{
   AUTOGETINSTANCE( AWT::AlignParametric::SimpleSampledSurface<Dims>, ( points ) );
}

template <unsigned char Dims>
GETNAMEMACRO( AWT::AlignParametric::SimpleSampledSurface<Dims> );

template <unsigned char Dims>
AWT::AlignParametric::Idx AWT::AlignParametric::SimpleSampledSurface<Dims>::getParameterDimensionality( ) const
{
   return Dims;
}

template <unsigned char Dims>
AWT::AlignParametric::Idx AWT::AlignParametric::SimpleSampledSurface<Dims>::getNumberOfSamples( ) const
{
   return m_D->points->getNumberOfPoints( );
}

template <unsigned char Dims>
void AWT::AlignParametric::SimpleSampledSurface<Dims>::getSamples( vnl_matrix<T>& samples ) const
{
   const Idx N = getNumberOfSamples( );
   samples.set_size( Dims, N );

   T vtx[Dims];
   for ( Idx i = 0; i < N; ++i )
   {
      m_D->points->getPoint( i, vtx );

      for ( Idx d = 0; d < Dims; ++d )
         samples( d, i ) = vtx[d];
   }
}

template <unsigned char Dims>
AWT::AlignParametric::Idx AWT::AlignParametric::SimpleSampledSurface<Dims>::getNumberOfParameters( ) const
{
   return m_D->points->getNumberOfPoints( );
}

template <unsigned char Dims>
void AWT::AlignParametric::SimpleSampledSurface<Dims>::getParameters( vnl_matrix<T>& controls ) const
{
   const Idx N = getNumberOfSamples( );
   const Idx C = getParameterDimensionality( );

   controls.set_size( C, N );

   T* vtx = new T[Dims];
   for ( Idx i = 0; i < N; ++i )
   {
      m_D->points->getPoint( i, vtx );

      for ( Idx d = 0; d < C; ++d )
      {
         controls( d, i ) = vtx[d];
      }
   }

   delete [] vtx;
}

template <unsigned char Dims>
void AWT::AlignParametric::SimpleSampledSurface<Dims>::setParameters( vnl_matrix<T>& controls )
{
   const Idx N = getNumberOfSamples( );
   const Idx C = getParameterDimensionality( );

   T vtx[Dims];
   for ( Idx i = 0; i < N; ++i )
   {
      m_D->points->getPoint( i, vtx );

      for ( Idx d = 0; d < C; ++d )
         vtx[d] = controls( d, i );

      m_D->points->setPoint( i, vtx );
   }
}

template <unsigned char Dims>
void AWT::AlignParametric::SimpleSampledSurface<Dims>::jacobian( const AWT::AlignParametric::Idx l, const AWT::AlignParametric::Idx p, vnl_matrix<T>& matrix ) const
{
   const Idx C = getParameterDimensionality( );

   matrix.set_size( Dims, C );
   matrix.fill( 0 );

   if ( l == p )
   {
      for ( Idx d = 0; d < C; ++d )
         matrix( d, d ) = 1;
   }
}

template <unsigned char Dims>
void AWT::AlignParametric::SimpleSampledSurface<Dims>::resetIterator( )
{
   m_D->iteratorPtr = 0;
}

template <unsigned char Dims>
bool AWT::AlignParametric::SimpleSampledSurface<Dims>::next( Idx& l, Idx& p )
{
   const bool ret = m_D->iteratorPtr < getNumberOfSamples( );

   if ( ret )
   {
      l = p = m_D->iteratorPtr++;
   }
   else
   {
      l = p = std::numeric_limits<Idx>::max( );
   }

   return ret;
}

template class AWT::AlignParametric::SimpleSampledSurface<2>;
template class AWT::AlignParametric::SimpleSampledSurface<3>;
#endif