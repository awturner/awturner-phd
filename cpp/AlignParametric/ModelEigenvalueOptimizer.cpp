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
#include "ModelEigenvalueOptimizer.h"

#include "Useful/Timestamp.h"

#include "Mesh/Tuples.h"
#include "Mesh/TuplesFunctions.h"
#include "Mesh/MeshFunctions.h"
#include "Mesh/VTKMeshWriter.h"
#include "Mesh/MeshImpl.h"
#include "Mesh/VNLTuples.h"

#include "RefinementStrategy.h"

#include "Useful/ProfTimer.h"
#include "Useful/ArrayVectorFunctions.h"
#include "Useful/ColouredConsole.h"
#include "Useful/Timestamp.h"
#include "Useful/VectorIterator.h"
#include "Useful/Noise.h"

#include "vnl/algo/vnl_svd.h"
#include "vnl/algo/vnl_symmetric_eigensystem.h"
#include "vnl/vnl_det.h"
#include "vnl/vnl_trace.h"
#include "vnl/vnl_diag_matrix.h"
#include "vnl/vnl_vector.h"
#include "vnl/vnl_matlab_filewrite.h"

#include <vector>
#include <fstream>
#include <stdio.h>
#include <iomanip>
#include <time.h>

using namespace AWT;
using namespace AWT::AlignParametric;

struct AWT::AlignParametric::ModelEigenvalueOptimizer::D
{
   struct SampledSurfaceEx
   {
      SurfaceType::P       surface;
      Transformation transform;

      MatrixType           sampleLocations;
      MatrixType           parameterUpdates;
   };

   void updateStoredSampleLocations( SampledSurfaceEx& ssx )
   {
      MatrixType samples;
      ssx.surface->getSamples( samples );

      // Transform the samples before pushing them in
      ssx.sampleLocations = ssx.transform * samples;

      /*
      Idx is[] = { 0, 1, 63 };
      for (Idx i = 0; i < 3; ++i)
      {
         PRINTVBL(ssx.sampleLocations.get_column(is[i]));
      }
      */

      // Make sure that there are the correct number of samples
      const Idx N = self->getNumberOfSamplesPerSurface();
      if ( samples.cols() != N )
         throw "Incorrect number of samples";

      // Do some checking to make sure that the coordinates are
      // homogeneous and correctly normalized
      if ( samples.rows() != 4 )
         throw "Samples locations must be homogeneous";

      for ( Idx c = 0; c < N; ++c )
         if ( samples(3,c) != 1 )
            throw "Homogeneous sample must have 1 in final row!";
   }

   void updateStoredSampleLocations( )
   {
      const Idx M = getM( );
      for ( Idx s = 0; s < M; ++s )
         updateStoredSampleLocations( surfaceExes[s] );
   }

   // Arun's method
   void pairwiseAlign( const MatrixType& x, const MatrixType& y, Transformation& R )
   {
      // Reset this
      R.set_identity( );

      const Idx N = self->getNumberOfSamplesPerSurface();

      // Calculate the mean-centred covariance matrix
      MatrixType sum_xy( 3, 3 ); sum_xy.fill( 0 );
      VectorType sum_x( 3 );     sum_x.fill( 0 );
      VectorType sum_y( 3 );     sum_y.fill( 0 );
      T var_x = 0;
      T var_y = 0;
      T var_xy = 0;

      T sum_w = 0;

      for ( Idx i = 0; i < N; ++i )
      {
         const T w = sampleWeights(i);

         VectorType xi = x.get_column(i);
         VectorType yi = y.get_column(i);

         for ( Idx r = 0; r < 3; ++r )
         {
            for ( Idx c = 0; c < 3; ++c )
            {
               sum_xy(r,c) += w*x(r,i)*y(c,i);
            }

            sum_x(r) += w*x(r,i);
            sum_y(r) += w*y(r,i);

            var_x  += w*x(r,i)*x(r,i);
            var_y  += w*y(r,i)*y(r,i);
            var_xy += w*x(r,i)*y(r,i);
         }

         // Need the total weight to normalize correctly
         sum_w += w;
      }
      
      const MatrixType Cov = sum_xy - outer_product(sum_x, sum_y) / sum_w;
      
      // Calculate the relative scale of the two point clouds
      var_x  -= inner_product( sum_x, sum_x ) / sum_w;
      var_y  -= inner_product( sum_y, sum_y ) / sum_w;
      var_xy -= inner_product( sum_x, sum_y ) / sum_w;

      // Convert these to means
      sum_x /= sum_w;
      sum_y /= sum_w;

      // Can only calculate a rotation if there are sufficient points
      if ( rotationAllowed && N >= 3 )
      {
         // Take SVD to get the rotations
         vnl_svd<T> svd( Cov );

         MatrixType rot = svd.U() * svd.V().transpose();
         T det = rot(0,0)*(rot(1,1)*rot(2,2)-rot(2,1)*rot(1,2))+rot(0,1)*(rot(1,2)*rot(2,0)-rot(2,2)*rot(1,0))+rot(0,2)*(rot(1,0)*rot(2,1)-rot(2,0)*rot(1,1));
         
         // Umeyana modification
         if ( det < 0 )
         {
            vnl_diag_matrix<T> diag( 3 );
            diag.fill_diagonal( 1 );
            diag(2) = -1;
            rot = svd.U() * diag * svd.V().transpose();
         }

         for ( Idx r = 0; r < 3; ++r )
            for ( Idx c = 0; c < 3; ++c )
               R(r,c) = rot(r,c);
      }

      // Can only calculate a scale if there are sufficient points
      if ( N >= 2 && scaleNormalization != ModelEigenvalueOptimizer::NRML_NONE )
      {
         T scale;

         switch ( scaleNormalization )
         {
         case NRML_HYPERSPHERE:
            scale = sqrt( var_x / var_y );
            break;
         case NRML_HYPERPLANE:
            scale = (var_xy / var_y);
            break;
         default:
            scale = 1;
         }

         R *= scale;
         R(0,3) = R(1,3) = R(2,3) = 0;
         R(3,0) = R(3,1) = R(3,2) = 0;
         R(3,3) = 1;
      }

      // Can only calculate a translation if there are sufficient points
      if ( translationAllowed && N >= 1 )
      {
         const VectorType t = sum_x - R.extract(3,3)*sum_y;

         R(0,3) = t(0);
         R(1,3) = t(1);
         R(2,3) = t(2);
      }
   }

   T shapeRms( const MatrixType& shape )
   {
      const Idx N = self->getNumberOfSamplesPerSurface();
      PRINTVBL( N );

      T          sum_w_xt_x = 0;
      VectorType sum_w_x(3); sum_w_x.fill( 0 );
      T          sum_w = 0;

      for ( Idx i = 0; i < N; ++i )
      {
         const T weight = sampleWeights(i);

         for ( Idx ax = 0; ax < 3; ++ax )
         {
            sum_w_xt_x  += weight * shape(ax,i) * shape(ax,i);
            sum_w_x(ax) += weight * shape(ax,i);
         }

         sum_w += weight;
      }

      return sqrt( sum_w_xt_x - sum_w_x.squared_magnitude() / sum_w );
   }

   Transformation scaleByMatrix( const T scaleBy )
   {
      Transformation ret;
      ret.set_identity();
      ret(0,0) = ret(1,1) = ret(2,2) = scaleBy;
      return ret;
   }

   void alignSamples( )
   {
      const Idx M = getM( );

      // If we've only got one surface, there's nothing to do
      // (Actually, then there's no correspondences to optimize... but hey)
      if ( M <= 1 )
         return;

      // Using Algorithm 2.1 of "Statistical Models of Shape", Davies et al., 2008.

      // Choose one shape as the reference frame, call it x_ref
      MatrixType x_ref = surfaceExes[0].sampleLocations;

      if ( scaleNormalization != NRML_NONE )
      {
         // Normalize the scale so that ||x_ref|| = 1
         const T theRms = shapeRms( x_ref );

         const T normalizeScale = 1 / theRms;
         PRINTVBLNL(normalizeScale);
         PRINTVBLNL(theRms);

         x_ref = scaleByMatrix( normalizeScale ) * x_ref;
      }

      // Set the initial estimate of the mean shape to be x_ref
      meanShape = x_ref;

      Transformation compareTo;
      compareTo.set_identity();

      while ( true )
      {
         // Perform pairwise alignment of all shapes to the current estimate of the mean shape
         T maxFroNorm = 0;
         for ( Idx s = 0; s < M; ++s )
         {
            SampledSurfaceEx& ssx = surfaceExes[s];

            Transformation R;
            pairwiseAlign( meanShape, ssx.sampleLocations, R );
            
            for ( Idx r = 0; r < 4; ++r )
               for ( Idx c = 0; c < 4; ++c )
                  if ( R(r,c) != R(r,c) )
                  {
                     PRINTVBLNL(meanShape);
                     PRINTVBLNL(ssx.sampleLocations);
                     throw "NaN in rotation matrix!";
                  }

            // Update the positions of these samples
            ssx.sampleLocations = R*ssx.sampleLocations;
            
            // Update the transformation estimate
            ssx.transform = R*ssx.transform;

            maxFroNorm = std::max( maxFroNorm, (R-compareTo).fro_norm() );
         }
         
         //PRINTVBL( maxFroNorm );

         // Recompute the mean of the samples
         calculateMeanShape( );

         if ( maxFroNorm < 1e-9 )
            break;
         
         // Align the mean shape to the reference frame

         Transformation Rmean;
         pairwiseAlign( x_ref, meanShape, Rmean );
         meanShape = Rmean * meanShape;
      }

      // Recompute the mean of the samples
      calculateMeanShape( );

      // Construct a normalizing transformation
      Transformation Rnorm;
      Rnorm.set_identity();
      bool RnormChanged = false;

      const VectorType sw = sampleWeights.diagonal();
      const VectorType centroid = meanShape * sw / sw.sum();

      {
         // Shift centroid to origin
         Transformation Rtrans;
         Rtrans.set_identity();

         for ( Idx ax = 0; ax < 3; ++ax )
            Rtrans(ax,3) = -centroid(ax);

         // Increment the transformation
         Rnorm = Rtrans * Rnorm;
      }

      {
         // Scale to mean scale of the models
         double scMean = 0;
         for ( Idx s = 0; s < M; ++s )
         {
            const double sc = calculateScale( surfaceExes[s].transform );
            scMean += sc;
         }
         scMean = M / scMean;

         Transformation Rscale;
         Rscale.set_identity();
         Rscale *= scMean;
         Rscale(3,3) = 1;

         // Increment the transformation
         Rnorm = Rscale * Rnorm;
      }

      {
         // Rotate so that principal axes are aligned with coordinate axes
         Transformation covar(0.0);
         const Idx N = meanShape.cols();
         for ( Idx v = 0; v < N; ++v )
         {
            const VectorType p = meanShape.get_column(v) - centroid;
            covar += outer_product( sw(v) * p, p );
         }

         vnl_svd<double> svd(covar);
         Transformation Rrot = svd.U().transpose();

         // Make sure that the transformation is just a rotation, not reflection
         if ( vnl_det<double>(Rrot) < 0 )
         {
            Transformation flip; flip.set_identity();
            flip(2,2) = -1;

            Rrot = flip * Rrot;
         }

         // Apply an arbitrary rule to avoid repeated flipping
         // Pick the orientation with the smallest sum-squared distance
         // away from the min coordinate plane in the x- and y-directions
         // Sum squared sum is not calculated since this is common in the
         // quantities being compared
         T sums[]   = { 0, 0, 0 };
         T mins[]  = {  std::numeric_limits<T>::infinity(),  std::numeric_limits<T>::infinity(),  std::numeric_limits<T>::infinity() };
         T maxs[]  = { -std::numeric_limits<T>::infinity(), -std::numeric_limits<T>::infinity(), -std::numeric_limits<T>::infinity() };
         for (Idx v = 0; v < N; ++v)
         {
            const VectorType p = Rrot * meanShape.get_column(v);

            for (Idx ax = 0; ax < 3; ++ax)
            {
               sums[ax]  += sampleWeights(v)*p(ax);
               mins[ax]  = std::min(mins[ax], p(ax));
               maxs[ax]  = std::max(maxs[ax], p(ax));
            }
         }

         const T sumsSampleWeights = sampleWeights.diagonal().sum();
         Transformation momentFlip; momentFlip.set_identity();
         bool flipFlag = false;

         for (Idx ax = 0; ax < 2; ++ax)
         {
            if ( (mins[ax]*mins[ax]*sumsSampleWeights - 2*mins[ax]*sums[ax]) > (maxs[ax]*maxs[ax]*sumsSampleWeights - 2*maxs[ax]*sums[ax]) )
            {
               momentFlip(ax,ax) = -1;
               flipFlag ^= true;
            }
         }

         if (flipFlag)
         {
            // Just to make sure that it remains a rotation
            momentFlip(2,2) = -1;
         }

         Rrot = momentFlip * Rrot;

         /*
         // Now, pick the flippage with the least rotation
         // This is the matrix (flip * Rrot) with the smallest trace
         Transformation flip; flip.set_identity();
         Transformation bestFlip = flip; double bestTrace = vnl_trace<double>(Rrot);
         
         for (Idx i = 0; i < 3; ++i)
         {
            for (Idx j = 0; j < 3; ++j)
               flip(j,j) = (j == i) ? 1 : -1;

            const double thisTrace = vnl_trace<double>(flip * Rrot);
            if (thisTrace > bestTrace)
            {
               bestTrace = thisTrace;
               bestFlip = flip;
            }
         }
         */

         Rnorm = momentFlip * Rrot * Rnorm;
      }

      {
         //PRINTVBLNL(Rnorm);
         //PRINTVBL( vnl_det<T>(Rnorm) );
         for ( Idx s = 0; s < M; ++s )
         {
            //PRINTVBL( vnl_det<T>(surfaceExes[s].transform) );
            incrementSurfaceTransform( surfaceExes[s], Rnorm );
         }

         // Recalculate the mean shape
         calculateMeanShape( );
      }
   }

   void incrementSurfaceTransform( SampledSurfaceEx& ssx, const Transformation& R )
   {
      ssx.sampleLocations = R*ssx.sampleLocations;
      ssx.transform       = R*ssx.transform;
   }

   T calculateScale( Transformation& trans )
   {
      const T detMag = vnl_svd<T>( trans ).determinant_magnitude( );
      return pow( detMag, 1.0/3.0 );
   }

   void calculateMeanShape( )
   {
      const Idx M = getM( );

      if ( meanShape.rows() != surfaceExes[0].sampleLocations.rows() || meanShape.rows() != surfaceExes[0].sampleLocations.cols() )
         meanShape.set_size( surfaceExes[0].sampleLocations.rows(), surfaceExes[0].sampleLocations.cols() );

      meanShape.fill( 0 );
      for ( Idx s = 0; s < M; ++s )
         meanShape += surfaceExes[s].sampleLocations;
      meanShape /= M;

      VectorType newSampleWeights( self->getNumberOfSamplesPerSurface() );
      self->calculateSampleWeights( meanShape, newSampleWeights );
      
      // Normalize so that the total is 1
      newSampleWeights /= newSampleWeights.sum();

      sampleWeights.set( newSampleWeights );
   }

   void buildShapeModes( )
   {
      const Idx M = self->getNumberOfSampledSurfaces( );

      while ( shapeModes.size() < M )
         shapeModes.push_back( MatrixType( meanShape.rows(), meanShape.cols() ) );

      if ( eigenvectors.rows() == M && eigenvectors.cols() == M )
      {
         for ( Idx m = 0; m < M; ++m )
         {
            shapeModes[m].fill( 0 );

            for ( Idx s = 0; s < M; ++s )
               shapeModes[m] += ( surfaceExes[s].sampleLocations - meanShape ) * eigenvectors( s, m );
            
            // Shrink by sqrt(M) so that weights have unit variance
            shapeModes[m] /= sqrt( M + 0.0 );

            //shapeModes[m] += meanShape;
         }
      }
   }

   void saveMesh( const char* filename )
   {
      std::ofstream os( filename );

      const Idx M = self->getNumberOfSampledSurfaces( );
      const Idx N = self->getNumberOfSamplesPerSurface( );

      os << "v " << N << std::endl;
      for ( MeshIndex v = 0; v < N; ++v )
      {
         os << meanShape(0,v) << " " << meanShape(1,v) << " " << meanShape(2,v) << std::endl;
      }

      FaceType::P faces = self->getFaces( );
      if ( *faces != 0 )
      {
         const MeshIndex nf = faces->getNumberOfPoints();
         MeshIndex vs[3];

         os << "f " << nf << std::endl;
         for ( MeshIndex f = 0; f < nf; ++f )
         {
            faces->getPoint(f, vs);
            os << vs[0] << " " << vs[1] << " " << vs[2] << std::endl;
         }
      }

      // Write the modes out
      if ( eigenvectors.rows() == M && eigenvectors.cols() == M )
      {
         for ( Idx m = 0; m < M; ++m )
         {
            os << "v " << N << std::endl;
            for ( MeshIndex v = 0; v < N; ++v )
            {
               os << shapeModes[m](0,v) << " " << shapeModes[m](1,v) << " " << shapeModes[m](2,v) << std::endl;
            }
         }
      }
   }

   void saveMatlab( const char* filename )
   {
      vnl_matlab_filewrite fw( filename );

      fw.write( meanShape, "meanshape" );

      // Write the mean shape's connectivity
      FaceType::P faces = self->getFaces( );
      if ( *faces != 0 )
      {
         const MeshIndex nf = faces->getNumberOfPoints();
         MatrixType faceMatrix( nf, 3 );

         Idx vs[3];
         for ( MeshIndex f = 0; f < nf; ++f )
         {
            faces->getPoint(f,vs);

            faceMatrix(f,0) = 1+vs[0];
            faceMatrix(f,1) = 1+vs[1];
            faceMatrix(f,2) = 1+vs[2];
         }

         fw.write( faceMatrix, "faces" );
      }

      // and the sample weights - but it's a diagonal matrix, so
      // don't need to write the whole thing
      fw.write( sampleWeights.diagonal(), "sampleWeights" );
   
      const Idx M = self->getNumberOfSampledSurfaces( );
      const Idx N = self->getNumberOfSamplesPerSurface( );

      // Copy the correspondences into a shape matrix
      MatrixType pp( 3*N, M );
      for ( Idx s = 0; s < M; ++s )
      {
         MatrixType column = surfaceExes[s].sampleLocations.extract(3,N);

         for ( Idx c = 0; c < N; ++c )
            for ( Idx ax = 0; ax < 3; ++ax )
               pp( 3*c + ax, s ) = column( ax, c );
      }
      fw.write( pp, "P" );

      fw.write( eigenvalues,  "D" );
      fw.write( eigenvectors, "V" );
   
      // Write the modes out
      if ( eigenvectors.rows() == M && eigenvectors.cols() == M )
      {
         MatrixType modes(3*N, M);

         // Write the mean shape into the first column
         for (Idx n = 0; n < N; ++n)
         {
            modes( 3*n+0, 0 ) = meanShape( 0, n );
            modes( 3*n+1, 0 ) = meanShape( 1, n );
            modes( 3*n+2, 0 ) = meanShape( 2, n );
         }

         // Write the mode shapes into the subsequent columns
         for ( Idx m = 0; m < M-1; ++m )
         {
            for (Idx n = 0; n < N; ++n)
            {
               modes( 3*n+0, m+1 ) = shapeModes[m]( 0, n );
               modes( 3*n+1, m+1 ) = shapeModes[m]( 1, n );
               modes( 3*n+2, m+1 ) = shapeModes[m]( 2, n );
            }
         }
         fw.write(modes, "modes");
      }
   }

   const Idx getM( ) const
   {
      return self->getNumberOfSampledSurfaces( );
   }

   void calculateCovarianceMatrix( )
   {
      const Idx M = getM( );
      const Idx N = self->getNumberOfSamplesPerSurface();

      updateStoredSampleLocations( );

      alignSamples( );

      Covar.set_size( M, M );
      Covar.fill( 0 );

      PRINTVBL( sampleWeights.diagonal().sum() );

      for ( Idx k = 0; k < M; ++k )
      {
         SampledSurfaceEx& ssxk          = surfaceExes[k];
         const MatrixType  centredShapeK_times_w = ( ssxk.sampleLocations - meanShape ) * sampleWeights;

         for ( Idx j = k; j < M; ++j )
         {
            SampledSurfaceEx& ssxj          = surfaceExes[j];
            const MatrixType  centredShapeJ = ssxj.sampleLocations - meanShape;

            // Take the sum of the element-wise product of the two matrices...
            // Note that the dot is only over (Dims*N) elements, since the last row
            // is necessarily zero
            Covar(k,j) = Covar(j,k) = dot<T>( centredShapeK_times_w.data_block(), centredShapeJ.data_block(), 3*N );
         }
      }
   }

   T calculateE( bool rebuildCovariance = true )
   {
      ProfTimer timer;

      const Idx M = getM( );
      
      T modelCost = 0;
      if ( M > 1 && modelWeight != 0 )
      {
         if ( rebuildCovariance )
            calculateCovarianceMatrix( );

         {
            vnl_symmetric_eigensystem<T> eig( Covar );

            eigenvalues.set_size( M );
            eigenvalues.set( eig.D.diagonal( ).data_block() );
            eigenvectors = eig.V;
         }
         
         if ( eigenvalues[0] < eigenvalues[eigenvalues.size()-1] )
         {
            // Flip so that eigenvalues are in ascending order
            eigenvalues.flip();
            eigenvectors.fliplr();
         }

         VectorType E_grad( M );
         E_grad.fill( 0 );

         modelCost = costFunction->calculate( eigenvalues, E_grad );
         
         // While we are here, calculate the gradient of E
         // wrt all of the elements of the covariance matrix
         dE_by_dAjk.set_size( M, M );
         dE_by_dAjk.fill( 0 );

         dE_by_dAjk_sums.set_size( M );
         
         for ( Idx j = 0; j < M; ++j )
            for ( Idx k = 0; k < M; ++k )
               for ( Idx i = 0; i < M; ++i )
                  dE_by_dAjk(j,k) += E_grad(i) * eigenvectors(j,i) * eigenvectors(k,i);

         dE_by_dAjk_sums.fill( 0 );
         for ( Idx k = 0; k < M; ++k )
            for ( Idx j = 0; j < M; ++j )
               dE_by_dAjk_sums(k) += dE_by_dAjk(j,k);
      }

      T regCost = 0;
      //regularizationWeight = 0;
      if ( regularizationWeight != 0 )
      {
         for ( Idx s = 0; s < M; ++s )
         {
            PRINTVBL( s );
            const T incr = surfaceExes[s].surface->regularizationCost( );
            PRINTVBL( incr );
            DEBUGMACRO( "reg cost (surface " << (*surfaceExes[s].surface) << ") = " << std::setprecision(16) << incr );
            regCost += incr;
         }
      }
      
      buildShapeModes();

      return modelWeight*modelCost - regularizationWeight*regCost;
   }

   void getTransformedJacobian( const Idx s, const Idx sampleIndex, const Idx parameterIndex, MatrixType& jac )
   {
      SampledSurfaceEx& ssx = surfaceExes[s];

      ssx.surface->jacobian( sampleIndex, parameterIndex, jac );

      // This Jacobian is how much the correspondence will move in the surface's 
      // coordinate space given a unit change to the parameters.
      
      // Transform into the Procrustes-aligned space
      jac = ssx.transform * jac;
   }

   void calculateGradients( const Idx s, T& maxAllowedT )
   {
      SampledSurfaceEx& ssx = surfaceExes[s];

      const Idx M = getM( );
      const Idx K = ssx.surface->getNumberOfParameters( );
      const Idx P = ssx.surface->getParameterDimensionality( );

      ssx.parameterUpdates.set_size( K, P );
      ssx.parameterUpdates.fill( 0 );

      if ( s != 0 || !firstShapeFixed )
      {
         TimedBlock timer( "updatecalc" );
         Idx sampleIndex, parameterIndex;

         MatrixType jac( 3+1, P );
         VectorType incr( 3+1 );

         if ( M > 1 && modelWeight > 0 )
         {
            ssx.surface->resetIterator( );
            while ( ssx.surface->next( sampleIndex, parameterIndex ) )
            {
               incr.fill( 0 );
               for ( Idx j = 0; j < M; ++j )
               {
                  const T psi = dE_by_dAjk(j,s) - dE_by_dAjk_sums(j)/M;
                  for ( Idx q = 0; q < 3+1; ++q )
                     incr(q) += psi * ( surfaceExes[j].sampleLocations( q, sampleIndex ) - meanShape(q, sampleIndex ) );
               }

               // Multiply this increment by twice the fractional area at the sample...
               // Saves repeatedly doing it
               incr *= 2 * sampleWeights( sampleIndex );

               // Get the transformed jacobian at this point - it should be non-zero
               // because otherwise there is no need for iterator to return it
               getTransformedJacobian( s, sampleIndex, parameterIndex, jac );

               for ( Idx p = 0; p < P; ++p )
                  for ( Idx q = 0; q < 3; ++q )
                     ssx.parameterUpdates( parameterIndex, p ) += incr( q ) * jac( q, p );
            }
         }

         // Now calculate the regularization term
         MatrixType reg;
         if ( regularizationWeight != 0 )
         {
            PRINTVBLNL(ssx.transform);
            ssx.surface->regularizationGradient( reg, ssx.transform );
         }
         else
         {
            reg.set_size( ssx.parameterUpdates.rows(), ssx.parameterUpdates.cols() );
            reg.fill( 0 );
         }

         // Weight the two components
         ssx.parameterUpdates = modelWeight*ssx.parameterUpdates - regularizationWeight*reg;

         // The update is calculated, but it would be useful to know how far the particles 
         // will actually move under this update
         MatrixType deltas( 3, ssx.surface->getNumberOfSamples() );
         deltas.fill( 0 );
         ssx.surface->resetIterator( );
         while ( ssx.surface->next( sampleIndex, parameterIndex ) )
         {
            getTransformedJacobian( s, sampleIndex, parameterIndex, jac );

            for ( Idx p = 0; p < P; ++p )
               for ( Idx q = 0; q < 3; ++q )
                  deltas(q,sampleIndex) += jac(q,p) * ssx.parameterUpdates(parameterIndex,p);
         }

         T maxNormSq = 0;
         Idx maxC = INVALID_INDEX;
         for ( Idx c = 0; c < deltas.cols(); ++c )
         {
            T normSq = 0;
            
            FOREACHAXIS( ax ) normSq += deltas(ax,c)*deltas(ax,c);

            if ( normSq > maxNormSq )
            {
               maxNormSq = normSq;
               maxC = c;
            }
         }
         const T scale = pow( vnl_det<T>( ssx.transform ), 1.0/3.0 );
         maxAllowedT = ssx.surface->getMaxMove( ) * scale / sqrt( maxNormSq );
      }
      else
      {
         maxAllowedT = std::numeric_limits<T>::infinity();
      }
   }

   void addSampledSurface( SampledSurface::P samp )
   {
      //DEBUGMACRO( "Creating holder" );
      surfaceExes.push_back( SampledSurfaceEx() );

      SampledSurfaceEx& ssx = surfaceExes.back();

      ssx.surface = samp;
      ssx.transform.set_identity();

      ssx.sampleLocations.set_size( 3, samp->getNumberOfSamples() );
      ssx.parameterUpdates.set_size( samp->getNumberOfParameters(), samp->getParameterDimensionality() );

      updateStoredSampleLocations( surfaceExes.back() );
      calculateMeanShape( );
      //DEBUGMACRO( "Done." );
   }

   void randomVector( VectorType& ws, const Idx toSet )
   {
      for ( Idx i = 0; i < toSet && i < ws.size(); ++i )
         ws[i] = AWT::Noise<double>::randn( 0, 1 );

      for ( Idx i = toSet; i < ws.size(); ++i )
         ws[i] = 0;
   }

   void specGen( const Idx R )
   {
      const Idx M = getM( );

      MatrixType id(M,M); id.set_identity();
      PRINTVBL( (eigenvectors * eigenvectors.transpose() - id).fro_norm() );
      PAUSE;

      // Construct the generator matrix from the eigenvectors
      MatrixType meanShift(M,M);
      for (Idx r = 0; r < M; ++r)
         for (Idx c = 0; c < M; ++c)
            meanShift(r,c) = ( ((r==c)?1.0:0.0) - 1.0/M );

      const MatrixType generator = meanShift * eigenvectors / sqrt(M+0.0);

      MatrixType samples = meanShape;
      Transformation trans; trans.set_identity();

      MatrixType closestRandomToTraining(M-1, M, std::numeric_limits<double>::infinity());

      double accum = 0;
      for ( Idx i = 0; i < R; ++i )
      {
         ProfilingTimer timer;
         timer.start();
         VectorType ws(M);

         for ( Idx m = 1; m < M-1; ++m )
         {
            randomVector(ws, m);

            // Transform to get the actual weights for the test shapes
            ws = generator*ws;

            // Now calculate the sample locations given by these weights
            samples.update( meanShape );
            for (Idx i = 0; i < ws.size(); ++i)
               samples += ws[i] * surfaceExes[i].sampleLocations;
            samples.set_row(3, 1.0);

            for ( Idx s = 0; s < M; ++s )
            {
               // Align these samples to each of the training shapes in turn
               //spairwiseAlign(surfaceExes[s].sampleLocations, samples, trans);

               // Calculate the fro norm of the difference in sample locations
               const double norm = (surfaceExes[s].sampleLocations - trans*samples).fro_norm();
            }
         }

         timer.stop();
         accum += timer.getDurationInSecs();
         
         PRINTVBL2( "eta", (R-i) * accum / i);
      }
   }

   std::vector< SampledSurfaceEx > surfaceExes;

   MeshType::P sampleMesh;

   // Holds the mean location of the samples
   MatrixType meanShape;

   std::vector< MatrixType > shapeModes;

   // Holds the fractional areas around each sample
   vnl_diag_matrix<T> sampleWeights;
   
   // Holds the covariance matrix
   MatrixType Covar;

   // Holds the eigenvalues of the covariance matrix
   VectorType eigenvalues;
   MatrixType eigenvectors;

   // Holds derivatives
   MatrixType dE_by_dAjk;

   // Holds derivatives
   VectorType dE_by_dAjk_sums;

   // This is the cost function which will be optimized
   EigenvalueCostFunction::P costFunction;

   // There is a transformation matrix associated with each of the sampled surfaces,
   // because the shapes aren't initially aligned
   //std::vector< Transformation > transforms;

   ModelEigenvalueOptimizer* self;

   bool firstShapeFixed;

   std::string savePrefix;

   ScaleNormalizationType scaleNormalization;

   T sampleConvergenceThreshold;

   T modelWeight;
   T regularizationWeight;

   RefinementStrategy::P refinementStrategy;

   bool rotationAllowed;
   bool translationAllowed;
};


AWT::AlignParametric::ModelEigenvalueOptimizer::ModelEigenvalueOptimizer( )
{
   m_D = new D;
   m_D->self = this;

   m_D->sampleConvergenceThreshold = 5e-3;

   m_D->modelWeight = 1;
   m_D->regularizationWeight = 1;

   m_D->scaleNormalization = ModelEigenvalueOptimizer::NRML_HYPERSPHERE;

   m_D->firstShapeFixed = true;
   m_D->savePrefix = getTimestamp();

   m_D->rotationAllowed = false;
   m_D->translationAllowed = false;
}


AWT::AlignParametric::ModelEigenvalueOptimizer::~ModelEigenvalueOptimizer( )
{
   delete m_D;
}

GETNAMEMACRO( AWT::AlignParametric::ModelEigenvalueOptimizer );

EigenvalueCostFunction::P AWT::AlignParametric::ModelEigenvalueOptimizer::getCostFunction( )
{
   return m_D->costFunction;
}


void AWT::AlignParametric::ModelEigenvalueOptimizer::setCostFunction( EigenvalueCostFunction::P func )
{
   m_D->costFunction = func;
}


Idx AWT::AlignParametric::ModelEigenvalueOptimizer::getNumberOfSampledSurfaces( ) const
{
   return static_cast<Idx>( m_D->surfaceExes.size() );
}


AWT::AlignParametric::SampledSurface::P AWT::AlignParametric::ModelEigenvalueOptimizer::getSampledSurface( const Idx i )
{
   return m_D->surfaceExes[i].surface;
}

void AWT::AlignParametric::ModelEigenvalueOptimizer::_add(AWT::AlignParametric::SampledSurface::P surf)
{
   m_D->addSampledSurface( surf );
   modified();
}

Idx AWT::AlignParametric::ModelEigenvalueOptimizer::getNumberOfSamplesPerSurface( ) const
{
   return m_D->surfaceExes[0].surface->getNumberOfSamples();
}
T AWT::AlignParametric::ModelEigenvalueOptimizer::calculateNow( )
{
   const T ret = m_D->calculateE( );
   modified();

   return ret;
}

void AWT::AlignParametric::ModelEigenvalueOptimizer::saveModel( const std::string& filename )
{
   m_D->saveMesh( filename.c_str() );
   DEBUGMACRO( "Mesh written to " << filename );
}

void AWT::AlignParametric::ModelEigenvalueOptimizer::saveMatlab( const std::string& filename )
{
   m_D->saveMatlab( filename.c_str() );
   DEBUGMACRO( "MATLAB data written to " << filename );
}

T AWT::AlignParametric::ModelEigenvalueOptimizer::calculateGradients( )
{
   T maxAllowedT = std::numeric_limits<T>::infinity();

   DEBUGMACRO( "Calculating updates..." );
   for ( Idx i = 0; i < getNumberOfSampledSurfaces( ); ++i )
   {
      T allowedT;

      PRINTVBL( i );
      m_D->calculateGradients( i, allowedT );
      
      maxAllowedT = std::min<T>( maxAllowedT, allowedT );
   }
   modified();

   //maxAllowedT = 1e-1;
   DEBUGMACRO( "The maximum recommended time step is " << maxAllowedT );

   return maxAllowedT;
}

const MatrixType& AWT::AlignParametric::ModelEigenvalueOptimizer::getSampledSurfaceUpdate( const Idx i ) const
{
   return m_D->surfaceExes[i].parameterUpdates;
}


void AWT::AlignParametric::ModelEigenvalueOptimizer::applyUpdates( const T deltaT )
{
   Idx minNParams = std::numeric_limits<Idx>::max();
   Idx maxNParams = std::numeric_limits<Idx>::min();

   const Idx N = getNumberOfSamplesPerSurface();

   bool* converged = new bool[ N ];
   for ( Idx i = 0; i < N; ++i )
      converged[i] = true;

   const T threshSq = m_D->sampleConvergenceThreshold * m_D->sampleConvergenceThreshold;

   T minVel = std::numeric_limits<T>::infinity();
   T maxVel = 0;

   for ( Idx s = 0; s < getNumberOfSampledSurfaces( ); ++s )
   {
      if ( s == 0 && m_D->firstShapeFixed )
      {
         DEBUGMACRO( "Holding first shape fixed" );
         continue;
      }

      D::SampledSurfaceEx& ssx = m_D->surfaceExes[s];
      MatrixType pointDelta = ssx.sampleLocations;

      MatrixType controlValuesNow;
      ssx.surface->getParameters( controlValuesNow );

      minNParams = std::min<Idx>( minNParams, controlValuesNow.rows() );
      maxNParams = std::max<Idx>( maxNParams, controlValuesNow.rows() );

      controlValuesNow -= deltaT*ssx.parameterUpdates;
      
      // Record the current positions of the points so that we can compare afterwards
   
      {
         //m_D->writeSampleLocations( "Before set", __LINE__ );
         //TimedBlock timer( "Set new control values" );
         ssx.surface->setParameters( controlValuesNow );
      }
      
      //m_D->writeSampleLocations( "Before update", __LINE__ );
      m_D->updateStoredSampleLocations( ssx );
      //m_D->writeSampleLocations( "After update", __LINE__ );

      pointDelta -= ssx.sampleLocations;
      pointDelta /= deltaT;

      for ( Idx i = 0; i < N; ++i )
      {
         if ( converged[i] )
         {
            T deltaSq = 0;
            FOREACHAXIS( ax )
               deltaSq += pointDelta(ax,i) * pointDelta(ax,i);

            if ( deltaSq > threshSq )
               converged[i] = false;

            minVel = std::min<T>( minVel, deltaSq );
            maxVel = std::max<T>( maxVel, deltaSq );
         }
      }
   }

   minVel = sqrt( minVel );
   maxVel = sqrt( maxVel );

   DEBUGMACRO( minVel << "\t" << maxVel );
   PRINTVBL( deltaT );

DEBUGLINE;
   m_D->calculateMeanShape( );

   // Determine which particles have converged, and then pass these off to the refinement
   // managers
   std::vector<Idx> convergedParticles;
   for ( Idx i = 0; i < N; ++i )
      if ( converged[i] ) convergedParticles.push_back( i );
   delete [] converged;

   BLANKLINE;
   DEBUGMACRO( "CHECKING FOR REF STRAT" );
   if ( *m_D->refinementStrategy )
   {
      {
         ColouredConsole cons(ColouredConsole::COL_MAGENTA|ColouredConsole::COL_BRIGHT);
         PRINTVBL( convergedParticles.size() );
      }

      AWT::Container::Iterator<Idx>::P iter = VectorIterator<Idx>::getInstance( convergedParticles );
      if ( m_D->refinementStrategy->refine( iter ) )
      {
         // In case something has changed
         m_D->calculateMeanShape( );
         m_D->updateStoredSampleLocations( );
      }
   }

   modified();
}


void AWT::AlignParametric::ModelEigenvalueOptimizer::debugGradients( ModelEigenvalueOptimizer::P so )
{
   BLANKLINE;
   DEBUGMACRO( "Debugging the gradient..." );

   const T delta = 1e-6;
   for ( Idx s = 0; s < so->getNumberOfSampledSurfaces(); ++s )
   {
      SampledSurface::P surf = so->getSampledSurface( s );

      if ( s == 0 && so->isFirstShapeFixed() )
      {
         ColouredConsole cons( 12 );
         DEBUGMACRO( "First shape is fixed; skipping" );
         continue;
      }

      MatrixType cv;
      const T val = so->calculateNow();
      so->calculateGradients();
      surf->getParameters( cv );

      PRINTVBL( val );

      const Idx N = cv.rows();
      const Idx C = cv.columns();

      MatrixType numGrad( N, C );
      const MatrixType calcGrad = so->getSampledSurfaceUpdate( s );
      PRINTVBL( calcGrad.rows() );
      PRINTVBL( calcGrad.cols() );

      for ( Idx i = 0; i < N; ++i )
      {
         for ( Idx j = 0; j < C; ++j )
         {
            {
               ColouredConsole cons( ColouredConsole::COL_CYAN | ColouredConsole::COL_BRIGHT );
               //DEBUGMACRO( "Surface " << *surf << "\t" << i << "\t" << j );

               surf->setParameters( cv );
               const T v = so->m_D->calculateE( false );
               //PRINTVBL( v );
            }

            // Reset

            MatrixType cvp = cv; cvp( i, j ) += delta;
            MatrixType cvm = cv; cvm( i, j ) -= delta;

            surf->setParameters( cvp );
            //const T vp = so->calculateNow( );
            const T vp = so->m_D->calculateE( false );

            surf->setParameters( cvm );
            //const T vm = so->calculateNow( );
            const T vm = so->m_D->calculateE( false );

            // Reset
            surf->setParameters( cv );

            numGrad( i, j ) = ( vp - vm ) / ( 2*delta );
         }
      }

      {
         ColouredConsole cons( 12 );
         PRINTVBLNL( numGrad );
         PRINTVBLNL( calcGrad );
      }
   }
   PAUSE;
}


bool AWT::AlignParametric::ModelEigenvalueOptimizer::isFirstShapeFixed( ) const
{
   return m_D->firstShapeFixed;
}


void AWT::AlignParametric::ModelEigenvalueOptimizer::setFirstShapeFixed( const bool v )
{
   if ( m_D->firstShapeFixed != v )
   {
      m_D->firstShapeFixed = v;
      modified( );
   }
}

const VectorType& AWT::AlignParametric::ModelEigenvalueOptimizer::getEigenvalues( ) const
{
   return m_D->eigenvalues;
}


TuplesType::P AWT::AlignParametric::ModelEigenvalueOptimizer::getMeanSamples( ) const
{
   return VNLTuplesType::getInstance( m_D->meanShape, true );
}


TuplesType::P AWT::AlignParametric::ModelEigenvalueOptimizer::getMode( const Idx m ) const
{
   return VNLTuplesType::getInstance( m_D->shapeModes[m], true );
}


TuplesType::P AWT::AlignParametric::ModelEigenvalueOptimizer::getSamples( const Idx s ) const
{
   return VNLTuplesType::getInstance( m_D->surfaceExes[s].sampleLocations, true );
}


Transformation AWT::AlignParametric::ModelEigenvalueOptimizer::getTransformation( const Idx s ) const
{
   return m_D->surfaceExes[s].transform;
}


AWT::AlignParametric::ModelEigenvalueOptimizer::ScaleNormalizationType AWT::AlignParametric::ModelEigenvalueOptimizer::getScaleNormalization( ) const
{
   return m_D->scaleNormalization;
}


void ModelEigenvalueOptimizer::setScaleNormalization( const ModelEigenvalueOptimizer::ScaleNormalizationType v )
{
   if ( v != m_D->scaleNormalization )
   {
      m_D->scaleNormalization = v;
      modified();
   }
}


void AWT::AlignParametric::ModelEigenvalueOptimizer::setSampleConvergenceThreshold( const T thresh )
{
   if ( thresh != m_D->sampleConvergenceThreshold )
   {
      if ( thresh < 0 )
         throw "Sample convergence threshold cannot be negative";

      m_D->sampleConvergenceThreshold = thresh;
      modified();
   }
}


T AWT::AlignParametric::ModelEigenvalueOptimizer::getSampleConvergenceThreshold( ) const
{
   return m_D->sampleConvergenceThreshold;
}

RefinementStrategy::P AWT::AlignParametric::ModelEigenvalueOptimizer::getRefinementStrategy( ) const
{
   return m_D->refinementStrategy;
}


void ModelEigenvalueOptimizer::setRefinementStrategy( RefinementStrategy::P v )
{
   if ( v != m_D->refinementStrategy )
   {
      m_D->refinementStrategy = v;
      modified();
   }
}


T AWT::AlignParametric::ModelEigenvalueOptimizer::getModelWeight( ) const
{
   return m_D->modelWeight;
}


void AWT::AlignParametric::ModelEigenvalueOptimizer::setModelWeight( const T v )
{
   if ( v != m_D->modelWeight )
   {
      m_D->modelWeight = v;
      modified();
   }
}


T AWT::AlignParametric::ModelEigenvalueOptimizer::getRegularizationWeight( ) const
{
   return m_D->regularizationWeight;
}


void AWT::AlignParametric::ModelEigenvalueOptimizer::setRegularizationWeight( const T v )
{
   if ( v != m_D->regularizationWeight )
   {
      m_D->regularizationWeight = v;
      modified();
   }
}


bool AWT::AlignParametric::ModelEigenvalueOptimizer::isRotationAllowed( ) const
{
   return m_D->rotationAllowed;
}


void AWT::AlignParametric::ModelEigenvalueOptimizer::setRotationAllowed( const bool v )
{
   if ( v != m_D->rotationAllowed )
   {
      m_D->rotationAllowed = v;
      modified();
   }
}

bool AWT::AlignParametric::ModelEigenvalueOptimizer::isTranslationAllowed( ) const
{
   return m_D->translationAllowed;
}


void AWT::AlignParametric::ModelEigenvalueOptimizer::setTranslationAllowed( const bool v )
{
   if ( v != m_D->translationAllowed )
   {
      m_D->translationAllowed = v;
      modified();
   }
}

void AWT::AlignParametric::ModelEigenvalueOptimizer::calculateMeasures( )
{
   TimedBlock("calculateMeasures");
   m_D->specGen( 10000 );
}