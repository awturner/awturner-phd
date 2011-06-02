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
#include "SamplingGridIrregular.h"

#include "vnl/algo/vnl_svd.h"
#include "Maths/BSpline.h"
#include "LuaPlus.h"
#include <vector>
#include "Useful/PrintMacros.h"

#include <fstream>

#define TYPEDDIV(T,x,y) (static_cast<T>(x)/static_cast<T>(y))

namespace AWT
{
   namespace AlignParametric
   {
      T SamplingGridIrregular::calculateWeight( const T spos[2], const T kpos[2], const T scale[2], const BSpline<T>& bspline )
      {
         T xx[] = { kpos[0]-1, kpos[0], kpos[0]+1 };
         T yy[] = { kpos[1]-1, kpos[1], kpos[1]+1 };

         T delta[2];

         T ret = 0;

         for ( Idx yi = 0; yi < 3; ++yi )
         {
            delta[1] = bspline.evaluate( (yy[yi] - spos[1]) / scale[1] );

            for ( Idx xi = 0; xi < 3; ++xi )
            {
               delta[0] = bspline.evaluate( (xx[xi] - spos[0]) / scale[0] );

               ret += delta[0]*delta[1];
            }
         }

         return ret;
      }

      SamplingGridIrregular::SamplingGridIrregular( TuplesType::P sampleLocations, const Idx _nk[2] )
      {
         originalSampleLocations = sampleLocations;
         
         nk[0] = _nk[0];
         nk[1] = _nk[1];

         const double kernelscale[] = {
            TYPEDDIV(double, 1, 2*nk[0]),
            TYPEDDIV(double, 1, 2*nk[1]),
         };

         BSpline<double> bspline( 256 );

         const Idx nsamples = sampleLocations->getNumberOfPoints( );
         const Idx nkernels = nk[0] * nk[1];

         // The matrix of control values is nice and easy to set up - set it to zero
         // Each row stores x,y component of control value
         for ( Idx i = 0; i < 2; ++i )
         {
            controlValues[i].set_size( nkernels );
            controlValues[i].fill( 0 );
         }

         // The weights matrix is constructed eigOpt that displacement = ws * controlValues
         // i.e. the row corresponds to the sample, column corresponds to kernel
         weightings.set_size( nsamples, nkernels );
         
         const double startw = 1 / ( bspline.evaluate( 0 ) * bspline.evaluate( 0 ) );

         double spos[2];
         double kpos[2];
         for ( Idx r = 0; r < nsamples; ++r )
         {
            sampleLocations->getPoint( r, spos );

            for ( Idx c = 0; c < nkernels; ++c )
            {
               // Calculate where the kernel is
               kpos[0] = ( c % nk[0] + 0.0 ) / ( nk[0] + 0.0 );
               kpos[1] = ( c / nk[0] + 0.0 ) / ( nk[1] + 0.0 );

               const T ww = calculateWeight( spos, kpos, kernelscale, bspline );

               if ( ww != 0 )
               {
                  weightings( r, c ) = ww;
               }
            }
         }
      }

      SamplingGridIrregular::~SamplingGridIrregular( )
      {
      }

      void SamplingGridIrregular::getNKernels( Idx nk[2] ) const
      {
         nk[0] = this->nk[0];
         nk[1] = this->nk[1];
      }

      void SamplingGridIrregular::getPositions( Displacements positions[2] ) const
      {
         getDisplacements( positions );
         
         //PRINTVBL( positions[0][0] );
         //PRINTVBL( positions[1][0] );

         double pos[3];
         for ( Idx i = 0; i < originalSampleLocations->getNumberOfPoints( ); ++i )
         {
            originalSampleLocations->getPoint( i, pos );

            //PRINTVEC( pos, 3 );

            positions[0][i] += pos[0];
            positions[1][i] += pos[1];
         }

         /*
         BLANKLINE;
         PRINTVBL( positions[0][0] );
         PRINTVBL( positions[1][0] );
         BLANKLINE;

         PAUSE;
         */
      }

      // Calculate the displacement from start position
      void SamplingGridIrregular::getDisplacements( Displacements displacements[2] ) const
      {
         for ( int i = 0; i < 2; ++i )
         {
            //controlValues[i].fill( 0 );

            displacements[i].set_size( weightings.rows() );
            weightings.mult( controlValues[i], displacements[i] );
         }
      }

      const KernelWeights& SamplingGridIrregular::getWeights( ) const
      {
         return weightings;
      }

      ControlValues& SamplingGridIrregular::getParameters( Idx axis )
      {
         return controlValues[axis];
      }
   }
}

AWT::AlignParametric::SamplingGridIrregular::P AWT::AlignParametric::SamplingGridIrregular::getInstance( TuplesType::P sampleLocations, const AWT::AlignParametric::Idx _nk[])
{
   AUTOGETINSTANCE( AWT::AlignParametric::SamplingGridIrregular, ( sampleLocations, _nk ) );
}

GETNAMEMACRO( AWT::AlignParametric::SamplingGridIrregular );

