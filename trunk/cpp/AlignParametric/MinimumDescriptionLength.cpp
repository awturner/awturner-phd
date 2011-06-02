#include "MinimumDescriptionLength.h"

#include "TypeDefs.h"

struct AWT::AlignParametric::MinimumDescriptionLength::D
{
   T lambdaCut;
};

AWT::AlignParametric::MinimumDescriptionLength::MinimumDescriptionLength( )
{
   m_D = new D;

   m_D->lambdaCut = 1;
}

AWT::AlignParametric::MinimumDescriptionLength::~MinimumDescriptionLength( )
{
   delete m_D;
}

AWT::AlignParametric::MinimumDescriptionLength::P AWT::AlignParametric::MinimumDescriptionLength::getInstance( )
{
   AUTOGETINSTANCE( AWT::AlignParametric::MinimumDescriptionLength, ( ) );
}

GETNAMEMACRO( AWT::AlignParametric::MinimumDescriptionLength );

AWT::AlignParametric::T AWT::AlignParametric::MinimumDescriptionLength::getLambdaCut( ) const
{
   return m_D->lambdaCut;
}

void AWT::AlignParametric::MinimumDescriptionLength::setLambdaCut( const AWT::AlignParametric::T v )
{
   if ( v != m_D->lambdaCut )
   {
      m_D->lambdaCut = v;
      modified( );
   }
}

AWT::AlignParametric::T AWT::AlignParametric::MinimumDescriptionLength::calculate( const VectorType& eigenvalues, VectorType& E_grad )
{
   const Idx M = eigenvalues.size( );

   E_grad.set_size( M );  E_grad.fill( 0 );

   T ret = 0;
   const T lamCut = m_D->lambdaCut;
   for ( Idx i = 0; i < M; ++i )
   {
      if ( eigenvalues(i) > lamCut )
      {
         ret += 1 + log( eigenvalues(i) ) - log( lamCut );
         E_grad(i)  = 1 / eigenvalues(i);
      }
      else
      {
         ret += eigenvalues(i) / lamCut;
         E_grad(i) = 1 / lamCut;
      }
   }

   return ret;
}
