#include "SamplePoints.h"

#include <vector>

#include "Useful/PrintMacros.h"

struct AWT::SamplePoints::D
{
   std::vector<AWT::MeshIndex> m_SamplePoints;
};

AWT::SamplePoints::SamplePoints( AWT::MeshIndex npoints )
{
   m_D = new D;

   m_D->m_SamplePoints.reserve( npoints );
}

AWT::SamplePoints::~SamplePoints( )
{
   delete m_D;
}

GETNAMEMACRO( AWT::SamplePoints );

AWT::SamplePoints::P AWT::SamplePoints::getInstance( AWT::MeshIndex npoints )
{
   AUTOGETINSTANCE( SamplePoints, (npoints) );
}

const AWT::MeshIndex AWT::SamplePoints::operator[]( AWT::MeshIndex i ) const
{
   return m_D->m_SamplePoints[i];
}

AWT::MeshIndex AWT::SamplePoints::getNumberOfSamples( ) const
{
   return static_cast<MeshIndex>( m_D->m_SamplePoints.size( ) );
}

AWT::MeshIndex AWT::SamplePoints::getSampleIndex( AWT::MeshIndex i ) const
{
   return m_D->m_SamplePoints[i];
}

void AWT::SamplePoints::ensureSize( AWT::MeshIndex i )
{
   m_D->m_SamplePoints.reserve( i );
   modified( );
}

void AWT::SamplePoints::add( AWT::MeshIndex i )
{
   m_D->m_SamplePoints.push_back( i );
   modified( );
}

void AWT::SamplePoints::clear( )
{
   m_D->m_SamplePoints.clear( );
   modified( );
}

