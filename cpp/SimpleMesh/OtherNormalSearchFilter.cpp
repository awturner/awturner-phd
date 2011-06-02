#include "OtherNormalSearchFilter.h"

#include "MeshFunctions.h"

struct AWT::SimpleMesh::OtherNormalSearchFilter::D
{
   Point testPoint;
   Mesh::P mesh;

   double cosineThreshold;
};

AWT::SimpleMesh::OtherNormalSearchFilter::OtherNormalSearchFilter( Mesh::P mesh )
{
   m_D = new D;

   m_D->mesh = mesh;
   m_D->cosineThreshold = 0.707;
}

AWT::SimpleMesh::OtherNormalSearchFilter::~OtherNormalSearchFilter()
{
   delete m_D;
}

AWT::SimpleMesh::OtherNormalSearchFilter::P AWT::SimpleMesh::OtherNormalSearchFilter::getInstance( Mesh::P mesh )
{
   AUTOGETINSTANCE( AWT::SimpleMesh::OtherNormalSearchFilter, ( mesh ) );
}

GETNAMEMACRO( AWT::SimpleMesh::OtherNormalSearchFilter );

void AWT::SimpleMesh::OtherNormalSearchFilter::setTestPoint(const AWT::SimpleMesh::Point p)
{
   m_D->testPoint = p;
   modified( );
}

bool AWT::SimpleMesh::OtherNormalSearchFilter::check( const Point p, const Index i ) const
{
   return true;

   // Need to look up the normal on the other mesh
   Point nml = calculateFaceNormal( m_D->mesh, i );

   Point delta = m_D->testPoint - p;
   if ( delta.squared_magnitude() < 1 )
      return true;

   return inner_product<double>( delta.normalize( ), nml ) >= m_D->cosineThreshold;
}

AWT::SimpleMesh::Point AWT::SimpleMesh::OtherNormalSearchFilter::getTestPoint( ) const
{
   return m_D->testPoint;
}

AWT::SimpleMesh::Mesh::P AWT::SimpleMesh::OtherNormalSearchFilter::getTestMesh( ) const
{
   return m_D->mesh;
}

void AWT::SimpleMesh::OtherNormalSearchFilter::setCosineThreshold( const double v )
{
   if ( v != m_D->cosineThreshold )
   {
      m_D->cosineThreshold = v;
      modified( );
   }
}

double AWT::SimpleMesh::OtherNormalSearchFilter::getCosineThreshold( ) const
{
   return m_D->cosineThreshold;
}