#include "DeterminantOfCovariance.h"

#include "TypeDefs.h"

struct AWT::AlignParametric::DeterminantOfCovariance::D
{
   T epsilon;
};

AWT::AlignParametric::DeterminantOfCovariance::DeterminantOfCovariance( )
{
   m_D = new D;

   m_D->epsilon = 1.0;
}

AWT::AlignParametric::DeterminantOfCovariance::~DeterminantOfCovariance( )
{
   delete m_D;
}

AWT::AlignParametric::DeterminantOfCovariance::P AWT::AlignParametric::DeterminantOfCovariance::getInstance( )
{
   AUTOGETINSTANCE( AWT::AlignParametric::DeterminantOfCovariance, ( ) );
}

GETNAMEMACRO( AWT::AlignParametric::DeterminantOfCovariance );

double AWT::AlignParametric::DeterminantOfCovariance::getEpsilon( ) const
{
   return m_D->epsilon;
}

void AWT::AlignParametric::DeterminantOfCovariance::setEpsilon( const T v )
{
   if ( v != m_D->epsilon )
   {
      m_D->epsilon = v;
      modified( );
   }
}

double AWT::AlignParametric::DeterminantOfCovariance::calculate( const VectorType& eigenvalues, VectorType& E_grad )
{
   const Idx M = eigenvalues.size( );

   E_grad.set_size( M );
   E_grad.fill( 0 );

   T E = 0;
   for ( Idx i = 0; i < M; ++i )
   {
      E         += log( eigenvalues(i) + m_D->epsilon );
      E_grad(i)  = 1 / ( eigenvalues(i) + m_D->epsilon );

      DEBUGMACRO( eigenvalues(i) << "\t" << E_grad(i) );
   }

   E -= M*log( m_D->epsilon );

   return E;
}
