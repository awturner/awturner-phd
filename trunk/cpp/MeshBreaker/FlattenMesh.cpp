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
#include "FlattenMesh.h"

#include "Mesh/Mesh.h"
#include "Mesh/MeshImpl.h"
#include "Mesh/MeshConnectivity.h"

#include "Mesh/VTKMeshWriter.h"

#include "HalfEdge.h"
#include "HalfEdgeVertex.h"

#include "Useful/WalkInASpiral.h"
#include "Useful/ProfTimer.h"

#include "Mesh/AreaAveragedNormalCalculator.h"

#include <set>
#include <fstream>
#include <map>

#include "vnl/vnl_matrix.h"
#include "vnl/vnl_sparse_matrix.h"
#include "vnl/algo/vnl_sparse_lu.h"
#include "vnl/algo/vnl_lsqr.h"
#include "vnl/vnl_sparse_matrix_linear_system.h"

#include "Mesh/TriangleProjection.h"

#include "MeshSearching/FacesRayIntersectionSearch.h"
#include "MeshSearching/FacesNearestPointSearch.h"

template <class T>
struct AWT::FlattenMesh<T>::D
{
   typedef std::vector<HalfEdge*>                HalfEdgeVec;

   D( )
   {
      m_FlattenScale = 1;
   }

   void findNeighbourSet( const HalfEdgeVec& vec, std::set<HalfEdgeVertex*>& contour, std::set<HalfEdgeVertex*>& neigh )
   {
      // Let's find all of the vertices surrounding all of the vertices on the contour
      HalfEdgeVec::const_iterator begin = vec.begin( );
      HalfEdgeVec::const_iterator end   = vec.end( );

      for ( ; begin != end; ++begin )
      {
         HalfEdgeVertex* v = (*begin)->getVertex( );

         contour.insert( v );

         // Work out what the next and previous vertices along are
         HalfEdgeVertex* vn;
         HalfEdgeVertex* vp;

         if ( (begin+1) != end )
            vn = (*(begin+1))->getVertex( );
         else
            vn = (*vec.begin( ))->getVertex( );

         if ( begin == vec.begin( ) )
            vp = vec.back( )->getVertex( );
         else
            vp = (*(begin-1))->getVertex( );

         // Get the outgoing edge to vn
         HalfEdge* e = v->getEdge( )->getPair( );
         while ( e->getVertex( ) != vn )
            e = e->getPair( )->getNext( );

         // Add all the vertices between vn and vp (exclusive)
         e = e->getPair( )->getNext( );
         while ( e->getVertex( ) != vp )
         {
            neigh.insert( e->getVertex( ) );

            e = e->getPair( )->getNext( );
         }
      }
   }

   // Given the sides of a triangle, calculates the angle bac
   // as in a*a = b*b + c*c - 2*b*c*cos(A)
   T angleA( const T a, const T b, const T c )
   {
      return acos( ( b*b + c*c - a*a ) / ( 2*b*c ) );
   }

   const T getWeight( HalfEdge* e )
   {
      // Get the four vertices
      T vtxI[3]; m_Fg->getMesh( )->getVertex( e->getPair( )->getVertex( )->getIndex( ), vtxI );
      T vtxJ[3]; m_Fg->getMesh( )->getVertex( e->getVertex( )->getIndex( ), vtxJ );
      T vtxK[3]; m_Fg->getMesh( )->getVertex( e->getPair( )->getNext( )->getVertex( )->getIndex( ), vtxK );
      T vtxM[3]; m_Fg->getMesh( )->getVertex( e->getNext( )->getVertex( )->getIndex( ), vtxM );
      
      // Let's work out the five edge lengths in this triangle pair
      const T ij = deltaNorm( vtxI, vtxJ, 3 );
      const T ik = deltaNorm( vtxI, vtxK, 3 );
      const T im = deltaNorm( vtxI, vtxM, 3 );
      const T jk = deltaNorm( vtxJ, vtxK, 3 );
      const T jm = deltaNorm( vtxJ, vtxM, 3 );

      switch ( m_WeightType )
      {
      case WEIGHT_FLOATER:
         {
            // Floater's mean value coordinates
            const T jim = angleA( jm, ij, im );
            const T jik = angleA( jk, ik, ij );
            return ( tan( jim/2 ) + tan( jik/2 ) ) / ij;
         }
         break;
      case WEIGHT_HARMONIC:
         {
            //Discrete harmonic coordinates
            const T ikj = angleA( ij, ik, jk );
            const T imj = angleA( ij, im, jm );
            return 1 / tan( ikj ) + 1 / tan( imj );
         }
         break;
      case WEIGHT_WACHSPRESS:
         {
            // Wachspress coordinates
            const T ijm = angleA( im, ij, jm );
            const T ijk = angleA( ik, ij, jk );
            return ( 1/tan(ijm) + 1/tan(ijk) ) / (ij*ij);
         }
         break;

         
      case WEIGHT_TUTTE:
      default:
         // This equally weights the surrounding vertices
         return 1;
      }
   }

   void normalizeRow( vnl_sparse_matrix<T>& Ap, T constraintsp[2], const MeshIndex v )
   {
      // Normalize the row
      Ap( v, v ) = 0;
      const T rowsum = -Ap.sum_row( v );

      if ( rowsum != 0 )
      {
         Ap.scale_row( v, 1/rowsum );
         constraintsp[0] /= rowsum;
         constraintsp[1] /= rowsum;
      }

      Ap( v, v ) = 1;
   }

   bool inContour( const MeshIndex c, const MeshIndex v )
   {
      std::vector< HalfEdgeVertex* >* vertices = &m_Fg->getHalfEdgeMesh( )->getVertices( );
      return m_ContourVertices[c].find( (*vertices)[v] )   != m_ContourVertices[c].end( );
   }

   bool inContourNeighbours( const MeshIndex c, const MeshIndex v )
   {
      std::vector< HalfEdgeVertex* >* vertices = &m_Fg->getHalfEdgeMesh( )->getVertices( );
      return m_NeighbourVertices[c].find( (*vertices)[v] )   != m_NeighbourVertices[c].end( );
   }

   bool isSecondPassFixed( const MeshIndex v )
   {
      std::vector< HalfEdgeVertex* >* vertices = &m_Fg->getHalfEdgeMesh( )->getVertices( );

      return false
         || inContour( 0, v )
         || inContour( 1, v )
         || inContourNeighbours( 0, v )
         || inContourNeighbours( 1, v );
   }

   void printSparseMatrix( const vnl_sparse_matrix<T>& A, const char* varname, const char* filename )
   {
      std::ofstream os( filename );

      os << varname << " = spalloc(" << A.rows() << "," << A.columns() << ", " << 10*A.rows() << ");" << std::endl;
      A.reset( );
      while ( A.next() )
      {
         os << varname << "(" << (1+A.getrow()) << "," << (1+A.getcolumn()) << ") = " << A.value() << ";" << std::endl;
      }
      os.close( );
   }

   struct FlattenLinearSystem
   {
      vnl_sparse_matrix<double> A;
      vnl_vector<double>        b[2];
   };

   void initializeSystem( FlattenLinearSystem& fls )
   {
      const MeshIndex nverts = m_FlatMesh->getNumberOfVertices( );

      fls.A.set_size( nverts, nverts );
      fls.b[0].set_size( nverts );
      fls.b[1].set_size( nverts );

      fls.b[0].fill( 0 );
      fls.b[1].fill( 0 );

      for ( MeshIndex v = 0; v < nverts; ++v )
         fls.A( v, v ) = 1;
   }

   void solve( )
   {
      const MeshIndex nverts = m_FlatMesh->getNumberOfVertices( );

      // Weighting Matrix
      //vnl_sparse_matrix<double> Ap( nverts, nverts );
      //vnl_vector<double>        bp[] = {
      //   vnl_vector<double>( nverts ),
      //   vnl_vector<double>( nverts ),
      //};
      //initializeSystem( Ap, bp[0], bp[1] );
      
      FlattenLinearSystem r, q;
      initializeSystem( r );
      initializeSystem( q );
      /*
      vnl_sparse_matrix<double> Aq;
      vnl_vector<double>        bq[2];
      initializeSystem( Aq, bq[0], bq[1] );

      vnl_sparse_matrix<double> Ar;
      vnl_vector<double>        br[2];
      initializeSystem( Ar, br[0], br[1] );
      */

      std::vector< HalfEdgeVertex* >* vertices = &m_Fg->getHalfEdgeMesh( )->getVertices( );

      PRINTVBL( m_FixedPoint );

      //MeshConnectivity<T>::P conn = MeshConnectivity<T>::getInstance( m_FlatMesh );

      DEBUGMACRO( "Populating adjacency matrix" );
      try
      {
         for ( MeshIndex v = 0; v < nverts; ++v )
         {
            // Non-manifold vertex?
            if ( (*vertices)[v]->getEdge( ) == 0 )
            {
               DEBUGMACRO( "Disconnected vertex at " << v );
               continue;
            }

            // OK, let's get the vertex and its neighbours
            T w;

            // Get an outgoing edge from the current vertex
            HalfEdge* e = (*vertices)[v]->getEdge( )->getPair( );
            const HalfEdge* s = e;
            
            const bool spf = isSecondPassFixed( v );

            do
            {
               w = getWeight( e );

               if ( v != m_FixedPoint )
                  q.A( v, e->getVertex( )->getIndex( ) ) = -w;

               if ( !spf )
                  r.A( v, e->getVertex( )->getIndex( ) ) = -w;

               // Move to the next outgoing edge
               e = e->getPair( )->getNext( );
            }
            while ( e != s );
         }
      }
      catch (std::exception ex)
      {
         PRINTVBL( ex.what( ) );
         throw ex;
      }
      DEBUGLINE;

      std::vector<T> vertexStretch;
      MESH_EACHFACE( m_FlatMesh, f )
         vertexStretch.push_back( 1 );

      T lastStretchNorm = std::numeric_limits<T>::infinity();

      for ( MeshIndex iter = 0; iter < 25; ++iter )
      {
         BLANKLINE;
         DEBUGMACRO( "========== " << iter << " ==========" );
         // Go round again to set the constraints
         MESH_EACHVERTEX( m_FlatMesh, v )
         {
            // Is this vertex in the boundary set?
            const bool isInA[]  = { inContour( 0, v ),           inContour( 1, v )           };

            // Is this vertex in the boundary neighbour set?
            const bool isInNa[] = { inContourNeighbours( 0, v ), inContourNeighbours( 1, v ) };
            
            // Will this be fixed on the second pass?
            const bool spf = isSecondPassFixed( v );

            // Get an outgoing edge from the current vertex
            HalfEdge* e = (*vertices)[v]->getEdge( )->getPair( );
            const HalfEdge* s = e;

            T constraintsq[] = { 0, 0 };

            do
            {
               const MeshIndex vi = e->getVertex( )->getIndex( );
               for ( int ax = 0; ax < 2; ++ax )
               {
                  // This enforces the cyclic boundary constraints
                  if ( isInA[ax] )
                  {
                     if ( inContourNeighbours( ax, vi ) )
                     {
                        constraintsq[ax] += q.A( v, vi );
                     }
                  }
                  else if ( isInNa[ax] )
                  {
                     if ( inContour( ax, vi ) )
                     {
                        constraintsq[ax] -= q.A( v, vi );
                     }
                  }
               }

               // Move to the next outgoing edge
               e = e->getPair( )->getNext( );
            }
            while ( e != s );

            // Put into the rank-deficient system
            normalizeRow( q.A, constraintsq, v );
            q.b[0][v] = constraintsq[0];
            q.b[1][v] = constraintsq[1];

            // Doesn't matter what constraints are, they are recalculated
            T constraintsr[2];
            normalizeRow( r.A, constraintsr, v );
         }
         
         vnl_vector<double> x[2];
         vnl_vector<double> oldx[2];

         {
            // If the adjacency matrices were constructed with known values
            // then they can be solved directly using the LU factorization
            vnl_sparse_lu luq( q.A, vnl_sparse_lu::estimate_condition );
            vnl_sparse_lu lur( r.A, vnl_sparse_lu::estimate_condition );

            for ( MeshIndex i = 0; i < 2; ++i )
            {
               x[i] = luq.solve( q.b[i] );

               // Construct "Second pass" boundary conditions, where the edge
               // is now fixed.  Sorts out tanglement inside
               r.b[i].fill( 0 );
               MESH_EACHVERTEX( m_Fg->getMesh( ), v )
                  r.b[i][v] = ( isSecondPassFixed( v ) ) ? x[i][v] : 0;

               x[i] = lur.solve( r.b[i] );
            }
         }

         oldx[0].set_size( x[0].size() );
         oldx[1].set_size( x[1].size() );

         // Print the reciprocal condition number

         DEBUGMACRO( "Solved!" );

         DEBUGMACRO( "Updating vertex positions" );
         for ( MeshIndex v = 0; v < nverts; ++v )
         {
            T vtx[3];
            m_FlatMesh->getVertex( v, vtx );

            // Store the old positions in case we need to roll it back
            oldx[0][v] = vtx[0];
            oldx[1][v] = vtx[1];
            // Discard the z-value

            vtx[0] = static_cast<T>( x[0][v] );
            vtx[1] = static_cast<T>( x[1][v] );
            vtx[2] = 0;

            m_FlatMesh->setVertex( v, vtx );

            if ( v == m_FixedPoint )
            {
               PRINTVEC( vtx, 3 );
            }
         }
         DEBUGMACRO( "Vertex positions updated" );

         if ( m_SpreadType == SPREAD_NONE )
         {
            DEBUGMACRO( "Not using vertex spreading" );
            break;
         }
         else if ( m_SpreadType == SPREAD_AWT )
         {
            DEBUGMACRO( "Using AWT vertex spreading!" );
            spreadVertices( );
            break;
         }

         std::vector<T> vertexStretch;
         const T newStretchNorm = calculateVertexStretch( vertexStretch );

         if ( newStretchNorm > lastStretchNorm )
         {
            // Copy the old parametrization back again
            for ( MeshIndex v = 0; v < nverts; ++v )
            {
               T vtx[3];
               vtx[0] = static_cast<T>( oldx[0][v] );
               vtx[1] = static_cast<T>( oldx[1][v] );
               vtx[2] = 0;

               m_FlatMesh->setVertex( v, vtx );
            }

            break;
         }

         lastStretchNorm = newStretchNorm;
         PRINTVBL( lastStretchNorm );

         q.A.reset( );

         while ( q.A.next() )
         {
            if ( q.A.getrow() != m_FixedPoint )
               q.A( q.A.getrow(), q.A.getcolumn() ) /= vertexStretch[ q.A.getcolumn() ];
         }
         
         r.A.reset( );

         while ( r.A.next() )
         {
            r.A( r.A.getrow(), r.A.getcolumn() ) /= vertexStretch[ r.A.getcolumn() ];
         }
      }
   }

   T calculateVertexStretch( std::vector<T>& vertexStretch )
   {

      DEBUGMACRO( "Calculating sigma for all triangles" );

      std::vector<T> triangleSigma;
      std::vector<T> triangleArea;

      std::vector< HalfEdgeVertex* >* vertices = &m_Fg->getHalfEdgeMesh( )->getVertices( );

      T norm_num = 0;
      T norm_den = 0;

      //T maxArea = 0;
      //T maxSigma = 0;

      MESH_EACHFACE( m_FlatMesh, f )
      {
         // Need to work out the Jacobian for this face in the mapping
         // from 3d-2d

         // Get the vertices from the original mesh
         T originalVertices[3][3];
         m_Fg->getMesh( )->getFace( f, originalVertices[0], originalVertices[1], originalVertices[2] );

         // Get the flattened vertices
         T flatVertices[3][3]; // Third element should be zero
         m_FlatMesh->getFace( f, flatVertices[0], flatVertices[1], flatVertices[2] );

         // Get the indices of the vertices
         MeshIndex faceIndices[3];
         m_FlatMesh->getFaceIndices( f, faceIndices );

         // So... need to determine whether any of the vertex indices are in the neighbour set.
         // If they are, then any vertex indices in the contour set need to be unwrapped
         // How to do this...?
         for ( MeshIndex ax = 0; ax < 2; ++ax )
         {
            bool flag = false;
            for ( MeshIndex v = 0; v < 3 && !flag; ++v )
            {
               if ( m_NeighbourVertices[ax].find( (*vertices)[v] )   != m_NeighbourVertices[ax].end( ) )
                  flag = true;
            }

            if ( flag )
            {
               for ( MeshIndex v = 0; v < 3; ++v )
               {
                  if ( m_ContourVertices[ax].find( (*vertices)[v] )   != m_ContourVertices[ax].end( ) )
                     flatVertices[v][ax] += m_FlattenScale;
               }
            }
         }
         // OK, now the vertices should be unwrapped

         // Shift relative to 0th vertex
         FOREACHAXIS( ax )
         {
            originalVertices[1][ax] -= originalVertices[0][ax];
            originalVertices[2][ax] -= originalVertices[0][ax];

            flatVertices[1][ax] -= flatVertices[0][ax];
            flatVertices[2][ax] -= flatVertices[0][ax];
         }

         cross( originalVertices[1], originalVertices[2], originalVertices[0] );
         const T area = norm( originalVertices[0], 3 );

         T j_u[3];
         T j_v[3];

         T mat[2][2];
         mat[0][0] =  flatVertices[2][1];
         mat[0][1] = -flatVertices[1][1];
         mat[1][0] = -flatVertices[2][0];
         mat[1][1] =  flatVertices[1][0];

         const T det = mat[0][0]*mat[1][1] - mat[1][0]*mat[0][1];

         FOREACHAXIS( ax )
         {
            j_u[ax] = ( mat[0][0]*originalVertices[1][ax] + mat[0][1]*originalVertices[2][ax] ) / det;
            j_v[ax] = ( mat[1][0]*originalVertices[1][ax] + mat[1][1]*originalVertices[2][ax] ) / det;
         }

         // Now we have the Jacobian, now need to calculate the metric tensor
         T tens[2][2];
         tens[0][0] = dot( j_u, j_u, 3 );
         tens[0][1] = tens[1][0] = dot( j_u, j_v, 3 );
         tens[1][1] = dot( j_v, j_v, 3 );

         // And now we can calculate its eigenvalues
         const T det2 = sqrt( (tens[0][0]-tens[1][1])*(tens[0][0]-tens[1][1]) + 4*tens[0][1]*tens[0][1] );

         const T lambda[] = {
            ( tens[0][0] + tens[1][1] + det2 ) / 2,
            ( tens[0][0] + tens[1][1] - det2 ) / 2,
         };

         // Finally, this is our stretch, sigma
         const T sigma = sqrt( ( lambda[0] + lambda[1] ) / 2 );

         if ( sigma > 1e20 )
         {
            
            BLANKLINE;

            PRINTVEC( mat[0], 2 );
            PRINTVEC( mat[1], 2 );

            PRINTVBL( det );
            PRINTVEC( lambda, 2 );
            BLANKLINE;

            PAUSE;
         }

         // Store these
         triangleArea.push_back( area );
         triangleSigma.push_back( sigma );

         //maxArea = std::max( maxArea, area );
         //maxSigma = std::max( maxSigma, sigma );

         norm_num += sigma*sigma * area;
         norm_den += area;
      }

      const T stretchNorm = sqrt( norm_num / norm_den );
      
      vertexStretch.clear( );

      // Right, now we have the sigmas for triangles, need to convert them for vertices
      MESH_EACHVERTEX( m_FlatMesh, v )
      {
         // Get the edge coming into this vertex
         HalfEdge* e = (*vertices)[v]->getEdge( );
         const HalfEdge* estart = e;

         T num = 0;
         T den = 0;

         do
         {
            if ( e->hasFace( ) )
            {
               const MeshIndex face = e->getFace( );

               num += triangleArea[face] * triangleSigma[face] * triangleSigma[face];
               den += triangleArea[face];
            }
            
            e = e->getNext( )->getPair( );
         }
         while ( e != estart );

         const T sigma = pow( sqrt( num / den ), static_cast<T>( 0.5 ) ); 

         vertexStretch.push_back( sigma );
      }

      return stretchNorm;
   }

   void spreadVertices( )
   {
      // Need to go through the vertices in the x- and y-components
      // and Parzen window them

      // These will hold the marginalized pdfs
      const MeshIndex ngrid = 2000;
      const T gridspacing = static_cast<T>( 1 ) / static_cast<T>( ngrid );

      T grid[ngrid];
      T marg[2][ngrid];
      for ( MeshIndex i = 0; i < ngrid; ++i )
      {
         grid[i] = static_cast<T>( i ) / static_cast<T>( ngrid );
         marg[0][i] = marg[1][i] = 0;
      }

      // Specify a sigma2, then we know how far either side of the mean to go
      const T sigma2 = static_cast<T>( 1e-5 );
      const T eps    = static_cast<T>( 1e-4 );
      const int xlim = static_cast<int>( ceil( sqrt( -sigma2 * log( eps ) ) / gridspacing ) );

      // Calculate the unnormalized pdfs
      for ( MeshIndex v = 0; v < m_FlatMesh->getNumberOfVertices( ); ++v )
      {
         T vtx[3];
         m_FlatMesh->getVertex( v, vtx );

         for ( MeshIndex ax = 0; ax < 2; ++ax )
         {
            // Wrap into the range 0..1
            vtx[ax] = vtx[ax] - floor( vtx[ax] );

            int xlower = static_cast<int>( floor( vtx[ax] / gridspacing ) ) - xlim;
            int xupper = static_cast<int>( ceil( vtx[ax]  / gridspacing ) ) + xlim;

            for ( int x = xlower; x <= xupper; ++x )
            {
               MeshIndex xi = ( x + ngrid ) % ngrid;
               T delta = vtx[ax] - grid[xi];

               marg[ax][xi] += exp( -(delta*delta)/sigma2 );
            }
         }
      }

      // Now do a numerical integration to get the cumulative pdf
      // The cumulative pdf has one more point 
      // (integration on closed domain)
      T cumulmarg[2][ngrid+1];
      cumulmarg[0][0] = cumulmarg[1][0] = 0;

      for ( MeshIndex i = 1; i <= ngrid; ++i )
      {
         // Just use trapezium integration
         for ( MeshIndex ax = 0; ax < 2; ++ax )
            cumulmarg[ax][i] = cumulmarg[ax][i-1] + gridspacing * ( marg[ax][i-1] + marg[ax][i%ngrid] ) / 2;
      }

      // Now, normalize the cumulative sum
      for ( MeshIndex i = 1; i < ngrid; ++i )
      {
         for ( MeshIndex ax = 0; ax < 2; ++ax )
         {
            cumulmarg[ax][i] /= cumulmarg[ax][ngrid];
            marg[ax][i] /= cumulmarg[ax][ngrid];
         }
      }
      // Final step of normalization
      cumulmarg[0][ngrid] = cumulmarg[1][ngrid] = 1;

      // Now use this to remap the vertices
      for ( MeshIndex v = 0; v < m_FlatMesh->getNumberOfVertices( ); ++v )
      {
         T vtx[3];
         m_FlatMesh->getVertex( v, vtx );

         for ( MeshIndex ax = 0; ax < 2; ++ax )
         {
            T xoffset = floor( vtx[ax] );
            T xscale = ( vtx[ax] - xoffset ) / gridspacing;
            int xi   = static_cast<int>( floor( xscale ) );
            T delta  = xscale - floor( xscale );

            vtx[ax] = ( 1 - delta ) * cumulmarg[ax][xi] + delta * cumulmarg[ax][xi+1] + xoffset;
         }

         m_FlatMesh->setVertex( v, vtx );
      }
   }

   void createMeshCopy( )
   {  
      Mesh<T>::P mesh = m_Fg->getMesh( );
      m_FlatMesh = MeshImpl<T>::getInstance( mesh );

      T bounds[6];
      bounds[0] = bounds[2] = bounds[4] =  std::numeric_limits<T>::max( );
      bounds[1] = bounds[3] = bounds[5] = -std::numeric_limits<T>::max( );

      T vtx[3];
      //for ( MeshIndex v = 0; v < mesh->getNumberOfVertices( ); ++v )
      MESH_EACHVERTEX( mesh, v )
      {
         mesh->getVertex( v, vtx );

         for ( int ax = 0; ax < 3; ++ax )
         {
            bounds[2*ax+0] = std::min( bounds[2*ax+0], vtx[ax] );
            bounds[2*ax+1] = std::max( bounds[2*ax+1], vtx[ax] );
         }
      }

      /*
      m_FlattenScale = 0;

      for ( int ax = 0; ax < 3; ++ax )
      {
         m_FlattenScale = std::max( m_FlattenScale, bounds[2*ax+1]-bounds[2*ax+0] );
      }
      */
   }

   void sliceMesh( typename Mesh<T>::P mesh, std::set<HalfEdgeVertex*> contourX[2], std::set<HalfEdgeVertex*> neighboursX[2], const bool doshift )
   {
      BLANKLINE;
      DEBUGMACRO( "Slicing Mesh!" );

      std::set<MeshIndex> contourIdx[2];
      std::set<MeshIndex> neighbourIdx[2];

      for ( int cc = 0; cc < 2; ++cc )
      {
         std::set<HalfEdgeVertex*>::iterator it, en;
         
         it = contourX[cc].begin();
         en = contourX[cc].end();
         for ( ; it != en; ++it )
         {
            contourIdx[cc].insert( (*it)->getIndex( ) );
         }

         it = neighboursX[cc].begin();
         en = neighboursX[cc].end();
         for ( ; it != en; ++it )
         {
            neighbourIdx[cc].insert( (*it)->getIndex( ) );
         }
         //BLANKLINE;
      }

      for ( int cc = 0; cc < 2; ++cc )
      {
         // So... we need to create a copy of each vertex in the contour
         std::map<MeshIndex,MeshIndex> copyMap;

         std::set<MeshIndex>::iterator it = contourIdx[cc].begin();
         std::set<MeshIndex>::iterator en = contourIdx[cc].end();

         std::vector< HalfEdgeVertex* >* vertices = &m_Fg->getHalfEdgeMesh( )->getVertices( );

         // Create the new vertices
         T vtx[3];
         for ( ; it != en; ++it )
         {
            //MeshIndex v = (*it)->getIndex( );
            MeshIndex v = (*it);

            // Get the current vertex's location
            mesh->getVertex( v, vtx );

            const MeshIndex newVertexIndex = mesh->getNumberOfVertices( );
            
            // Create the new vertex
            mesh->setVertex( newVertexIndex, vtx );

            // Push into the map
            copyMap[ v ] = newVertexIndex;

            // Now, if this vertex is on the /other/ contour or the /other/ neighbours,
            // this duplicate needs to be recorded as also being on that contour/neighbours

            if ( contourIdx[1-cc].find( v ) != contourIdx[1-cc].end( ) )
               contourIdx[1-cc].insert( newVertexIndex );

            if ( neighbourIdx[1-cc].find( v ) != neighbourIdx[1-cc].end( ) )
               neighbourIdx[1-cc].insert( newVertexIndex );
         }

         // Repoint the faces
         MeshIndex vs[3];
         //for ( MeshIndex f = 0; f < mesh->getNumberOfFaces( ); ++f )
         MESH_EACHFACE( mesh, f )
         {
            mesh->getFaceIndices( f, vs );
         
            // Look to see if we need to repoint this face
            bool inContour = false;
            bool inNeighbours = false;

            for ( int i = 0; i < 3; ++i )
            {
               inContour    |= contourIdx[cc].find( vs[i] )   != contourIdx[cc].end( );
               inNeighbours |= neighbourIdx[cc].find( vs[i] ) != neighbourIdx[cc].end( );
            }

            // If we have points both in the contour and in the neighbours, we need to
            // repoint this triangle
            if ( inContour && inNeighbours )
            {
               for ( int i = 0; i < 3; ++i )
               {
                  std::map<MeshIndex,MeshIndex>::iterator ff = copyMap.find( vs[i] );

                  // Repoint the vertex if we've created a duplicate
                  if ( ff != copyMap.end( ) )
                  {
                     // Move the duplicate vertex to its correct position
                     mesh->getVertex( vs[i], vtx );

                     if ( doshift )
                        vtx[cc] += 1;

                     mesh->setVertex( (*ff).second, vtx );

                     // And now repoint the vertex
                     vs[i] = (*ff).second;
                  }
               }

               mesh->setFaceIndices( f, vs );
            }
         }
      }

      DEBUGMACRO( "Sliced" );
   }

   bool insideOut( )
   {
      // Count the number of backwards facing triangles in the
      // flattened mesh - if this is more than the number of 
      // forward facing ones (actually, one of the counts should
      // be zero!), then the mesh has been effectively flattened
      // inside out, and so we can call flip to sort this out

      T vtx[3][3];
      unsigned int countPlus  = 0;
      unsigned int countMinus = 0;
      MESH_EACHFACE( m_FlatMesh, f )
      {
         m_FlatMesh->getFace( f, vtx[0], vtx[1], vtx[2] );

         for ( unsigned int ax = 0; ax < 3; ++ax )
         {
            vtx[1][ax] -= vtx[0][ax];
            vtx[2][ax] -= vtx[0][ax];
         }

         cross( vtx[1], vtx[2], vtx[0] );

         if ( vtx[0][2] > 0 )
            ++countPlus;
         else
            ++countMinus;
      }

      PRINTVBL( countPlus  );
      PRINTVBL( countMinus );
      return countMinus > countPlus;
   }

   void flipMesh( )
   {
      T vtx[3];

      MESH_EACHVERTEX( m_FlatMesh, v )
      {
         m_FlatMesh->getVertex( v, vtx );
         vtx[0] = /*static_cast<T>( 0.5 )*/ - vtx[0];
         m_FlatMesh->setVertex( v, vtx );
      }
   }

   void flatten( )
   {
      // Get the contours
      const HalfEdgeVec contourA = m_Fg->getGenerator( 0, 0 );
      const HalfEdgeVec contourB = m_Fg->getGenerator( 0, 1 );

      // Then get the vertex sets
      findNeighbourSet( contourA, m_ContourVertices[0], m_NeighbourVertices[0] );
      findNeighbourSet( contourB, m_ContourVertices[1], m_NeighbourVertices[1] );
      
      //m_FixedPoint = contourA[0]->getVertex( )->getIndex( );

      // Pick a fixed point which is in neither the contours nor neighbours
      {
         std::set<MeshIndex> notAllowedToBeFixedPoint;

         typedef std::set< HalfEdgeVertex* > HalfEdgeVertexSet;
         HalfEdgeVertexSet* sets[] = {
            &m_ContourVertices[0], &m_ContourVertices[1], &m_NeighbourVertices[0], &m_NeighbourVertices[1]
         };

         for ( MeshIndex i = 0; i < 4; ++i )
         {
            for ( HalfEdgeVertexSet::iterator it = (*sets[i]).begin(), en = (*sets[i]).end(); it != en; ++it )
               notAllowedToBeFixedPoint.insert( (*it)->getIndex( ) );
         }

         do
         {
            m_FixedPoint = rand() % m_Fg->getMesh()->getNumberOfVertices();
            PRINTVBL( m_FixedPoint );
         }
         while ( notAllowedToBeFixedPoint.find( m_FixedPoint ) != notAllowedToBeFixedPoint.end() );

      }

      createMeshCopy( );

      solve( );

      sliceMesh( m_FlatMesh, m_ContourVertices, m_NeighbourVertices, true );

      // Translate so that the crossing point is at (0,0)
      T vtxTrans[3];
      m_FlatMesh->getVertex( contourA[0]->getVertex( )->getIndex( ), vtxTrans );
      MESH_EACHVERTEX( m_FlatMesh, v )
      {
         T vtx[3];
         m_FlatMesh->getVertex( v, vtx );

         FOREACHAXIS( ax )
            vtx[ax] -= vtxTrans[ax];

         m_FlatMesh->setVertex( v, vtx );
      }

      // Create a copy of the input mesh and slice it
      m_SlicedMesh = MeshImpl<T>::getInstance( m_Fg->getMesh( ) );
      sliceMesh( m_SlicedMesh, m_ContourVertices, m_NeighbourVertices, false );

      // Now copy the flat mesh coordinates into the texture coordinates of the sliced mesh
      Tuples<T>::P texcoords = TuplesImpl<T>::getInstance( 2, m_FlatMesh->getNumberOfVertices( ) );
      T tc[3];
      MESH_EACHVERTEX( m_FlatMesh, v )
      {
         m_FlatMesh->getVertex( v, tc );
         texcoords->setPoint( v, tc );
      }
      m_SlicedMesh->setTextureCoords( texcoords );

      DEBUGMACRO( "Finished flattening" );
   }

   std::set< HalfEdgeVertex* >               m_ContourVertices[2];
   std::set< HalfEdgeVertex* >               m_NeighbourVertices[2];

   // Create mesh searchers
   typename FacesRayIntersectionSearch<T>::P m_RaySearcher;
   typename FacesNearestPointSearch<T>::P    m_NearestSearcher;

   MeshIndex                                 m_FixedPoint;

   typename GeneratorProvider<T>::P          m_Fg;
   typename Mesh<T>::P                       m_FlatMesh;
   typename Mesh<T>::P                       m_SlicedMesh;

   T                                         m_FlattenScale;

   WeightType m_WeightType;
   SpreadType m_SpreadType;
};


template <class T>
typename AWT::FlattenMesh<T>::WeightType AWT::FlattenMesh<T>::getWeightType( const char* wstr )
{
   if ( strcmp( wstr, "floater" ) == 0 )
   {
      return FlattenMesh<T>::WEIGHT_FLOATER;
   }
   else if ( strcmp( wstr, "harmonic" ) == 0 )
   {
      return FlattenMesh<T>::WEIGHT_HARMONIC;
   }
   else if ( strcmp( wstr, "tutte" ) == 0 )
   {
      return FlattenMesh<T>::WEIGHT_TUTTE;
   }
   else if ( strcmp( wstr, "wachspress" ) == 0 )
   {
      return FlattenMesh<T>::WEIGHT_WACHSPRESS;
   }

   DEBUGMACRO( "Specified spread type not recognised; assuming tutte" );
   return FlattenMesh<T>::WEIGHT_TUTTE;
}

template <class T>
typename AWT::FlattenMesh<T>::SpreadType AWT::FlattenMesh<T>::getSpreadType( const char* wstr )
{
   if ( strcmp( wstr, "none" ) == 0 )
   {
      return FlattenMesh<T>::SPREAD_NONE;
   }
   else if ( strcmp( wstr, "yoshizawa" ) == 0 )
   {
      return FlattenMesh<T>::SPREAD_YOSHIZAWA;
   }
   else if ( strcmp( wstr, "awt" ) == 0 )
   {
      return FlattenMesh<T>::SPREAD_AWT;
   }

   DEBUGMACRO( "Specified weight type not recognised; assuming yoshizawa" );
   return FlattenMesh<T>::SPREAD_YOSHIZAWA;
}

template <class T>
AWT::FlattenMesh<T>::FlattenMesh( )
{
}

template <class T>
AWT::FlattenMesh<T>::~FlattenMesh( )
{
}

template <class T>
typename AWT::FlattenMeshPair<T>::P AWT::FlattenMesh<T>::flattenToPair( typename GeneratorProvider<T>::P fg, const WeightType wt, const SpreadType st )
{
   D* m_D = new D;

   m_D->m_Fg         = fg;
   m_D->m_WeightType = wt;
   m_D->m_SpreadType = st;

   m_D->m_RaySearcher     = FacesRayIntersectionSearch<T>::getInstance( );
   m_D->m_NearestSearcher = FacesNearestPointSearch<T>::getInstance( );

   m_D->flatten( );

   /*
   const bool iio = m_D->insideOut( );
   PRINTBOOL( iio );

   if ( iio ) m_D->flipMesh( );
   */

   return FlattenMeshPair<T>::getInstance( m_D->m_SlicedMesh, m_D->m_FlatMesh );
}

template <class T>
typename AWT::Mesh<T>::P AWT::FlattenMesh<T>::flatten( typename GeneratorProvider<T>::P fg, const WeightType wt, const SpreadType st )
{
   D* m_D = new D;

   m_D->m_Fg         = fg;
   m_D->m_WeightType = wt;
   m_D->m_SpreadType = st;

   m_D->m_RaySearcher     = FacesRayIntersectionSearch<T>::getInstance( );
   m_D->m_NearestSearcher = FacesNearestPointSearch<T>::getInstance( );

   m_D->flatten( );

   /*
   const bool iio = m_D->insideOut( );
   PRINTBOOL( iio );

   if ( iio ) m_D->flipMesh( );
   */

   return m_D->m_SlicedMesh;
}

template class AWT::FlattenMesh<double>;