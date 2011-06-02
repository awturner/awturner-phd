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
#include "Pose.h"

#include "vnl/algo/vnl_svd.h"
#include "vnl/vnl_vector_fixed.h"

#include "Useful/Exception.h"

#include "Maths/Constants.h"

#include <cmath>

template <class T>
struct AWT::Pose<T>::D
{
   D( )
   {
      identity( m_Matrix );
   }

   static void identity( PoseMatrix& mat )
   {
      mat.set_identity( );
   }

   static void mult4x4( const T* a, const T* b, T* m )
   {
      for ( int r = 0; r < 4; ++r )
         for ( int c = 0; c < 4; ++c )
         {
            m[4*c + r] = 0;

            for ( int i = 0; i < 4; ++i )
               m[4*c + r] += a[4*i + r] * b[4*c + i];
         }
   }

   static void transpose( PoseMatrix& mat )
   {
      mat = mat.transpose( );
   }

   static void swap( T& a, T& b )
   {
      T tmp = a;
      a = b;
      b = tmp;
   }

   PoseMatrix m_Matrix;
};

template <class T>
AWT::Pose<T>::Pose( )
{
   m_D = new D;
}

template <class T>
AWT::Pose<T>::~Pose( )
{
   delete m_D;
}

template <class T>
typename AWT::Pose<T>::P AWT::Pose<T>::getInstance( )
{
   AUTOGETINSTANCE( AWT::Pose<T>, ( ) );
}

template <class T>
GETNAMEMACRO( AWT::Pose<T> );

template <class T>
typename AWT::Pose<T>::PoseMatrix AWT::Pose<T>::getMatrix( ) const
{
   return m_D->m_Matrix;
}

template <class T>
void AWT::Pose<T>::setMatrix( const PoseMatrix& matrix )
{
   m_D->m_Matrix = matrix;
   modified( );
}

template <class T>
void AWT::Pose<T>::project( const T* in_Point, T* out_Point, const T implicit ) const
{
   vnl_vector_fixed<T,4> pnt;

   for ( unsigned int ax = 0; ax < 3; ++ax )
      pnt[ax] = in_Point[ax];
   pnt[3] = implicit;

   vnl_vector_fixed<T,4> proj = m_D->m_Matrix * pnt;

   if ( proj[3] != 0 )
   {
      for ( unsigned int ax = 0; ax < 4; ++ax )
         proj[ax] /= proj[3];
   }

   for ( unsigned int ax = 0; ax < 3; ++ax )
      out_Point[ax] = proj[ax];
}

template <class T>
typename AWT::Pose<T>::P AWT::Pose<T>::getInversePose( ) const
{
   typename Pose<T>::P ident = getInstance( );
   typename Pose<T>::P ret   = getRelativePose( ident );

   return ret;
}

template <class T>
typename AWT::Pose<T>::P AWT::Pose<T>::getRelativePose( const typename Pose<T>::P pose ) const
{
   PoseMatrix iMat = vnl_svd<T>( m_D->m_Matrix ).inverse( );

   PoseMatrix rel = pose->getMatrix( ) * iMat;

   rel /= rel( 3, 3 );

   Pose<T>::P ret = getInstance( );

   ret->setMatrix( rel );

   return ret;
}

template <class T>
vnl_matrix<T> operator*( const AWT::Pose<T>& pose, const vnl_matrix<T>& points )
{
   vnl_matrix<T> ret( 3, points.cols( ) );

   T vtx[3];
   T proj[3];

   for ( unsigned int c = 0; c < points.cols( ); ++c )
   {
      for ( unsigned int ax = 0; ax < 3; ++ax )
         vtx[ax] = points( ax, c );

      pose.project( vtx, proj );

      for ( unsigned int ax = 0; ax < 3; ++ax )
         ret[ax][c] = proj[ax];
   }

   return ret;
}

template <class T>
void AWT::Pose<T>::identity( )
{
   D::identity( m_D->m_Matrix );

   modified( );
}

template <class T>
void AWT::Pose<T>::translate( const T* trans )
{
   m_D->m_Matrix( 0, 3 ) += trans[0];
   m_D->m_Matrix( 1, 3 ) += trans[1];
   m_D->m_Matrix( 2, 3 ) += trans[2];

   modified( );
}

template <class T>
void AWT::Pose<T>::rotateDeg( const T theta, const T* axis )
{
   rotate( theta * AWT::Constants<T>::DegreesToRadians, axis );
}

template <class T>
void AWT::Pose<T>::rotateDeg( const T theta, const T* axis, const T* centre )
{
   rotate( theta * AWT::Constants<T>::DegreesToRadians, axis, centre );
}

template <class T>
void AWT::Pose<T>::rotate( const T theta, const T* axis )
{
   static const T centre[] = { 0, 0, 0 };
   rotate( theta, axis, centre );
}

template <class T>
void AWT::Pose<T>::rotate( const T theta, const T* axis, const T* centre )
{
   // http://local.wasp.uwa.edu.au/~pbourke/geometry/rotate/

   PoseMatrix overall;
   overall.set_identity( );

   PoseMatrix trans;
   T normAxis[3];

   T axisLength = sqrt( axis[0]*axis[0] + axis[1]*axis[1] + axis[2]*axis[2] );

   // Step 1 - translate so that rotation axis goes through origin
   trans.set_identity( );
   for ( int i = 0; i < 3; ++i )
   {
      trans( i, 3 ) -= centre[i];
      normAxis[i] = axis[i] / axisLength;
   }

   // Step 2 - Rotate space about the x axis so that the rotation axis lies in the xz plane
   PoseMatrix rx; rx.set_identity( );
   T d = sqrt( normAxis[1]*normAxis[1] + normAxis[2]*normAxis[2] );
   if ( d != 0 )
   {
      rx( 1, 1 ) =  normAxis[2] / d;
      rx( 2, 1 ) =  normAxis[1] / d;

      rx( 1, 2 ) = -normAxis[1] / d;
      rx( 2, 2 ) =  normAxis[2] / d;
   }

   // Step 3 - Rotate space about the y axis so that the rotation axis lies along the positive z axis
   PoseMatrix ry; ry.set_identity( );

   ry( 0, 0 ) =  d;
   ry( 2, 0 ) =  normAxis[0];

   ry( 0, 2 ) = -normAxis[0];
   ry( 2, 2 ) =  d;

   // Step 4 - Rotation about the z axis by t (theta)
   PoseMatrix rz; rz.set_identity( );
   rz( 0, 0 ) =  rz( 1, 1 ) = cos( theta );
   rz( 1, 0 ) =  rz( 0, 1 ) = sin( theta );
   rz( 1, 0 ) = -rz( 1, 0 );

   overall = trans.transpose( ) * rx.transpose( ) * ry.transpose( ) * rz * ry * rx * trans;

   m_D->m_Matrix = overall * m_D->m_Matrix;

   modified( );
}

template <class T>
void AWT::Pose<T>::scale( const T sc )
{
   PoseMatrix overall;
   overall.set_identity( );

   for ( int i = 0; i < 3; ++i )
      overall(i,i) = sc;

   m_D->m_Matrix = overall * m_D->m_Matrix;

   modified( );
}

template class AWT::Pose<double>;
template class AWT::Pose<float>;

template vnl_matrix<double> operator*( const AWT::Pose<double>& pose, const vnl_matrix<double>& points );
template vnl_matrix<float> operator*( const AWT::Pose<float>& pose, const vnl_matrix<float>& points );