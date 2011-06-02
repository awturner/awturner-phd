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
