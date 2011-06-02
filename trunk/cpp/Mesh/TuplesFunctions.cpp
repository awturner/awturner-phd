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
#include "TuplesFunctions.h"

#include "vnl/algo/vnl_svd.h"
#include "vnl/vnl_matrix_fixed.h"
#include "vnl/vnl_vector_fixed.h"
#include "vnl/vnl_det.h"

#include "TuplesImpl.h"

#include "Useful/PrintMacros.h"
#include "Useful/ArrayVectorFunctions.h"

template <class T>
AWT::TuplesFunctions<T>::TuplesFunctions( )
{
}

template <class T>
AWT::TuplesFunctions<T>::~TuplesFunctions( )
{
}

template <class T>
void AWT::TuplesFunctions<T>::transformPoints( typename Tuples<T>::P pts, const vnl_matrix_fixed<T,4,4> r, const bool asDirections = false )
{
   const MeshIndex N = pts->getNumberOfPoints( );

   vnl_vector_fixed<T,4> vtx;

   for ( MeshIndex v = 0; v < N; ++v )
   {
      pts->getPoint( v, vtx.data_block() );
      vtx(3) = static_cast<T>( asDirections ? 0 : 1 );

      vtx = r * vtx;
      pts->setPoint( v, vtx.data_block( ) );
   }
}

template <class T>
void AWT::TuplesFunctions<T>::transformPoints( vnl_vector<T>& pts, const vnl_matrix_fixed<T,4,4> r, const bool asDirections = false )
{
   const MeshIndex N = pts.size( ) / 3;

   vnl_vector_fixed<T,4> vtx;

   for ( MeshIndex v = 0; v < N; ++v )
   {
      for ( MeshIndex ax = 0; ax < 3; ++ax )
         vtx(ax) = pts( 3*v + ax );
      vtx(3) = static_cast<T>( asDirections ? 0 : 1 );

      vtx = r * vtx;

      for ( MeshIndex ax = 0; ax < 3; ++ax )
         pts( 3*v + ax ) = vtx(ax);
   }
}

template <class T>
typename AWT::Tuples<T>::P AWT::TuplesFunctions<T>::transform( typename Tuples<T>::P pts, const vnl_matrix<T> R )
{
   const MeshIndex N = pts->getNumberOfPoints( );
   
   Tuples<T>::P ret = TuplesImpl<T>::getInstance( R.rows(), N );

   vnl_vector<T> pnt( pts->getTupleSize( ) );
   vnl_vector<T> tra( R.rows() );
   
   for ( MeshIndex v = 0; v < N; ++v )
   {
      pts->getPoint( v, pnt.data_block() );
      tra = R * pnt;
      ret->setPoint( v, tra.data_block( ) );
   }
   
   return ret;
}

template <class T>
T AWT::TuplesFunctions<T>::calculateRms( const typename Tuples<T>::P ptsX, const typename Tuples<T>::P ptsY )
{
   T rss = calculateRss( ptsX, ptsY );

   return sqrt( rss / ptsX->getNumberOfPoints( ) );
}

template <class T>
T AWT::TuplesFunctions<T>::calculateRss( const typename Tuples<T>::P ptsX, const typename Tuples<T>::P ptsY )
{
   vnl_vector<T> x( ptsX->getTupleSize( ) );
   vnl_vector<T> y( ptsY->getTupleSize( ) );

   T ret = 0;

   const MeshIndex N = ptsX->getNumberOfPoints( );

   for ( MeshIndex i = 0; i < N; ++i )
   {
      ptsX->getPoint( i, x.data_block( ) );
      ptsY->getPoint( i, y.data_block( ) );

      ret += ( x - y ).squared_magnitude( );
   }

   return ret;
}

template <class T>
T AWT::TuplesFunctions<T>::calculateRmsOrtho( const typename Tuples<T>::P ptsX, const typename Tuples<T>::P ptsY, const typename Tuples<T>::P normals, 
                                              const T sigmaParallel, const T sigmaPerp,
                                              T& maxParallel, T& maxPerp  )
{
   T rss = calculateRssOrtho( ptsX, ptsY, normals, sigmaParallel, sigmaPerp, maxParallel, maxPerp );

   return sqrt( rss / ptsX->getNumberOfPoints( ) );
}

template <class T>
T AWT::TuplesFunctions<T>::calculateRssOrtho( const typename Tuples<T>::P ptsX, const typename Tuples<T>::P ptsY, const typename Tuples<T>::P normals, 
                                              const T sigmaParallel, const T sigmaPerp,
                                              T& maxParallel, T& maxPerp  )
{
   vnl_vector<T> x( ptsX->getTupleSize( ) );
   vnl_vector<T> y( ptsY->getTupleSize( ) );
   vnl_vector<T> nml( ptsX->getTupleSize( ) );

   // Use these to record the maximum gaps
   maxParallel = maxPerp = 0;

   T ret = 0;
   const MeshIndex N = ptsX->getNumberOfPoints( );

   vnl_vector<T> delta( ptsX->getTupleSize( ) );
   for ( MeshIndex i = 0; i < N; ++i )
   {
      ptsX->getPoint( i, x.data_block( ) );
      ptsY->getPoint( i, y.data_block( ) );

      normals->getPoint( i, nml.data_block( ) );

      // The difference between the points
      delta = x - y;

      // This is the component of delta parallel to the normal
      const T lambda = dot_product<T>( delta, nml );

      // Parallel and perpendicular distances squared
      const T parallel2  = ( lambda * lambda ) / ( sigmaParallel * sigmaParallel );
      const T perp2      = ( delta - lambda*nml ).squared_magnitude( ) / ( sigmaPerp * sigmaPerp );

      if ( perp2     > maxPerp     ) maxPerp     = perp2;
      if ( parallel2 > maxParallel ) maxParallel = parallel2;

      ret += perp2 + parallel2;
   }

   // Calculate the distances in the perpendicular and parallel directions
   maxPerp     = sqrt( maxPerp     ) * sigmaPerp;
   maxParallel = sqrt( maxParallel ) * sigmaParallel;

   return ret;
}

template <class T>
vnl_matrix_fixed<T,4,4> AWT::TuplesFunctions<T>::absoluteOrientation( const vnl_vector<T> ptsX, const typename vnl_vector<T> ptsY, const bool scale )
{
   vnl_vector<T> x( 3 );
   vnl_vector<T> y( 3 );

   vnl_matrix_fixed<T,3,3> sum_xy;
   vnl_vector<T> sum_x( 3, 1 );
   vnl_vector<T> sum_y( 3, 1 );

   T xtx = 0;
   T yty = 0;

   sum_xy.fill( 0 );
   sum_x.fill( 0 );
   sum_y.fill( 0 );

   const MeshIndex N = ptsX.size( ) / 3;

   for ( MeshIndex i = 0; i < N; ++i )
   {
      for ( MeshIndex ax = 0; ax < 3; ++ax )
      {
         x( ax ) = ptsX( 3*i + ax );
         y( ax ) = ptsY( 3*i + ax );
      }

      xtx += x.squared_magnitude();
      yty += y.squared_magnitude();

      sum_x += x;
      sum_y += y;

      // Outer product
      for ( unsigned int r = 0; r < 3; ++r )
         for ( unsigned int c = 0; c < 3; ++c )
            sum_xy(r,c) += x(c)*y(r);
   }

   return doAbsoluteOrientation( sum_xy, sum_x, sum_y, xtx, yty, N, scale );
}

template <class T>
vnl_matrix_fixed<T,4,4> AWT::TuplesFunctions<T>::absoluteOrientation( const typename Tuples<T>::P ptsX, const typename Tuples<T>::P ptsY, const bool scale )
{
   vnl_vector<T> x( 3 );
   vnl_vector<T> y( 3 );

   vnl_matrix_fixed<T,3,3> sum_xy;
   vnl_vector<T> sum_x( 3, 1 );
   vnl_vector<T> sum_y( 3, 1 );

   T xtx = 0;
   T yty = 0;

   sum_xy.fill( 0 );
   sum_x.fill( 0 );
   sum_y.fill( 0 );

   const MeshIndex N = ptsX->getNumberOfPoints( );

   for ( MeshIndex i = 0; i < N; ++i )
   {
      ptsX->getPoint( i, x.data_block( ) );
      ptsY->getPoint( i, y.data_block( ) );

      xtx += x.squared_magnitude();
      yty += y.squared_magnitude();

      sum_x += x;
      sum_y += y;

      // Outer product
      for ( unsigned int r = 0; r < 3; ++r )
         for ( unsigned int c = 0; c < 3; ++c )
            sum_xy(r,c) += x(c)*y(r);
   }

   return doAbsoluteOrientation( sum_xy, sum_x, sum_y, xtx, yty, N, scale );
}

template <class T>
vnl_matrix_fixed<T,4,4> AWT::TuplesFunctions<T>::doAbsoluteOrientation(const vnl_matrix_fixed<T,3,3> &sum_xy, const vnl_vector<T> &sum_x, 
                                                                       const vnl_vector<T> &sum_y, const T xtx, const T yty, const MeshIndex N, const T doScale)
{
   vnl_vector<T> mean_x = sum_x / static_cast<T>( N );
   vnl_vector<T> mean_y = sum_y / static_cast<T>( N );

   // Calculate the mean-shifted covariance
   vnl_matrix_fixed<T,3,3> covar = sum_xy;
   for ( unsigned int r = 0; r < 3; ++r )
      for ( unsigned int c = 0; c < 3; ++c )
         covar(r,c) -= sum_x(c)*sum_y(r) / N;

   covar /= static_cast<T>( N - 1 );

   vnl_svd<T> svd( covar );

   vnl_matrix<T> U  = svd.U();
   vnl_matrix<T> Vt = svd.V().transpose();

   vnl_matrix_fixed<T,3,3> R;
   
   if ( svd.determinant_magnitude( ) < 1e-9 )
   {
      R.set_identity( );
   }
   else
   {
      R = U*Vt;

      if ( vnl_det<T>( R ) == -1 )
      {
         Vt.scale_row(2,-1);
         R = U*Vt;
      }
   }

   if ( doScale )
   {
      //DEBUGLINE;
      const T sigma_x = sqrt( ( xtx - sum_x.squared_magnitude() / N ) / N );
      const T sigma_y = sqrt( ( yty - sum_y.squared_magnitude() / N ) / N );

      R *= sigma_y / sigma_x;
   }
   
   vnl_vector<T> mean_xx(4);
   vnl_vector<T> mean_yy(4);

   for ( int i = 0; i < 3; ++i )
   {
      mean_xx(i) = mean_x(i);
      mean_yy(i) = mean_y(i);
   }
   mean_xx(3) = mean_yy(3) = 1;

   vnl_vector<T> t = mean_y - R*mean_x;

   vnl_matrix_fixed<T,4,4> ret;
   ret.fill( 0 );

   for ( unsigned int r = 0; r < 3; ++r )
   {
      for ( unsigned int c = 0; c < 3; ++c )
         ret(r,c) = R(r,c);

      ret(r,3) = t(r);
   }
   ret(3,3) = 1;

   //PRINTVBL( ret );
   
   return ret;
}

template <class T>
vnl_matrix_fixed<T,4,4> AWT::TuplesFunctions<T>::rotationTranslationAboutPoint( const T axis[3], const T theta, const T centre[3], const T translate[3] )
{
   vnl_matrix_fixed<T,4,4> R;
   R.fill( 0 );

   T axisRot[3] = { axis[0], axis[1], axis[2] };
   normalize( axisRot, 3 );

   const T c   = cos(theta); 
   const T s   = sin(theta); 
   const T C   = 1-c;
   const T xs  = axisRot[0]*s;   
   const T ys  = axisRot[1]*s;   
   const T zs  = axisRot[2]*s;
   const T xC  = axisRot[0]*C;   
   const T yC  = axisRot[1]*C;   
   const T zC  = axisRot[2]*C;
   const T xyC = axisRot[0]*yC; 
   const T yzC = axisRot[1]*zC; 
   const T zxC = axisRot[2]*xC;

   R(0,0) = axisRot[0]*xC+c;
   R(0,1) = xyC-zs;
   R(0,2) = zxC+ys;

   R(1,0) = xyC+zs;
   R(1,1) = axisRot[1]*yC+c;
   R(1,2) = yzC-xs;

   R(2,0) = zxC-ys;
   R(2,1) = yzC+xs;
   R(2,2) = axisRot[2]*zC+c;

   R(3,3) = 1;
   
   // Rotate around the centroid
   for ( int ax = 0; ax < 3; ++ax )
      R(ax,3) = translate[ax] + centre[ax] - R(ax,0)*centre[0] - R(ax,1)*centre[1] - R(ax,2)*centre[2];

   return R;
}

template <class T>
void AWT::TuplesFunctions<T>::fill( typename Tuples<T>::P self, const T scalar )
{
   T* vtxSelf  = new T[self->getTupleSize()];
   for ( MeshIndex ax = 0; ax < self->getTupleSize(); ++ax )
      vtxSelf[ax] = scalar;

   for ( MeshIndex i = 0, imax = self->getNumberOfPoints( ); i < imax; ++i )
      self->setPoint( i, vtxSelf );

   delete [] vtxSelf;
}

template <class T>
void AWT::TuplesFunctions<T>::calculateBounds( const typename AWT::Tuples<T>::P tuples, double* out_Bounds )
{
   const MeshIndex N = tuples->getTupleSize();

   T* vtx = new T[ N ];
   
   tuples->getPoint( 0, vtx );
   for ( MeshIndex ax = 0; ax < N; ++ax )
      out_Bounds[2*ax+0] = out_Bounds[2*ax+1] = vtx[ax];
   
   for ( MeshIndex v = 0, vmax = tuples->getNumberOfPoints( ); v < vmax; ++v )
   {
      tuples->getPoint( v, vtx );   

      for ( MeshIndex ax = 0; ax < N; ++ax )
      {
         out_Bounds[2*ax+0] = std::min<double>( out_Bounds[2*ax+0], vtx[ax] );
         out_Bounds[2*ax+1] = std::max<double>( out_Bounds[2*ax+1], vtx[ax] );
      }
   }

   delete [] vtx;
}

template <class T>
void AWT::TuplesFunctions<T>::expandBounds( const typename AWT::Tuples<T>::P mesh, double* out_Bounds )
{
   double bounds[6];
   calculateBounds( mesh, bounds );
   expandBounds( bounds, out_Bounds );
}

template <class T>
void AWT::TuplesFunctions<T>::expandBounds( const double* bounds, double* out_Bounds )
{
   for ( int ax = 0; ax < 3; ++ax )
   {
      out_Bounds[2*ax+0] = std::min<double>( out_Bounds[2*ax+0], bounds[2*ax+0] );
      out_Bounds[2*ax+1] = std::max<double>( out_Bounds[2*ax+1], bounds[2*ax+1] );
   }
}

template <class T>
void AWT::TuplesFunctions<T>::copyToVNLVector( const typename Tuples<T>::P pts, vnl_vector<T>& mat, const MeshIndex element )
{
   const MeshIndex N = pts->getNumberOfPoints();

   if ( mat.size() != N )
      mat.set_size( N );

   for ( MeshIndex i = 0; i < N; ++i )
      mat(i) = pts->getPointElement( i, element );
}

template <class T>
void AWT::TuplesFunctions<T>::copyToVNLMatrix( const typename Tuples<T>::P pts, vnl_matrix<T>& mat, const bool transpose )
{
   const MeshIndex N = pts->getNumberOfPoints();
   const MeshIndex M = pts->getTupleSize();

   if ( !transpose )
   {
      if ( mat.rows() != M || mat.cols() != N )
         mat.set_size( M, N );
   }
   else
   {
      if ( mat.cols() != M || mat.rows() != N )
         mat.set_size( N, M );
   }

   T* vtx = new T[M];

   for ( MeshIndex i = 0; i < N; ++i )
   {
      pts->getPoint( i, vtx );

      if ( !transpose )
         mat.set_column( i, vtx );
      else
         mat.set_row( i, vtx );
   }

   delete [] vtx;
}

template class AWT::TuplesFunctions<double>;
template class AWT::TuplesFunctions<float>;