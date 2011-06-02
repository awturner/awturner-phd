#include "LMICP.h"

#include "Mesh/TuplesFunctions.h"
#include "Mesh/MeshFunctions.h"

#include "Useful/PrintMacros.h"

struct AWT::LMICP::D
{
   Tuples<double>::P m_Movable;
   Tuples<double>::P m_MovableNormals;
   Tuples<double>::P m_Fixed;
   Tuples<double>::P m_FixedNormals;

   double m_Centroid[3];

   double (*m_Func)( double );
};

AWT::LMICP::LMICP( Tuples<double>::P movable, Tuples<double>::P movableNormals, Tuples<double>::P fixed, Tuples<double>::P fixedNormals, const double centroid[3], double (*func)( double ) )
: vnl_least_squares_function( 7, movable->getNumberOfPoints( ), vnl_least_squares_function::no_gradient )
{
   m_D = new D;

   m_D->m_Movable = movable;
   m_D->m_MovableNormals = movableNormals;
   m_D->m_Fixed = fixed;
   m_D->m_FixedNormals = fixedNormals;

   m_D->m_Centroid[0] = centroid[0];
   m_D->m_Centroid[1] = centroid[1];
   m_D->m_Centroid[2] = centroid[2];

   m_D->m_Func = func;
}

AWT::LMICP_PointPoint::LMICP_PointPoint( Tuples<double>::P movable, Tuples<double>::P movableNormals, Tuples<double>::P fixed, Tuples<double>::P fixedNormals, const double centroid[3], double (*func)( double ) )
: LMICP( movable, movableNormals, fixed, fixedNormals, centroid, func )
{
}

AWT::LMICP_PointPlane::LMICP_PointPlane( Tuples<double>::P movable, Tuples<double>::P movableNormals, Tuples<double>::P fixed, Tuples<double>::P fixedNormals, const double centroid[3], double (*func)( double ) )
: LMICP( movable, movableNormals, fixed, fixedNormals, centroid, func )
{
}

AWT::LMICP_PointPoint::~LMICP_PointPoint( )
{
}

AWT::LMICP_PointPlane::~LMICP_PointPlane( )
{
}

AWT::LMICP::~LMICP( )
{
   delete m_D;
}

AWT::LMICP::P AWT::LMICP_PointPoint::getInstance( Tuples<double>::P movable, Tuples<double>::P movableNormals, Tuples<double>::P fixed, Tuples<double>::P fixedNormals, const double centroid[3], double (*func)( double ) )
{
   AUTOGETINSTANCE( AWT::LMICP_PointPoint, ( movable, movableNormals, fixed, fixedNormals, centroid, func ) );
}

AWT::LMICP::P AWT::LMICP_PointPlane::getInstance( Tuples<double>::P movable, Tuples<double>::P movableNormals, Tuples<double>::P fixed, Tuples<double>::P fixedNormals, const double centroid[3], double (*func)( double ) )
{
   AUTOGETINSTANCE( AWT::LMICP_PointPlane, ( movable, movableNormals, fixed, fixedNormals, centroid, func ) );
}

GETNAMEMACRO( AWT::LMICP_PointPoint );
GETNAMEMACRO( AWT::LMICP_PointPlane );

vnl_matrix_fixed<double,4,4> AWT::LMICP::makeTransformation( const vnl_vector<double>& params, const double centroid[3] )
{
   const double axis[] = { params[0], params[1], params[2] };
   const double theta  = params[3];
   const double translate[] = { params[4], params[5], params[6] };

   // Rotate about the centroid so that the movements due to rotation are smallest
   return TuplesFunctions<double>::rotationTranslationAboutPoint( axis, theta, centroid, translate );
}

void AWT::LMICP_PointPoint::f( const vnl_vector<double>& params, vnl_vector<double>& residuals )
{
   vnl_matrix_fixed<double,4,4> R = makeTransformation( params, m_D->m_Centroid );

   vnl_vector<double> vecFixed( 4 );
   vnl_vector<double> vecMovable( 4 );

   for ( MeshIndex i = 0, imax = m_D->m_Movable->getNumberOfPoints( ); i < imax; ++i )
   {
      m_D->m_Movable->getPoint( i, vecMovable.data_block( ) );
      m_D->m_Fixed->getPoint( i, vecFixed.data_block( ) );

      vecMovable( 3 ) = vecFixed( 3 ) = 1;

      vecMovable = R * vecMovable;

      // This is the linear residual
      const double residual = ( vecFixed - vecMovable ).magnitude( );

      residuals( i ) = m_D->m_Func( residual );
   }
}

void AWT::LMICP_PointPlane::f( const vnl_vector<double>& params, vnl_vector<double>& residuals )
{
   vnl_matrix_fixed<double,4,4> R = makeTransformation( params, m_D->m_Centroid );

   vnl_vector<double> vecFixed( 4 );
   vnl_vector<double> vecMovable( 4 );
   vnl_vector<double> nmlFixed( 4 );

   for ( MeshIndex i = 0, imax = m_D->m_Movable->getNumberOfPoints( ); i < imax; ++i )
   {
      m_D->m_Movable->getPoint( i, vecMovable.data_block( ) );
      m_D->m_Fixed->getPoint( i, vecFixed.data_block( ) );
      m_D->m_FixedNormals->getPoint( i, nmlFixed.data_block( ) );

      vecMovable( 3 ) = vecFixed( 3 ) = 1;
      nmlFixed( 3 ) = 0;

      vecMovable = R * vecMovable;

      // This is the point-to-plane residual
      const double residual = dot_product<double>( vecFixed - vecMovable, nmlFixed );

      // This is the linear residual
      residuals( i ) = m_D->m_Func( residual );
   }

}