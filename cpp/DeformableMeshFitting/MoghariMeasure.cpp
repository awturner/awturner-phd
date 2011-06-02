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
#include "MoghariMeasure.h"

#include "Mesh/TuplesImpl.h"
#include "Mesh/Mesh.h"

#include "MeshSearching/VerticesNearestVerticesSearch.h"

#include "vnl/algo/vnl_determinant.h"
#include "vnl/algo/vnl_symmetric_eigensystem.h"
#include "vnl/algo/vnl_determinant.h"
#include "vnl/algo/vnl_svd.h"
#include "vnl/vnl_matrix.h"

template <class T>
struct AWT::MoghariMeasure<T>::D
{
   vnl_matrix_fixed<T,3,3> calculateEigenvectors( const vnl_matrix_fixed<T,3,3>& m )
   {
      vnl_symmetric_eigensystem<T> eig( m );

      vnl_matrix_fixed<T,3,3> eigenvectors = eig.V;

      // Make sure that we've got a RH coordinate system
      if ( vnl_determinant<T>( eigenvectors ) < 0 )
      {
         for ( unsigned int r = 0; r < 3; ++r )
            eigenvectors[r][2] *= -1;
      }

      return eigenvectors;
   }

   void calculateLocalStatistics( typename Mesh<T>::P mesh, const MeshIndex v, typename VerticesNearestVerticesSearch<T>::P vnvs, 
         LocalStatistics* lstats )
   {
      T vtx[3];

      vnvs->reset( );
   
      mesh->getVertex( v, vtx );
      vnvs->setTestPoint( vtx );

      mesh->searchVertices( vnvs );

      MeshIndex npts = vnvs->getNumberOfPointsFound( );

      vnl_matrix_fixed<T,3,3> sum_xxt;
      vnl_vector_fixed<T,3>   sum_x;
      T                       sum_xtx;

      sum_xxt.fill( 0 );
      sum_x.fill( 0 );
      sum_xtx = 0;

      for ( MeshIndex vv = 0; vv < npts; ++vv )
      {
         mesh->getVertex( vnvs->getPointFoundIndex( vv ), vtx );
         
         // Update the mean and variance
         for ( int r = 0; r < 3; ++r )
         {
            sum_x(r) += vtx[r];
            sum_xtx  += vtx[r]*vtx[r];
         }

         // Update the covariance
         for ( int r = 0; r < 3; ++r )
            for ( int c = 0; c < 3; ++c )
            {
               sum_xxt[r][c] += vtx[r]*vtx[c];
            }

      }

      lstats->covariance   = sum_xxt - outer_product( sum_x, sum_x ) / static_cast<T>( npts );
      lstats->mean         = sum_x / static_cast<T>( npts );
      lstats->scale        = sqrt( ( sum_xtx - inner_product( sum_x.as_vector(), sum_x.as_vector() ) / npts ) / npts );
      lstats->npts         = npts;
      
      lstats->eigenvectors = calculateEigenvectors( lstats->covariance );
   }

   void calculateForVertex( typename Mesh<T>::P mesh, const MeshIndex v, typename VerticesNearestVerticesSearch<T>::P vnvs, vnl_vector<T>& out_Matrix )
   {
      LocalStatistics* lstats = new LocalStatistics;

      calculateLocalStatistics( mesh, v, vnvs, lstats );
      
      // So: we now need to make the points invariant to translation, rotation and scale
      // (don't understand what he means by the reflection invariance)

      vnl_matrix_fixed<T,3,3> scalerotate = vnl_svd<T>( lstats->eigenvectors ).inverse( ) / lstats->scale;

      vnl_matrix<T> m( lstats->npts, 9 );

      vnl_vector_fixed<T,3> vtx;
      vnl_vector_fixed<T,3> vtxHolder;

      for ( MeshIndex v = 0; v < lstats->npts; ++v )
      {
         mesh->getVertex( vnvs->getPointFoundIndex( v ), vtxHolder.data_block() );

         // Shift, rotate and scale the point
         vtx = scalerotate * ( vtxHolder - lstats->mean );

         // f(x,y,z) =   a0 * x^2 + a1 * y^2 + a2 * z^2
         //            + a3 * x*y + a4 * x*z + a5 * y*z
         //            + a6 * x   + a7 * y   + a8 * z     + 1  <-- Couldn't you just do it with null-space method?

         m(v,0) = vtx[0] * vtx[0];
         m(v,1) = vtx[1] * vtx[1];
         m(v,2) = vtx[2] * vtx[2];

         m(v,3) = vtx[0] * vtx[1];
         m(v,4) = vtx[0] * vtx[2];
         m(v,5) = vtx[1] * vtx[2];

         m(v,6) = vtx[0];
         m(v,7) = vtx[1];
         m(v,8) = vtx[2];
      }

      vnl_vector<T> b( lstats->npts, 1 );
      b.fill( -1 );

      out_Matrix = vnl_svd<T>( m ).solve( b );
//PRINTVBL( out_Matrix );

      delete lstats;
   }
};

template <class T>
AWT::MoghariMeasure<T>::MoghariMeasure( )
{
   m_D = new D;
}

template <class T>
AWT::MoghariMeasure<T>::~MoghariMeasure( )
{
   delete m_D;
}

template <class T>
typename AWT::MoghariMeasure<T>::P AWT::MoghariMeasure<T>::getInstance( )
{
   AUTOGETINSTANCE( AWT::MoghariMeasure<T>, ( ) );
}

template <class T>
GETNAMEMACRO( AWT::MoghariMeasure<T> );

template <class T>
void AWT::MoghariMeasure<T>::calculateLocalStatistics( typename AWT::Mesh<T>::P mesh, const AWT::MeshIndex v, const T radius, 
                                                       typename AWT::MoghariMeasure<T>::LocalStatistics* lstats )
{
   VerticesNearestVerticesSearch<T>::P vnvs = VerticesNearestVerticesSearch<T>::getInstance( );
   vnvs->setRadius( radius );

   m_D->calculateLocalStatistics( mesh, v, vnvs, lstats );
}

template <class T>
void AWT::MoghariMeasure<T>::calculateForVertex( typename AWT::Mesh<T>::P mesh, const AWT::MeshIndex v, const T radius, vnl_vector<T>& out_Matrix )
{
   VerticesNearestVerticesSearch<T>::P vnvs = VerticesNearestVerticesSearch<T>::getInstance( );
   vnvs->setRadius( radius );

   DEBUGLINE;
   m_D->calculateForVertex( mesh, v, vnvs, out_Matrix );
   DEBUGLINE;
}

template <class T>
typename AWT::Tuples<T>::P AWT::MoghariMeasure<T>::calculate( typename AWT::Mesh<T>::P mesh, const T radius )
{
   T def[9];

   for ( int i = 0; i < 9; ++i )
      def[i] = 0;

   Tuples<T>::P ret = TuplesImpl<T>::getInstance( 9, def, mesh->getNumberOfVertices( ) );

   vnl_vector<T> holder(9);

   VerticesNearestVerticesSearch<T>::P vnvs = VerticesNearestVerticesSearch<T>::getInstance( );

   vnvs->setRadius( radius );

   //for ( MeshIndex v = 0; v < mesh->getNumberOfVertices( ); ++v )
   MESH_EACHVERTEX( mesh, v )
   {
      if ( v % 100 == 0 )
         DEBUGMACRO( v << "/" << mesh->getNumberOfVertices( ) );

      m_D->calculateForVertex( mesh, v, vnvs, holder );

      ret->setPoint( v, holder.data_block() );
   }

   return ret;
}

template <class T>
vnl_matrix_fixed<T,3,3> AWT::MoghariMeasure<T>::calculateEigenvectors( const vnl_matrix<T>& m )
{
   return m_D->calculateEigenvectors( m );
}

template class AWT::MoghariMeasure<double>;
template class AWT::MoghariMeasure<float>;
