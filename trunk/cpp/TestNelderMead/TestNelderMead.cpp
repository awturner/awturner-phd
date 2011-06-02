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
#include "Maths/NelderMead.h"
#include "Maths/OptimizationFunction.h"

#include "Useful/Noise.h"

using namespace AWT;

template <class T>
class RosenbrockFunction : public AWT::OptimizationFunction<T>
{
public:
   typedef ManagedAutoPointer<RosenbrockFunction<T>> P;

protected:
   RosenbrockFunction( const unsigned int ndims )
      : m_Dims( ndims )
   {
   }

   virtual ~RosenbrockFunction( )
   {
   }

public:
   static P getInstance( const unsigned int ndims )
   {
      AUTOGETINSTANCE( RosenbrockFunction<T>, ( ndims ) );
   }

   std::string getClassName( ) const;

   virtual unsigned int getNumberOfVariables( ) const
   {
      return m_Dims;
   }

   virtual T evaluate( const Matrix<T>& p ) const
   {
      T ret = 0;

      T tmpA, tmpB;

      for ( unsigned int i = 0; i < m_Dims - 1; ++i )
      {
         tmpA = 1 - p[i][0];
         tmpB = p[i+1][0] - p[i][0]*p[i][0];

         ret += tmpA*tmpA + 100*tmpB*tmpB;
      }

      return ret;
   }

protected:
   unsigned int m_Dims;
};

template <class T>
GETNAMEMACRO( RosenbrockFunction<T> );

typedef double T;

int main( int argc, char** argv )
{
   const int ndims = 2;

   T centr[ndims];

   Noise<T>::timeSeed( );

   for ( int i = 0; i < 100; ++i )
   {
      RosenbrockFunction<T>::P rosenbrock = RosenbrockFunction<T>::getInstance( ndims );
      NelderMead<T>::P nelder = NelderMead<T>::getInstance( rosenbrock );

      Matrix<T> initialPos( ndims, 1 );
      Matrix<T> initialStd( ndims, 1 );

      for ( MatrixIndex i = 0; i < ndims; ++i )
      {
         initialPos.setElement( i, 0, Noise<T>::randu( -100, 100 ) );
         initialStd.setElement( i, 0, 10 );
      }

      nelder->setInitialGuess( initialPos, initialStd );
      //PRINTVBL2( "init", nelder->getCentroid( ) );
      nelder->initialize( );

      unsigned int niter;
      for ( niter = 0; nelder->getRange( ) > 1e-16; ++niter )
      {
         nelder->step( );
      }

      nelder->getCentroid( ).getColMajor( centr );

      PRINTVBL( niter );
      PRINTVEC( centr, ndims );
      PRINTVBL( nelder->getSimplex( ) );
   }

   ManagedObject::report( );
}