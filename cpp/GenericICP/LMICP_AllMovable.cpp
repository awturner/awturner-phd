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
#include "LMICP_AllMovable.h"

#include <vector>
#include "Useful/ArrayVectorFunctions.h"
#include "vnl/vnl_vector_fixed.h"

#include "Mesh/MeshFunctions.h"
#include "Mesh/MeshImpl.h"

using namespace AWT;

struct AWT::LMICP_AllMovable::D
{
   struct Correspondence
   {
      MeshIndex psA;
      MeshIndex pntA;
      MeshIndex psB;
      MeshIndex pntB;
   };

   std::vector< Tuples<double>::P > m_PointSets;
   std::vector< Correspondence > m_Correspondences;

   bool m_Debug;
};

AWT::LMICP_AllMovable::LMICP_AllMovable( const unsigned int nparams, const unsigned int nresiduals )
: vnl_least_squares_function( nparams, nresiduals, vnl_least_squares_function::no_gradient )
{
   m_D = new D;

   m_D->m_Debug = false;
}

AWT::LMICP_AllMovable::~LMICP_AllMovable( )
{
   delete m_D;
}

AWT::LMICP_AllMovable::P AWT::LMICP_AllMovable::getInstance( const unsigned int nparams, const unsigned int nresiduals )
{
   AUTOGETINSTANCE( AWT::LMICP_AllMovable, ( nparams, nresiduals ) );
}

GETNAMEMACRO( AWT::LMICP_AllMovable );

void AWT::LMICP_AllMovable::addPoints( Tuples<double>::P pointSet )
{
   m_D->m_PointSets.push_back( pointSet );
}

void AWT::LMICP_AllMovable::addCorrespondence( const MeshIndex psA, const MeshIndex pntA, const MeshIndex psB, const MeshIndex pntB )
{
   D::Correspondence corr;
   corr.psA  = psA;
   corr.pntA = pntA;
   corr.psB  = psB;
   corr.pntB = pntB;

   m_D->m_Correspondences.push_back( corr );
}

void AWT::LMICP_AllMovable::f( const vnl_vector<double>& params, vnl_vector<double>& residuals )
{
   vnl_vector_fixed<double,4> vtxA, vtxB;
   double centroid[3];

   typedef std::vector<Tuples<double>::P>::iterator TIt;
   typedef std::vector<D::Correspondence>::iterator It;

   std::vector< vnl_matrix_fixed<double,4,4> > ts;

   // Identity transform for first mesh
   {
      centroid[0] = centroid[1] = centroid[2] = 0;

      vnl_vector<double> ps( 7 );
      ps.fill( 0 );
      ps[0] = 1;
      ts.push_back( LMICP::makeTransformation( ps, centroid ) );
   }

   MeshIndex i;

   i = 0;
   for ( TIt it = m_D->m_PointSets.begin() + 1, en = m_D->m_PointSets.end(); it != en; ++it, ++i )
   {
      // Create a temporary mesh to easily calculate the centroid of the point set
      Mesh<double>::P mesh = MeshImpl<double>::getInstance( 0, 0 );
      mesh->setVertices( *it );
      MeshFunctions<double>::calculateVertexCentroid( mesh, centroid );
      
      vnl_vector<double> ps = vectorslice( params, 7*i, 7 );
      ts.push_back( LMICP::makeTransformation( ps, centroid ) );

      if ( m_D->m_Debug )
      {
         PRINTVBL( ps );
         PRINTVBL( LMICP::makeTransformation( ps, centroid ) );
      }
   }
   //PAUSE;

   i = 0;
   for ( It it = m_D->m_Correspondences.begin(), en = m_D->m_Correspondences.end(); it != en; ++it, ++i )
   {
      // Get the points
      m_D->m_PointSets[ it->psA ]->getPoint( it->pntA, vtxA.data_block() ); vtxA[3] = 1;
      m_D->m_PointSets[ it->psB ]->getPoint( it->pntB, vtxB.data_block() ); vtxB[3] = 1;

      // Transform them
      vtxA = ts[ it->psA ] * vtxA;
      vtxB = ts[ it->psB ] * vtxB;

      // Calculate the magnitude difference
      residuals( i ) = deltaNorm( vtxA.data_block( ), vtxB.data_block( ), 3 );
   }
}

void LMICP_AllMovable::debugOn( )
{
   m_D->m_Debug = true;
}

void LMICP_AllMovable::debugOff( )
{
   m_D->m_Debug = false;
}
