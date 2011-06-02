#include "NormalSpaceSampler.h"

#include "Mesh.h"
#include "Tuples.h"
#include "AreaAveragedNormalCalculator.h"
#include "Useful/ArrayVectorFunctions.h"
#include "Maths/Constants.h"
#include "SamplePoints.h"
#include <cmath>

#include <vector>

template <class T>
struct AWT::NormalSpaceSampler<T>::D
{
   unsigned int m_LatDivs;
   unsigned int m_LngDivs;

   unsigned int m_NSamples;
};

template <class T>
AWT::NormalSpaceSampler<T>::NormalSpaceSampler( )
{
   m_D = new D;

   m_D->m_LatDivs = 16;
   m_D->m_LngDivs = 16;

   m_D->m_NSamples = 1000;
}

template <class T>
AWT::NormalSpaceSampler<T>::~NormalSpaceSampler( )
{
   delete m_D;
}

template <class T>
typename AWT::NormalSpaceSampler<T>::P AWT::NormalSpaceSampler<T>::getInstance( )
{
   AUTOGETINSTANCE( AWT::NormalSpaceSampler<T>, ( ) );
}

template <class T>
GETNAMEMACRO( AWT::NormalSpaceSampler<T> );

template <class T>
void AWT::NormalSpaceSampler<T>::setLatitudeDivisions( const unsigned int d )
{
   m_D->m_LatDivs = d;
   modified( );
}

template <class T>
unsigned int AWT::NormalSpaceSampler<T>::getLatitudeDivisions( ) const
{
   return m_D->m_LatDivs;
}

template <class T>
void AWT::NormalSpaceSampler<T>::setLongitudeDivisions( const unsigned int d )
{
   m_D->m_LngDivs = d;
   modified( );
}

template <class T>
unsigned int AWT::NormalSpaceSampler<T>::getLongitudeDivisions( ) const
{
   return m_D->m_LngDivs;
}

template <class T>
unsigned int AWT::NormalSpaceSampler<T>::getMaximumNumberOfSamples( ) const
{
   return m_D->m_NSamples;
}

template <class T>
void AWT::NormalSpaceSampler<T>::setMaximumNumberOfSamples( const unsigned int n )
{
   m_D->m_NSamples = n;
   modified( );
}

template <class T>
AWT::SamplePoints::P AWT::NormalSpaceSampler<T>::sampleMesh( typename AWT::Mesh<T>::P mesh )
{
   std::vector< std::vector<MeshIndex> > bins;

   // Create empty bins
   for ( unsigned int lng = 0; lng < m_D->m_LngDivs; ++lng )
      for ( unsigned int lat = 0; lat < m_D->m_LatDivs; ++lat )
         bins.push_back( std::vector<MeshIndex>( ) );
   if ( m_D->m_NSamples < mesh->getNumberOfVertices( ) )
   {
      if ( *mesh->getVertexNormals( ) == 0 )
      {
         AreaAveragedNormalCalculator<T>::getInstance( )->calculateNormalsAndSet( mesh );
      }

      Tuples<T>::P normals = mesh->getVertexNormals( );

      T nml[3];
      T angLat, angLng;

      unsigned int binLat, binLng, randPoint;

      for ( MeshIndex v = 0, vmax = normals->getNumberOfPoints(); v < vmax; ++v )
      {
         normals->getPoint( v, nml );
         normalize( nml, 3 );

         // Calculate the latitude and longitude
         angLat = asin( nml[2] );           // This is in the range [-pi/2..pi/2]
         angLng = atan2( nml[0], nml[1] );  // This is in the range [-pi..pi]

         // Shift and round these and put them into bins
         binLat = static_cast<unsigned int>( floor( ( ( angLat * 2 / Constants<T>::Pi ) + 1 ) / 2 * m_D->m_LatDivs ) );
         binLng = static_cast<unsigned int>( floor( ( ( angLng     / Constants<T>::Pi ) + 1 ) / 2 * m_D->m_LngDivs ) );

         binLat = std::min( binLat, m_D->m_LatDivs-1 );
         binLng = std::min( binLng, m_D->m_LngDivs-1 );

         bins[ binLat * m_D->m_LngDivs + binLng ].push_back( v );

      }

      std::vector<unsigned int> usedBins;
      for ( size_t i = 0; i < bins.size( ); ++i )
      {
         if ( !bins[i].empty( ) )
            usedBins.push_back( i );
      }

      SamplePoints::P ret = SamplePoints::getInstance( m_D->m_NSamples );
      
      for ( MeshIndex v = 0; v < m_D->m_NSamples; ++v )
      {
         // This is naughty for the moment - it gives undue weighting to the poles
         unsigned int binIdx = usedBins[ rand( ) % usedBins.size( ) ];
         
         // Pick a point from the bin selected
         randPoint = static_cast<unsigned int>( rand( ) % bins[ binIdx ].size( ) );
         ret->add( bins[ binIdx ][randPoint] );
      }

      return ret;
   }
   else
   {
      SamplePoints::P ret = SamplePoints::getInstance( mesh->getNumberOfVertices( ) );

      //for ( MeshIndex v = 0; v < mesh->getNumberOfVertices( ); ++v )
      MESH_EACHVERTEX( mesh, v )
         ret->add( v );

      return ret;
   }
}

template class AWT::NormalSpaceSampler<double>;
template class AWT::NormalSpaceSampler<float>;