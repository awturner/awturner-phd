#include "NormalSearchFilter.h"

struct AWT::SimpleMesh::NormalSearchFilter::D
{
   Point testPoint;
   Point testNormal;

   double cosineThreshold;
};

AWT::SimpleMesh::NormalSearchFilter::NormalSearchFilter()
{
   m_D = new D;

   m_D->cosineThreshold = 0.707;
}

AWT::SimpleMesh::NormalSearchFilter::~NormalSearchFilter()
{
   delete m_D;
}

AWT::SimpleMesh::NormalSearchFilter::P AWT::SimpleMesh::NormalSearchFilter::getInstance( )
{
   AUTOGETINSTANCE( AWT::SimpleMesh::NormalSearchFilter, ( ) );
}

GETNAMEMACRO( AWT::SimpleMesh::NormalSearchFilter );

void AWT::SimpleMesh::NormalSearchFilter::setTestPoint(const AWT::SimpleMesh::Point p)
{
   m_D->testPoint = p;
   modified( );
}

bool AWT::SimpleMesh::NormalSearchFilter::check( const Point p, const Index i ) const
{
   return true;

   Point delta = p - m_D->testPoint;

   if ( delta.squared_magnitude() < 1 )
      return true;

   return inner_product<double>( delta.normalize( ), m_D->testNormal ) >= m_D->cosineThreshold;
}

AWT::SimpleMesh::Point AWT::SimpleMesh::NormalSearchFilter::getTestPoint( ) const
{
   return m_D->testPoint;
}

void AWT::SimpleMesh::NormalSearchFilter::setTestNormal(const AWT::SimpleMesh::Point p)
{
   m_D->testNormal = p / p.magnitude( );
   modified( );
}

AWT::SimpleMesh::Point AWT::SimpleMesh::NormalSearchFilter::getTestNormal( ) const
{
   return m_D->testNormal;
}

void AWT::SimpleMesh::NormalSearchFilter::setCosineThreshold( const double v )
{
   if ( v != m_D->cosineThreshold )
   {
      m_D->cosineThreshold = v;
      modified( );
   }
}

double AWT::SimpleMesh::NormalSearchFilter::getCosineThreshold( ) const
{
   return m_D->cosineThreshold;
}