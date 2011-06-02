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
#include "HalfEdgeMesh.h"

#include "HalfEdgeVertex.h"
#include "HalfEdge.h"

#include "Useful/Exception.h"

#include <vector>
#include <limits>

#include <set>
#include <fstream>
#include <sstream>

struct AWT::HalfEdgeMesh::D
{
   D( )
   {
      for ( MeshIndex i = 0; i < 5; ++i )
         counts[i] = 0;
   }

   ~D( )
   {
      for ( std::vector<HalfEdgeVertex*>::iterator it = m_Vertices.begin( ); it != m_Vertices.end( ); ++it )
         delete *it;

      for ( std::vector<HalfEdge*>::iterator it = m_Edges.begin( ); it != m_Edges.end( ); ++it )
         delete *it;
   }

   HalfEdge* createEdge( const MeshIndex v )
   {
      HalfEdge* ret = new HalfEdge();
      ret->setVertex( m_Vertices[v] );

      m_Edges.push_back( ret );

      // If this is the first edge to point to this vertex, set the vertex's edge to this edge
      if ( m_Vertices[v]->getEdge( ) == 0 )
         m_Vertices[v]->setEdge( ret );

      return ret;
   }

   void ensureVertices( const MeshIndex* vs )
   {
      // This makes sure that there is a vertex for each of the vertex indices that we are trying to add
      for ( int i = 0; i < 3; ++i )
         while ( m_Vertices.size( ) <= vs[i] )
            m_Vertices.push_back( new HalfEdgeVertex( static_cast<MeshIndex>( m_Vertices.size( ) ) ) );
   }

   HalfEdge* findEdge( const MeshIndex from, const MeshIndex to )
   {
      // This finds the edge between vertex from and vertex to, if it exists

      HalfEdge* start = m_Vertices[from]->getEdge( );

      if ( start != 0 )
      {
         // At the moment, start is an incoming edge; we want an outgoing edge
         start = start->getPair( );

         HalfEdge* edge = start;

         do
         {
            if ( edge->getVertex( )->getIndex( ) == to )
            {
               //std::cerr << "Edge from " << from << " to " << to << " = " << edge << std::endl;
               return edge;
            }


            edge = edge->getPair( )->getNext( );
         }
         while ( edge != start );
      }

      //std::cerr << "Edge from " << from << " to " << to << " not found." << std::endl;
      return 0;
   }

   HalfEdge* join( HalfEdge* a, HalfEdge* c )
   {
      HalfEdge* e = createEdge( c->getVertex( )->getIndex( ) );
      HalfEdge* f = createEdge( a->getVertex( )->getIndex( ) );

      e->setPair( f );
      f->setPair( e );

      // Plumb these into the mesh
      e->setNext( c->getNext( ) );
      f->setNext( a->getNext( ) );
      a->setNext( e );
      c->setNext( f );

      return e;
   }

   void printEdgeNoDups( std::ostream& os, std::set< HalfEdge* >& previous, HalfEdge* edge )
   {
      if ( previous.find( edge ) == previous.end( ) )
      {
         os << *edge << std::endl;
         previous.insert( edge );
      }
   }

   void printVertexNoDups( std::ostream& os, std::set< HalfEdge* >& previous, HalfEdgeVertex* vertex )
   {
      HalfEdge* edge = vertex->getEdge( );
      HalfEdge* start = edge;

      do
      {
         if ( edge->getNext( )->getNext( )->getNext( ) == edge )
         {
            printEdgeNoDups( os, previous, edge->getNext( ) );
            printEdgeNoDups( os, previous, edge->getNext( )->getPair( ) );
         }

         printEdgeNoDups( os, previous, edge );
         edge = edge->getNext( );

         if ( edge->getNext( )->getNext( )->getNext( ) == edge )
         {
            printEdgeNoDups( os, previous, edge->getNext( ) );
            printEdgeNoDups( os, previous, edge->getNext( )->getPair( ) );
         }

         printEdgeNoDups( os, previous, edge );
         edge = edge->getPair( );

         
      }
      while ( edge != start );
   }

   void printVertex( HalfEdgeVertex* v )
   {
      std::ofstream fs( "aroundmerge.gv" );
      fs << "digraph aroundmerge { " << std::endl;
      fs << "overlap=scale" << std::endl;

      std::set< HalfEdge* > xx;
      printVertexNoDups( fs, xx, v );

      fs << "}" << std::endl;
      system( "neato -Lg -Tsvg -oaroundmerge.svg aroundmerge.gv" );
      DEBUGLINE;
      PAUSE;
   }

   HalfEdge* findIncomingFacelessEdge( AWT::HalfEdgeVertex* v )
   {
      // getEdge is an incoming edge
      HalfEdge* start = v->getEdge( );
      HalfEdge* e = start;

      do
      {
         if ( !e->hasFace( ) )
            return e;

         e = e->getNext( )->getPair( );
      }
      while ( e != start );

      AWTEXCEPTIONTHROW( "Could not find edge" );
   }

   MeshIndex getNewContourLabel( )
   {
      MeshIndex i;

      for ( i = 0; i < m_ContourCounts.size( ); ++i )
         if ( m_ContourCounts[i] == 0 )
            break;

      if ( i == m_ContourCounts.size() )
         m_ContourCounts.push_back( 0 );

      return i;
   }

   void relabel( HalfEdge* e, const MeshIndex v )
   {
      if ( e->getLabel( ) != std::numeric_limits<MeshIndex>::max( ) )
      {
         // Decrement the number of segments in the old contour
         --m_ContourCounts[ e->getLabel( ) ];
      }

      if ( v != std::numeric_limits<MeshIndex>::max( ) )
      {
         // Make sure that we don't get an array index error
         while ( m_ContourCounts.size( ) < v )
            m_ContourCounts.push_back( 0 );

         // Now increment the number of segments in the contour
         ++m_ContourCounts[ v ];
      }

      // Finally, actually set the label
      e->setLabel( v );
   }

   void labelLoop( HalfEdge* e, const MeshIndex v )
   {
      HalfEdge* start = e;

      do
      {
         relabel( e, v );

         e = e->getNext( );
      }
      while ( e != start );
   }

   bool canAddTriangle( const AWT::MeshIndex* vs )
   {
      // We need to check for pre-existing bow ties at each of the vertices
      // - if this triangle will create a "triple" bow-tie, we can't add it

      // Make sure that we have sufficient vertices
      ensureVertices( vs );

      HalfEdge* edges[] = {
         findEdge( vs[0], vs[1] ),
         findEdge( vs[1], vs[2] ),
         findEdge( vs[2], vs[0] )
      };

      // Count how many of the edges already exist
      unsigned int nullCount = 0;
      unsigned int firstNull    = 3;
      unsigned int firstNotNull = 3;
      for ( int i = 0; i < 3; ++i )
      {
         if ( !edges[i] )
         {
            ++nullCount;

            if ( firstNull == 3 )
               firstNull = i;
         }
         else
         {
            if ( firstNotNull == 3 )
               firstNotNull = i;

            if ( edges[i]->hasFace( ) )
            {
               std::stringstream sstr;
               sstr << "Edge (" << vs[i] << "," << vs[(i+1)%3] << ") already has a face!";
               AWTEXCEPTIONTHROW( sstr.str( ) );
            }
         }
      }

      if ( nullCount == 2 )
      {
         MeshIndex distalVertex = vs[ (firstNotNull+2) % 3 ];
         HalfEdgeVertex* vdist = m_Vertices[ distalVertex ];

         if ( vdist->getEdge( ) != 0 )
         {
            // Right, we've managed to haul ourselves to the distal vertex...
            // the question is: is this already a bow tie?

            // To do this, count the number of outgoing edges without a face
            // If this is 2, then it's a bow tie, and I don't want to do this

            HalfEdge* start = vdist->getEdge( )->getPair( );
            HalfEdge* e     = start;

            MeshIndex outCount = 0;

            do
            {
               if ( !e->hasFace( ) )
                  ++outCount;

               e = e->getPair( )->getNext( );
            }
            while ( e != start );

            if ( outCount == 2 )
               return false;
         }

         return true;
      }

      return true;
   }

   void addTriangle( const AWT::MeshIndex* vs, const MeshIndex faceIndex, bool& isMerge, AWT::MeshIndex& mergePoint )
   {
      const MeshIndex UNLABELLED = std::numeric_limits<MeshIndex>::max( );
      const MeshIndex oldNumberOfContours = getNumberOfContours( );

      // Make sure that we have sufficient vertices
      ensureVertices( vs );

      HalfEdge* edges[] = {
         findEdge( vs[0], vs[1] ),
         findEdge( vs[1], vs[2] ),
         findEdge( vs[2], vs[0] )
      };

      // So... we now know which edges exist.
      // We assume that this mesh is going to be generated by an
      // edgebreaker-like process, so we don't get bow-ties

      // Count how many of the edges already exist
      unsigned int nullCount = 0;
      unsigned int firstNull    = 3;
      unsigned int firstNotNull = 3;
      for ( int i = 0; i < 3; ++i )
      {
         if ( !edges[i] )
         {
            ++nullCount;

            if ( firstNull == 3 )
               firstNull = i;
         }
         else
         {
            if ( firstNotNull == 3 )
               firstNotNull = i;

            if ( edges[i]->hasFace( ) )
               AWTEXCEPTIONTHROW( "Edge already has face!" );
         }
      }

      // In general, this won't be changed again - only in the case of 2 unvisited.
      isMerge    = false;
      mergePoint = std::numeric_limits<MeshIndex>::max( );

      switch ( nullCount )
      {
      case 0:
         // We don't need to create any edges, since they all exist already
         //DEBUGMACRO( "Case 0" );

         // Incrementally update the labelling
         ++counts[0];

         break;

      case 1:
         {
            ++counts[1];

            // We just need to insert the one edge
            // and we can only do this in one place
            
            //DEBUGMACRO( "Case 1" );
            HalfEdge* newEdge = join( edges[(firstNull+2)%3], edges[(firstNull+1)%3]->getPrevious( ) );

            relabel( newEdge->getPair( ), edges[firstNotNull]->getLabel( ) );
         }

         break;

      case 2:
         {
            // This is slightly complicated because we need to insert in the correct order
            // and we act differently depending on whether the distal vertex is visited or not

            // The distal vertex is the target of the edge after the non-null edge
            // We need to know if this vertex has already been visited, because
            // we need to handle the two cases differently

            MeshIndex distalVertex = vs[ (firstNotNull+2) % 3 ];
            HalfEdgeVertex* vdist = m_Vertices[ distalVertex ];

            if ( vdist->getEdge( ) == 0 )
            {
               ++counts[2];
               //DEBUGMACRO( "Case 2 Unvisited" );

               // This vertex has no edges, so it hasn't been visited

               // Create the first pair of edges
               HalfEdge* e = createEdge( distalVertex );
               HalfEdge* f = createEdge( edges[firstNotNull]->getVertex( )->getIndex( ) );

               e->setPair( f );
               f->setPair( e );

               e->setNext( f );
               f->setNext( edges[firstNotNull]->getNext( ) );

               edges[firstNotNull]->setNext( e );

               // Now, insert the other edge
               join( edges[firstNotNull]->getPrevious( ), e );

               // Incrementally update the labelling
               relabel( edges[firstNotNull]->getNext( )->getPair( ), edges[firstNotNull]->getLabel( ) );
               relabel( edges[firstNotNull]->getNext( )->getNext( )->getPair( ), edges[firstNotNull]->getLabel( ) );
            }
            else
            {
               ++counts[3];
               //DEBUGMACRO( "Case 2 Visited" );

               // This vertex has at least one edge, so it has been visited
               // This edge is an incoming edge

               HalfEdge* es[3];

               // Look for the face-less edge incoming to vs[2]
               //es[0] = findIncomingFacelessEdge( m_Vertices[ vs[(firstNotNull+0)%3] ] );
               //es[1] = findIncomingFacelessEdge( m_Vertices[ vs[(firstNotNull+1)%3] ] );
               //es[2] = findIncomingFacelessEdge( m_Vertices[ vs[(firstNotNull+2)%3] ] );
               es[0] = edges[firstNotNull]->getPrevious( );
               es[1] = edges[firstNotNull];

               try
               {
                  es[2] = findIncomingFacelessEdge( m_Vertices[ vs[(firstNotNull+2)%3] ] );
               }
               catch ( AWT::Exception ex )
               {
                  throw ex;
               }
               
               join( es[2], es[0] );
               join( es[1], es[2] );

               // We've got a merge or a split here
               isMerge    = true;
               mergePoint = distalVertex;

               // We know that we will use at most two contours here, so let's get indices for them
               // If we don't use them, we'll give 'em back
               std::vector<MeshIndex> newLabels;

               // Go to the distal vertex and find all of the outgoing edges without faces
               HalfEdge* start = vdist->getEdge( )->getPair( );
               HalfEdge* e = start;

               do
               {
                  if ( !e->hasFace( ) )
                  {
                     // Make sure that the label on this edge isn't one of our new labels
                     bool isDifferent = true;

                     for ( MeshIndex i = 0; i < newLabels.size( ) && isDifferent; ++i )
                     {
                        if ( e->getLabel( ) == newLabels[i] )
                           isDifferent = false;
                     }

                     if ( isDifferent )
                     {
                        MeshIndex newLabel = getNewContourLabel( );
                        newLabels.push_back( newLabel );

                        labelLoop( e, newLabel );
                     }
                  }

                  e = e->getPair( )->getNext( );
               }
               while ( start != e );
            }
         }

         break;

      case 3:
         {
            ++counts[4];
            //DEBUGMACRO( "Case 3 Unvisited" );

            // If all of the edges are null, then we need to start a new contour
            HalfEdge* fwd[3];
            HalfEdge* bwd[3];

            fwd[0] = createEdge( vs[1] );
            fwd[1] = createEdge( vs[2] );
            fwd[2] = createEdge( vs[0] );

            bwd[0] = createEdge( vs[0] );
            bwd[1] = createEdge( vs[1] );
            bwd[2] = createEdge( vs[2] );

            MeshIndex newLabel = getNewContourLabel( );

            for ( int i = 0; i < 3; ++i )
            {
               fwd[i]->setNext( fwd[(i+1)%3] );
               bwd[i]->setNext( bwd[(i+2)%3] );

               fwd[i]->setPair( bwd[i] );
               bwd[i]->setPair( fwd[i] );

               relabel( bwd[i], newLabel );
            }
         }

         break;
      }

      // Find any newly created edges
      // and flag all of these edges as having a face
      for ( int i = 0; i < 3; ++i )
      {
         if ( edges[i] == 0 )
            edges[i] = findEdge( vs[i], vs[(i+1)%3] );

         edges[i]->setFace( faceIndex );
         relabel( edges[i], UNLABELLED );
      }

      // At the moment, isMerge is true for both splits and merges
      // Need to check this by comparing with previous number of contours
      // - if it has decreased, we had a merge event
      isMerge &= getNumberOfContours( ) < oldNumberOfContours;
   }

   MeshIndex getNumberOfContours( ) const
   {
      MeshIndex ret = 0;

      for ( size_t i = 0; i < m_ContourCounts.size( ); ++i )
         if ( m_ContourCounts[i] > 0 )
            ++ret;

      return ret;
   }

   void print( std::ostream& os )
   {
      halfEdgeVerbose = true;
      os << "digraph halfedge {" << std::endl;
      os << "  overlap=scale" << std::endl;
      os << std::endl;
      for ( std::vector<HalfEdge*>::iterator it = m_Edges.begin( ); it != m_Edges.end( ); ++it )
      {
         os << *(*it) << std::endl;
      }
      os << "}" << std::endl;

      halfEdgeVerbose = false;
   }

   std::vector<HalfEdgeVertex*> m_Vertices;
   std::vector<HalfEdge*>       m_Edges;

   std::vector<MeshIndex>       m_ContourCounts;

   MeshIndex counts[5];
};

AWT::HalfEdgeMesh::HalfEdgeMesh( )
{
   m_D = new D;
}

AWT::HalfEdgeMesh::~HalfEdgeMesh( )
{
   delete m_D;
}

AWT::HalfEdgeMesh::P AWT::HalfEdgeMesh::getInstance( )
{
   AUTOGETINSTANCE( AWT::HalfEdgeMesh, ( ) );
}

GETNAMEMACRO( AWT::HalfEdgeMesh );

void AWT::HalfEdgeMesh::addTriangle( const AWT::MeshIndex* vs, const AWT::MeshIndex faceIndex, bool& isMerge, MeshIndex& mergePoint )
{
   return m_D->addTriangle( vs, faceIndex, isMerge, mergePoint );
}

bool AWT::HalfEdgeMesh::canAddTriangle( const AWT::MeshIndex* vs )
{
   return m_D->canAddTriangle( vs );
}

AWT::MeshIndex AWT::HalfEdgeMesh::getNumberOfContours( )
{
   return m_D->getNumberOfContours( );
}

void AWT::HalfEdgeMesh::print(std::ostream &os)
{
   m_D->print( os );
}

std::vector< AWT::HalfEdgeVertex* >& AWT::HalfEdgeMesh::getVertices()
{
   return m_D->m_Vertices;
}

std::vector< AWT::HalfEdge* >& AWT::HalfEdgeMesh::getEdges()
{
   return m_D->m_Edges;
}

void AWT::HalfEdgeMesh::ensureVertices(const MeshIndex v)
{
   MeshIndex vs[] = { v-1, v-1, v-1 };

   m_D->ensureVertices( vs );
}

void AWT::HalfEdgeMesh::getCounts( MeshIndex* counts ) const
{
   for ( MeshIndex i = 0; i < 5; ++i )
      counts[i] = m_D->counts[i];
}