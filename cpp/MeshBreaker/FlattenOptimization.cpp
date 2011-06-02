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
#include "FlattenOptimization.h"

#include "Mesh/TuplesImpl.h"
#include "Useful/ArrayVectorFunctions.h"
#include "Useful/WalkInASpiral.h"
#include "NonRigidAlignment/ControlPoint.h"

#include "vnl/vnl_matrix.h"
#include <set>

template <class T>
struct AWT::FlattenOptimization<T>::D
{
   typedef std::pair<MeshIndex,T>  InfluencePair;
   typedef std::set<InfluencePair> InfluencePairSet;

   void AinvB( const T A[3][3], const T B[3][3], T M[3][3] )
   {
      // This assumes that the matrices are column-major
      // MATLAB generated code
      T det = B[0][0]*B[1][1]*B[2][2]-B[0][0]*B[1][2]*B[2][1]-B[1][0]*B[0][1]*B[2][2]+B[1][0]*B[0][2]*B[2][1]+B[2][0]*B[0][1]*B[1][2]-B[2][0]*B[0][2]*B[1][1];

      M[0][0] = ( A[0][0]*B[1][1]*B[2][2]-A[0][0]*B[1][2]*B[2][1]-A[1][0]*B[0][1]*B[2][2]+A[1][0]*B[0][2]*B[2][1]+A[2][0]*B[0][1]*B[1][2]-A[2][0]*B[0][2]*B[1][1] ) / det; 
      M[1][0] = (-A[0][0]*B[1][0]*B[2][2]+A[0][0]*B[1][2]*B[2][0]+A[1][0]*B[0][0]*B[2][2]-A[1][0]*B[0][2]*B[2][0]-A[2][0]*B[0][0]*B[1][2]+A[2][0]*B[0][2]*B[1][0] ) / det;
      M[2][0] = ( A[0][0]*B[1][0]*B[2][1]-A[0][0]*B[1][1]*B[2][0]-A[1][0]*B[0][0]*B[2][1]+A[1][0]*B[0][1]*B[2][0]+A[2][0]*B[0][0]*B[1][1]-A[2][0]*B[0][1]*B[1][0] ) / det;

      M[0][1] = ( A[0][1]*B[1][1]*B[2][2]-A[0][1]*B[1][2]*B[2][1]-A[1][1]*B[0][1]*B[2][2]+A[1][1]*B[0][2]*B[2][1]+A[2][1]*B[0][1]*B[1][2]-A[2][1]*B[0][2]*B[1][1] ) / det;
      M[1][1] = (-A[0][1]*B[1][0]*B[2][2]+A[0][1]*B[1][2]*B[2][0]+A[1][1]*B[0][0]*B[2][2]-A[1][1]*B[0][2]*B[2][0]-A[2][1]*B[0][0]*B[1][2]+A[2][1]*B[0][2]*B[1][0] ) / det;
      M[2][1] = ( A[0][1]*B[1][0]*B[2][1]-A[0][1]*B[1][1]*B[2][0]-A[1][1]*B[0][0]*B[2][1]+A[1][1]*B[0][1]*B[2][0]+A[2][1]*B[0][0]*B[1][1]-A[2][1]*B[0][1]*B[1][0] ) / det;

      M[0][2] = ( A[0][2]*B[1][1]*B[2][2]-A[0][2]*B[1][2]*B[2][1]-A[1][2]*B[0][1]*B[2][2]+A[1][2]*B[0][2]*B[2][1]+A[2][2]*B[0][1]*B[1][2]-A[2][2]*B[0][2]*B[1][1] ) / det;
      M[1][2] = (-A[0][2]*B[1][0]*B[2][2]+A[0][2]*B[1][2]*B[2][0]+A[1][2]*B[0][0]*B[2][2]-A[1][2]*B[0][2]*B[2][0]-A[2][2]*B[0][0]*B[1][2]+A[2][2]*B[0][2]*B[1][0] ) / det;
      M[2][2] = ( A[0][2]*B[1][0]*B[2][1]-A[0][2]*B[1][1]*B[2][0]-A[1][2]*B[0][0]*B[2][1]+A[1][2]*B[0][1]*B[2][0]+A[2][2]*B[0][0]*B[1][1]-A[2][2]*B[0][1]*B[1][0] ) / det;
   }

   void calculateFaceJacobians( )
   {
      // Want to calculate the direction of the flattened x-y directions in the unflattened space of each triangle

      // What does this mean?  It means that if a point is over triangle f and a control parameter
      // is waggled, then the change in unflattened space is w*( jac[0][f] jac[1][f] ), where w
      // is the influence parameter

      Mesh<T>::P flatMesh   = m_Flatten->getFlattenedMesh( );
      Mesh<T>::P unflatMesh = m_Flatten->getMesh( );

      m_Jacobians[0] = TuplesImpl<T>::getInstance( 3, flatMesh->getNumberOfFaces( ) );
      m_Jacobians[1] = TuplesImpl<T>::getInstance( 3, flatMesh->getNumberOfFaces( ) );

      T fOrigin[3], fBasis[3][3];
      T uOrigin[3], uBasis[3][3];
      T proj[3][3];

      MESH_EACHFACE( flatMesh, f )
      {
         // Get the flattened face and translate it to the first vertex as origin
         // Get the unflattened face and translate...
         flatMesh->getFace( f, fOrigin, fBasis[0], fBasis[1] );
         unflatMesh->getFace( f, uOrigin, uBasis[0], uBasis[1] );
         FOREACHAXIS( ax )
         {
            fBasis[0][ax] -= fOrigin[ax];
            fBasis[1][ax] -= fOrigin[ax];

            uBasis[0][ax] -= uOrigin[ax];
            uBasis[1][ax] -= uOrigin[ax];
         }

         // Cross to get the z-component
         cross<T>( fBasis[0], fBasis[1], fBasis[2] );
         cross<T>( uBasis[0], uBasis[1], uBasis[2] );

         // So, given a point x in the flattened space, we can project it back into the
         // triangle-orthogonal space (ignoring translation): inv( fBasis ) * x

         // But, we need to get it back into world space.  We can do this by pre-multiplying
         // by [ uvtxA uvtxB uvtxC ], i.e. the full transformation is 
         // [ uBasis ] * inv( fBasis ) * x = proj * x

         AinvB( uBasis, fBasis, proj );

         // Now, store the jacobian for each triangle: we want to project the x- and y-axes of
         // flattened space through: the x-axis projects to the 1st column, the y-axis projects
         // to the 2nd column
         m_Jacobians[0]->setPoint( f, proj[0] );
         m_Jacobians[1]->setPoint( f, proj[1] );
      }
   }

   void calculateWeights( )
   {
      // I'm going to assume that we have a small enough number of control points/ref points to do this
      // in an N^2 way... I may be wrong
      DEBUGMACRO( "Calculating weights N^2" );

      T vtx[3], vtxTest[3];

      vtxTest[2] = 0;

      // Create holders to look up in both directions
      for ( MeshIndex c = 0; c < m_ControlPoints.size( ); ++c )
         m_RefPointsNearControlPoints.push_back( InfluencePairSet( ) );

      for ( MeshIndex c = 0, cmax = m_ReferencePoints->getNumberOfPoints( ); c < cmax; ++c )
         m_ControlPointsNearRefPoints.push_back( InfluencePairSet( ) );

      for ( MeshIndex c = 0; c < m_ControlPoints.size( ); ++c )
      {
         ControlPoint<T>::P cp = m_ControlPoints[c];

         for ( MeshIndex r = 0, rmax = m_ReferencePoints->getNumberOfPoints( ); r < rmax; ++r )
         {
            // Get the point
            m_ReferencePoints->getPoint( r, vtx );

            // Calculate the influence on this point, bearing in mind that the influence
            // wraps around the mesh... walk outwards look for influence
            int x = 0, y = 0;

            T influence = 0;

            while ( influence == 0 && ( abs(x) < 2 || abs(y) < 2 ) )
            {
               vtxTest[0] = vtx[0] + x;
               vtxTest[1] = vtx[1] + y;

               influence = cp->getInfluence( vtxTest );

               walkInASpiral( x, y );
            }

            if ( influence != 0 )
            {
               // Keep both so that we can look up in either direction
               m_RefPointsNearControlPoints[c].insert( InfluencePair( r, influence ) );
               m_ControlPointsNearRefPoints[r].insert( InfluencePair( c, influence ) );
            }
         }
      }

      DEBUGMACRO( "Finished calculating weights N^2." );
   }

   void calculateJacobian( vnl_matrix<T>& jac )
   {
      jac.fill( 0 );

      for ( MeshIndex r = 0, rmax = m_ReferencePoints->getNumberOfPoints( ); r < rmax; ++r )
         calculateJacobianForReferencePoint( r, jac );
   }

   void calculateJacobianForReferencePoint( const MeshIndex r, vnl_matrix<T>& jac )
   {
      T vtxFlat[3], vtxUnflat[3];
      T jacX[3], jacY[3];

      const MeshIndex ncontrols = static_cast<MeshIndex>( m_ControlPoints.size( ) );

      // Get the reference point
      m_ReferencePoints->getPoint( r, vtxFlat );

      // Work out what face it is over in the flattened mesh...
      MeshIndex flatFace = m_Flatten->mapFlattenedToMesh( vtxFlat, vtxUnflat );

      // ...and get the unflattened jacobian for it
      m_Jacobians[0]->getPoint( flatFace, jacX );
      m_Jacobians[1]->getPoint( flatFace, jacY );

      // Don't fill with zero initially - will assume this has been done already

      InfluencePairSet::iterator it = m_ControlPointsNearRefPoints[r].begin( );
      InfluencePairSet::iterator en = m_ControlPointsNearRefPoints[r].end( );

      for ( ; it != en; ++it )
      {
         const MeshIndex c      = it->first;
         const T         weight = it->second;

         // When the x-component of the control point is varied by a unit change, this is how much this point moves
         jac[ 3*r+0 ][ 2*c + 0 ] = weight * jacX[0];
         jac[ 3*r+1 ][ 2*c + 0 ] = weight * jacX[1];
         jac[ 3*r+2 ][ 2*c + 0 ] = weight * jacX[2];

         // When the y-component of the control point is varied by a unit change, this is how much this point moves
         jac[ 3*r+0 ][ 2*c + 1 ] = weight * jacY[0];
         jac[ 3*r+1 ][ 2*c + 1 ] = weight * jacY[1];
         jac[ 3*r+2 ][ 2*c + 1 ] = weight * jacY[2];
      }
   }

   void getControlParameters( vnl_matrix<T>& v )
   {
      T val[3];

      for ( MeshIndex c = 0; c < m_ControlPoints.size( ); ++c )
      {
         ControlPoint<T>::P cp = m_ControlPoints[c];

         cp->getValue( val );

         v[0][2*c+0] = val[0];
         v[0][2*c+1] = val[1];
      }
   }

   void incrementControlParameters( vnl_matrix<T>& v )
   {
      T val[3];

      // Increment the control parameters
      for ( MeshIndex c = 0; c < m_ControlPoints.size( ); ++c )
      {
         ControlPoint<T>::P cp = m_ControlPoints[c];

         cp->getValue( val );

         val[0] += v[0][2*c+0];
         val[1] += v[0][2*c+1];

         cp->setValue( val );
      }

      updateReferencePointLocations( );
   }

   void updateReferencePointLocations( )
   {
      T vtx[3], val[3];

      // Reset the reference points to their original locations
      // and then move to their new location
      for ( MeshIndex r = 0, rmax = m_ReferencePoints->getNumberOfPoints( ); r < rmax; ++r )
      {
         m_ReferencePointsOrig->getPoint( r, vtx );

         InfluencePairSet::iterator it = m_ControlPointsNearRefPoints[r].begin( );
         InfluencePairSet::iterator en = m_ControlPointsNearRefPoints[r].end( );

         for ( ; it != en; ++it )
         {
            ControlPoint<T>::P cp        = m_ControlPoints[it->first];
            T                  influence = it->second;

            // Get the control point's parameters
            cp->getValue( val );

            vtx[0] += val[0]*influence;
            vtx[1] += val[1]*influence;
         }

         m_ReferencePoints->setPoint( r, vtx );
      }
   }

   typename Tuples<T>::P                      m_Jacobians[2];
   typename FlattenMeshPair<T>::P             m_Flatten;
   typename Tuples<T>::P                      m_ReferencePoints;
   typename Tuples<T>::P                      m_ReferencePointsOrig;

   std::vector< typename ControlPoint<T>::P > m_ControlPoints;

   std::vector< InfluencePairSet >            m_RefPointsNearControlPoints;
   std::vector< InfluencePairSet >            m_ControlPointsNearRefPoints;
};

template <class T>
AWT::FlattenOptimization<T>::FlattenOptimization( typename AWT::FlattenMeshPair<T>::P flatten, 
                                                  typename AWT::Tuples<T>::P referencePoints,
                                                  std::vector< typename ControlPoint<T>::P > controlPoints )
{
   m_D = new D;

   m_D->m_Flatten             = flatten;
   m_D->m_ReferencePointsOrig = TuplesImpl<T>::getInstance( referencePoints );
   m_D->m_ReferencePoints     = referencePoints;
   m_D->m_ControlPoints       = controlPoints;


   m_D->calculateFaceJacobians( );
   m_D->calculateWeights( );
}

template <class T>
AWT::FlattenOptimization<T>::~FlattenOptimization( )
{
   delete m_D;
}

template <class T>
typename AWT::FlattenOptimization<T>::P AWT::FlattenOptimization<T>::getInstance( typename AWT::FlattenMeshPair<T>::P flatten, 
                                                  typename AWT::Tuples<T>::P referencePoints,
                                                  std::vector< typename ControlPoint<T>::P > controlPoints )
{
   AUTOGETINSTANCE( AWT::FlattenOptimization<T>, ( flatten, referencePoints, controlPoints ) );
}

template <class T>
GETNAMEMACRO( AWT::FlattenOptimization<T> );

template <class T>
void AWT::FlattenOptimization<T>::getControlParameters( vnl_matrix<T>& v )
{
   m_D->getControlParameters( v );
}

template <class T>
void AWT::FlattenOptimization<T>::incrementControlParameters( vnl_matrix<T>& v )
{
   m_D->incrementControlParameters( v );
}

template class AWT::FlattenOptimization<double>;
template class AWT::FlattenOptimization<float>;