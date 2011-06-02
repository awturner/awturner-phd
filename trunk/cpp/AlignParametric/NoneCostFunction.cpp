#include "NoneCostFunction.h"

#include "TypeDefs.h"

AWT::AlignParametric::NoneCostFunction::NoneCostFunction( )
{
}

AWT::AlignParametric::NoneCostFunction::~NoneCostFunction( )
{
}

AWT::AlignParametric::NoneCostFunction::P AWT::AlignParametric::NoneCostFunction::getInstance( )
{
   AUTOGETINSTANCE( AWT::AlignParametric::NoneCostFunction, ( ) );
}

GETNAMEMACRO( AWT::AlignParametric::NoneCostFunction );

double AWT::AlignParametric::NoneCostFunction::calculate( const VectorType& eigenvalues, VectorType& E_grad )
{
   return 0.0;
}
