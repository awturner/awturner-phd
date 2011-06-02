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
#include "VisitedFaceSearchFilter.h"

#include "Connectivity.h"
#include "MeshIO.h"
#include "FacesInRadius.h"
#include "SimpleMesh/Search.h"
#include "Useful/ArrayVectorFunctions.h"
#include "MeshSearching/BoxQueries.h"
#include "MeshSearching/FaceQueries.h"
#include "MeshFunctions.h"

#include <fstream>

struct AWT::SimpleMesh::VisitedFaceSearchFilter::D
{
   enum VisitedState { UNVISITED = 0, VISITED_NOTMARKED = 1, VISITED = 2 };

   Connectivity::P conn;
   VisitedState* visited;
   Index nf;

   Search::P searcher;

   typedef std::pair<Index,Index> FaceTraversePair;

   void enqueueAdjacentFaces( std::queue<FaceTraversePair>& queue, Index i, Index depth )
   {
      Face face = conn->getMesh()->getFace(i);

      for ( Index vi = 0; vi < 3; ++vi )
      {
         Connectivity::IndexIterator::P it = conn->getFacesUsingVertex( face[vi] );

         while ( it->hasNext() )
            queue.push(FaceTraversePair(it->next(),depth));
      }
   }
};

AWT::SimpleMesh::VisitedFaceSearchFilter::VisitedFaceSearchFilter( Mesh::P mesh )
{
   m_D = new D;
   m_D->conn = Connectivity::getInstance( mesh );
   m_D->nf   = m_D->conn->getMesh( )->nf;

   m_D->visited = new D::VisitedState[ mesh->nf ];

   m_D->searcher = Search::getInstance( mesh, true );

   reset( );
}

AWT::SimpleMesh::VisitedFaceSearchFilter::~VisitedFaceSearchFilter( )
{
   delete [] m_D->visited;
   delete m_D;
}

AWT::SimpleMesh::VisitedFaceSearchFilter::P AWT::SimpleMesh::VisitedFaceSearchFilter::getInstance( AWT::SimpleMesh::Mesh::P mesh )
{
   AUTOGETINSTANCE( AWT::SimpleMesh::VisitedFaceSearchFilter, ( mesh ) );
}

GETNAMEMACRO( AWT::SimpleMesh::VisitedFaceSearchFilter );

void AWT::SimpleMesh::VisitedFaceSearchFilter::reset( )
{
   for ( Index i = 0, imax = m_D->nf; i < imax; ++i )
      m_D->visited[i] = D::UNVISITED;
}

bool AWT::SimpleMesh::VisitedFaceSearchFilter::check( const Point p, const Index i ) const
{
   return m_D->visited[i] != D::VISITED;
}

void AWT::SimpleMesh::VisitedFaceSearchFilter::accept( const Point p, const Index i )
{
   if ( i == INVALID_INDEX )
      return;

   // Flag the face and all adjacent faces as having been visited
   // The faces can still be found until markFaces( ) is called
   m_D->visited[i] = D::VISITED_NOTMARKED;

   return;

   Mesh::P mesh = m_D->conn->getMesh( );
   Face face = mesh->getFace( i );
   
   /*
   // Use the centre of this face as the start point
   Point centroid;
   centroid.fill(0);
   for ( Index ax = 0; ax < 3; ++ax )
      centroid += mesh->getVertex( face(ax) );
   centroid /= 3;

   FacesInRadius::P fir = FacesInRadius::getInstance();

   fir->reset();
   fir->setTestPoint( centroid );
   fir->setTestRadius( 5 );

   m_D->searcher->search(fir);

   std::vector<PointIndexWeights>& found = fir->getFaces();
   std::vector<PointIndexWeights>::iterator it = found.begin();
   std::vector<PointIndexWeights>::iterator en = found.end();

   while ( it != en )
   {
      Index ni = it->i;

      if ( m_D->visited[ ni ] == D::UNVISITED )
      {
         m_D->visited[ ni ] = D::VISITED_NOTMARKED;
      }

      ++it;
   }
   */

   std::queue<D::FaceTraversePair> queue;
   m_D->visited[i] = D::VISITED_NOTMARKED;
   m_D->enqueueAdjacentFaces(queue,i,2);

   while (!queue.empty())
   {
      D::FaceTraversePair ii = queue.front();
      queue.pop();

      if ( m_D->visited[ii.first] == D::UNVISITED )
      {
         m_D->visited[ii.first] = D::VISITED_NOTMARKED;

         if (ii.second > 0)
            m_D->enqueueAdjacentFaces(queue,ii.first,ii.second-1);
      }
   }

   /*
   for ( Index vi = 0; vi < 3; ++vi )
   {
      Connectivity::IndexIterator::P it = m_D->conn->getFacesUsingVertex( face[vi] );

      while ( it->hasNext() )
      {
         Index ni = it->next();

         if ( m_D->visited[ ni ] == D::UNVISITED )
         {
            m_D->visited[ ni ] = D::VISITED_NOTMARKED;
         }
      }
   }
   */
}

void AWT::SimpleMesh::VisitedFaceSearchFilter::markFaces( )
{
   return;
   for ( Index i = 0, imax = m_D->nf; i < imax; ++i )
   {
      if ( m_D->visited[i] == D::VISITED_NOTMARKED )
      {
         m_D->visited[i] = D::VISITED;
      }
   }
}

void AWT::SimpleMesh::VisitedFaceSearchFilter::writeUnvisitedMesh( const char* filename )
{
   Index nunvisited = 0;

   // Count the number of unvisited faces
   for ( Index i = 0, imax = m_D->nf; i < imax; ++i )
      nunvisited += ( m_D->visited[i] == D::UNVISITED ) ? 1 : 0;

   Mesh::P mesh = m_D->conn->getMesh();
   Mesh::P newMesh = Mesh::getInstance(mesh->nv, nunvisited);
   
   // Copy all vertices
   newMesh->getVertices().update( mesh->getVertices() );

   Index f = 0;
   for ( Index i = 0, imax = m_D->nf; i < imax; ++i )
   {
      if ( m_D->visited[i] == D::UNVISITED )
         newMesh->getFaces().set_column( f++, mesh->getFaces().get_column(i) );
   }

   std::ofstream os( filename );
   MeshIO::saveMesh( os, newMesh );
}