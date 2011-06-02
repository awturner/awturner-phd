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
