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
#include "ControlPointSet.h"

#include "ControlPoint.h"
#include "ControlPointSearcher.h"
#include "ControlPointSetTreeData.h"
#include "DeformationAccumulator.h"

#include "ObjectEKDTree/OEKDTree.h"

#include "Useful/PrintMacros.h"

#include "vnl/algo/vnl_cholesky.h"
#include "vnl/algo/vnl_sparse_lu.h"

#include "vnl/vnl_matrix.h"
#include "vnl/vnl_sparse_matrix.h"
#include "vnl/vnl_vector.h"

#include <cmath>
#include <vector>

template <class T>
struct AWT::ControlPointSet<T>::D
{
   typename ControlPointSetTreeData<T>::P          m_TreeData;
   std::vector<typename ControlPoint<T>::P>        m_ControlPoints;
   typename OEKDTree::OEKDTree<T,3>::P             m_Tree;
};

template <class T>
AWT::ControlPointSet<T>::ControlPointSet( )
{
   m_D = new D;

   m_D->m_TreeData = ControlPointSetTreeData<T>::getInstance( &m_D->m_ControlPoints );
}

template <class T>
AWT::ControlPointSet<T>::~ControlPointSet( )
{
   delete m_D;
}

template <class T>
typename AWT::ControlPointSet<T>::P AWT::ControlPointSet<T>::getInstance( )
{
   AWT::ControlPointSet<T>* inst = new AWT::ControlPointSet<T>( );
   inst->countMe( );

   typename AWT::ControlPointSet<T>::P ret( inst );
   inst->release( );

   return ret;
}

template <class T>
std::string AWT::ControlPointSet<T>::getClassName( ) const
{
   return "AWT::ControlPointSet<T>";
}

template <class T>
typename AWT::ControlPointSet<T>::P AWT::ControlPointSet<T>::getInstance( T* bounds, T spacing )
{
   ControlPointSet<T>::P ret = ControlPointSet<T>::getInstance( );
      
   int boundsCP[6];

   for ( int i = 0; i < 6; i += 2 )
   {
      boundsCP[i+0] = static_cast<int>( spacing * ( floor( (bounds[i+0]) / spacing ) - 1 ) );
      boundsCP[i+1] = static_cast<int>( spacing * ( ceil ( (bounds[i+1]) / spacing ) + 1 ) );
   }

   //PRINTVEC( boundsCP, 6 );
   
   T zero[] = { 0, 0, 0 };

   int pnti[3];
   T pnt[3];
   for ( pnti[2] = boundsCP[4]; pnti[2] <= boundsCP[5]; pnti[2] += static_cast<int>( spacing ) )
   {
      for ( pnti[1] = boundsCP[2]; pnti[1] <= boundsCP[3]; pnti[1] += static_cast<int>( spacing ) )
      {
         for ( pnti[0] = boundsCP[0]; pnti[0] <= boundsCP[1]; pnti[0] += static_cast<int>( spacing ) )
         {
            pnt[0] = static_cast<T>( pnti[0] );
            pnt[1] = static_cast<T>( pnti[1] );
            pnt[2] = static_cast<T>( pnti[2] );

            ControlPoint<T>::P cp = ControlPoint<T>::getInstance( pnt, spacing );
            cp->setValue( zero );
            ret->add( cp );
         }
      }
   }
   PRINTVBL( ret->getNumberOfObjects( ) );

   return ret;
}

template <class T>
void AWT::ControlPointSet<T>::add( typename ControlPoint<T>::P cp )
{
   m_D->m_ControlPoints.push_back( cp );

   modified( );
}

template <class T>
void AWT::ControlPointSet<T>::add( typename ControlPointSet<T>::P cps )
{
   for ( OEKDTree::ObjectIndex v = 0; v < cps->getNumberOfObjects( ); ++v )
      add( cps->getControlPoint( v ) );
}

template <class T>
AWT::OEKDTree::ObjectIndex AWT::ControlPointSet<T>::getNumberOfObjects( ) const
{
   return static_cast<OEKDTree::ObjectIndex>( m_D->m_ControlPoints.size( ) );
}

template <class T>
const typename AWT::ControlPoint<T>::P AWT::ControlPointSet<T>::getControlPointConst( OEKDTree::ObjectIndex oi ) const
{
   return *(m_D->m_ControlPoints[oi]);
}

template <class T>
typename AWT::ControlPoint<T>::P AWT::ControlPointSet<T>::getControlPoint( OEKDTree::ObjectIndex oi )
{
   return *(m_D->m_ControlPoints[oi]);
}

template <class T>
T AWT::ControlPointSet<T>::getValue( OEKDTree::ObjectIndex objectIndex, OEKDTree::AxisIndex axis ) const
{
   const ControlPoint<T>* cp = *(m_D->m_ControlPoints[objectIndex]);

   return cp->getPosition( axis );
}

template <class T>
T AWT::ControlPointSet<T>::getMinimumBound( OEKDTree::ObjectIndex objectIndex, OEKDTree::AxisIndex axis ) const
{
   const ControlPoint<T>* cp = *(m_D->m_ControlPoints[objectIndex]);

   return cp->getPosition( axis ) - 2*cp->getScale( );
}

template <class T>
T AWT::ControlPointSet<T>::getMaximumBound( OEKDTree::ObjectIndex objectIndex, OEKDTree::AxisIndex axis ) const
{
   const ControlPoint<T>* cp = *(m_D->m_ControlPoints[objectIndex]);

   return cp->getPosition( axis ) + 2*cp->getScale( );
}

template <class T>
void AWT::ControlPointSet<T>::search( typename OEKDTree::OEKDSearcher<T,3>::P searcher )
{
   if ( *m_D->m_Tree == 0 )
   {
      m_D->m_Tree = OEKDTree::OEKDTree<T,3>::getInstance( *m_D->m_TreeData );
   }
   else if ( m_D->m_Tree->getModifiedTime( ) < this->getModifiedTime( ) )
   {
      m_D->m_Tree->build( );
   }

   searcher->searchTree( *m_D->m_Tree );
}

template <class T>
void AWT::ControlPointSet<T>::fit(typename AWT::ControlPointSet<T>::P cps)
{
   const unsigned int n = this->getNumberOfObjects( );

   // We need these to search the other control points
   ControlPointSearcher<T>::P   searcher = ControlPointSearcher<T>::getInstance( );
   searcher->setData( getTreeData() );

   vnl_sparse_matrix<double> A( n, n );
   vnl_vector<double> b[] = { vnl_vector<double>(n), vnl_vector<double>(n), vnl_vector<double>(n) };

   b[0].fill( 0 );
   b[1].fill( 0 );
   b[2].fill( 0 );

   DeformationAccumulator<T>::P defAccum = DeformationAccumulator<T>::getInstance( cps );
   searcher->setCallback( defAccum );

   T posA[3];
   T def[3];

   for ( unsigned int r = 0; r < n; ++r )
   {
      ControlPoint<T>::P cpA = this->getControlPoint( r );
      cpA->getPosition( posA );

      for ( unsigned int c = 0; c < n; ++c )
      {
         // A is the matrix of coupling coeffs between points in this control set
         T influence = this->getControlPoint( c )->getInfluence( posA );
         
         if ( influence != 0 )
            A(c,r) = influence;
      }

      defAccum->reset( );
      defAccum->setTestPoint( posA );
      searcher->reset();
      searcher->setTestPoint( 0, posA );

      defAccum->getDeformation( def );

      cps->search( searcher );

      defAccum->getDeformation( def );

      for ( int ax = 0; ax < 3; ++ax )
         b[ax](r) = def[ax];
   }

   vnl_sparse_lu solver(A);
   vnl_vector<double> x[3];

   for ( int ax = 0; ax < 3; ++ax )
      x[ax] = solver.solve( b[ax] );

   for ( unsigned int r = 0; r < n; ++r )
   {
      ControlPoint<T>::P cpA = this->getControlPoint( r );

      for ( int ax = 0; ax < 3; ++ax )
         cpA->setValue( ax, static_cast<T>( x[ax](r) ) );
   }

   {
      DeformationAccumulator<T>::P defAccum = DeformationAccumulator<T>::getInstance( ControlPointSet<T>::P(this) );
      searcher->setCallback( defAccum );

      for ( unsigned int r = 0; r < 3; ++r )
      {
         ControlPoint<T>::P cpA = this->getControlPoint( r );
         cpA->getPosition( posA );

         defAccum->reset( );
         defAccum->setTestPoint( posA );
         searcher->reset();
         searcher->setTestPoint( 0, posA );

         defAccum->getDeformation( def );

         PRINTVEC(def,3);
         DEBUGMACRO(b[0][r] << "\t" << b[1][r] << "\t" << b[2][r]);
      }
   }
}

template <class T>
typename AWT::ControlPointSetTreeData<T>::P AWT::ControlPointSet<T>::getTreeData( )
{
   return m_D->m_TreeData;
}

template class AWT::ControlPointSet<double>;
template class AWT::ControlPointSet<float>;