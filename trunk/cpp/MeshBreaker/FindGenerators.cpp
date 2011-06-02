#include "FindGenerators.h"

#include "Mesh/MeshConnectivity.h"
#include "Mesh/TuplesImpl.h"

#include "Useful/ProfTimer.h"
#include "Useful/Noise.h"

#include "HalfEdgeMesh.h"
#include "FlattenMesh.h"

#include "HalfEdge.h"
#include "HalfEdgeVertex.h"

#include <vector>
#include <set>
#include <limits>
#include <queue>
#include <fstream>
#include <map>

#include <ctime>

template <class T>
struct AWT::FindGenerators<T>::D
{
   typedef TuplesImpl<MeshIndex> IndexTuples;

   typedef std::vector<HalfEdge*>                HalfEdgeVec;
   typedef std::pair< HalfEdgeVec, HalfEdgeVec > HalfEdgeVecPair;
   typedef std::vector<HalfEdgeVecPair>          HalfEdgeVecPairVec;

   typedef std::pair<T,MeshIndex> QueuePair;
   std::priority_queue<QueuePair> m_FaceQueue;
   //std::vector<MeshIndex>          m_FaceQueue;

   void prepareTraversal( MeshIndex from )
   {
      m_Conn = MeshConnectivity<T>::getInstance( m_Mesh );

      // Flag all faces as unvisited
      m_FacesVisited.clear( );
      //for ( MeshIndex f = 0; f < m_Mesh->getNumberOfFaces( ); ++f )
      MESH_EACHFACE( m_Mesh, f )
         m_FacesVisited.push_back( false );

      // Make sure that we have just the starting face in the queue for now
      while ( !m_FaceQueue.empty() ) m_FaceQueue.pop();
      enqueFace( from );

      m_HalfEdgeMesh = HalfEdgeMesh::getInstance( );
      m_HalfEdgeMesh->ensureVertices( m_Mesh->getNumberOfVertices( ) );
   }

   void enqueFace( const MeshIndex f )
   {
      static T priority = 0;

      T thisPriority;

      switch ( traversalOrder )
      {
      case TRAVERSE_RANDOM:
         thisPriority = Noise<T>::randu();
         break;
      case TRAVERSE_DEPTH:
         thisPriority = priority += 1;
         break;
      case TRAVERSE_BREADTH:
         thisPriority = priority -= 1;
         break;
      }

      m_FaceQueue.push( QueuePair( thisPriority, f ) );
   }

   MeshIndex deque( )
   {
      while ( !m_FaceQueue.empty() && m_FacesVisited[ m_FaceQueue.top().second ] )
         m_FaceQueue.pop();

      if ( m_FaceQueue.empty() )
         return std::numeric_limits<MeshIndex>::max( );

      MeshIndex ret = m_FaceQueue.top().second;
      m_FaceQueue.pop();

      return ret;
   }

   unsigned int m_NSteps;

   bool stepTraversal( )
   {
      const MeshIndex face = deque( );

      if ( face == std::numeric_limits<MeshIndex>::max( ) )
         return false;

      // Mark the face to say that we've been here already
      m_FacesVisited[ face ] = true;

      // Now... we need to do the clever bit of adding this into the half-edge mesh
      MeshIndex vs[3];
      m_Mesh->getFaceIndices( face, vs );

      bool isMerge;
      MeshIndex mergePoint;

      static MeshIndex count = 0;

      halfEdgeVerbose = true;

      try
      {
         if ( m_HalfEdgeMesh->canAddTriangle( vs ) )
         {
            //DEBUGMACRO( ++m_NSteps << " / " << m_Mesh->getNumberOfFaces( ) );

            m_HalfEdgeMesh->addTriangle( vs, face, isMerge, mergePoint );

            if ( isMerge )
            {
               handleMerge( mergePoint );

               m_LastMergePoint = mergePoint;
            }

            ++count;

            // Find the neighbours of this face and add them into the queue
            MeshIndex adj[10];
            const MeshIndex nadj = m_Conn->getAdjacentFaces( face, adj );

            for ( MeshIndex i = 0; i < nadj; ++i )
               enqueFace( adj[i] );

            return true;
         }
         else
         {
            //DEBUGMACRO( "Cannot add face " << face << " yet (" << vs[0] << "," << vs[1] << "," << vs[2] << ")" );

            // Put it back into the queue
            //PRINTVEC( vs, 3 );
            //PAUSE;
            enqueFace( face );
            return true;
         }
      }
      catch ( AWT::Exception ex )
      {
         PRINTVBL( ex );
         DEBUGMACRO( __LINE__ << ": face=" << face << "; vs=" << vs[0] << "," << vs[1] << "," << vs[2] /*<< "; flag=" << (flag?"true":"false")*/ );
         PAUSE;
         BLANKLINE;
         return true;
      }
   }

   HalfEdge* findEdge( MeshIndex vA, MeshIndex vB )
   {
      HalfEdgeVertex* v = m_HalfEdgeMesh->getVertices( )[vA];
      HalfEdge* ret = v->getEdge( )->getPair( );
      HalfEdge* start = ret;

      do
      {
         if ( ret->getVertex( )->getIndex( ) == vB )
            return ret;

         ret = ret->getPair( )->getNext( );
      }
      while ( ret != start );

      return 0;
   }

   void relabelEdges( MeshIndex label )
   {
      // Unlabel any edges which are labelled
      HalfEdgeVec::iterator it = m_HalfEdgeMesh->getEdges( ).begin( );
      HalfEdgeVec::iterator en = m_HalfEdgeMesh->getEdges( ).end( );

      for ( ; it != en; ++it )
      {
         (*it)->setLabel( label );
      }
   }

   T getContourLength( HalfEdgeVec& vec )
   {
      T ret = 0;
      T tmp;

      for ( HalfEdgeVec::iterator it = vec.begin( ); it != vec.end( ); ++it )
      {
         tmp = getEdgeLengthSquared( *it );
         ret += sqrt( tmp );
      }

      return ret;
   }

   T getEdgeLengthSquared( HalfEdge* e )
   {
      T va[3], vb[3];

      MeshIndex to   = e->getVertex( )->getIndex( );
      MeshIndex from = e->getPair( )->getVertex( )->getIndex( );

      m_Mesh->getVertex( from, va );
      m_Mesh->getVertex( to,   vb );

      T ret = 0;

      for ( int i = 0; i < 3; ++i )
      {
         va[i] -= vb[i];
         ret += va[i] * va[i];
      }
      
      return ret;
   }
   
   IndexTuples::P edgeStripToContour( HalfEdgeVec path )
   {
      IndexTuples::P generator( *TuplesImpl<MeshIndex>::getInstance( 1, 1 ) );

      MeshIndex v = path[0]->getPair( )->getVertex( )->getIndex( );

      generator->addPoint( &v );

      for ( HalfEdgeVec::iterator it = path.begin( ); it != path.end( ); ++it )
      {
         v = (*it)->getVertex( )->getIndex( );

         generator->addPoint( &v );
      }

      return generator;
   }

   void handleMerge( MeshIndex mergePoint )
   {
      // Find the two outgoing edges from the merge point which are on the contour
      HalfEdge* limbStarts[2];
      MeshIndex limbLengths[2];
      MeshIndex limbsFound = 0;

      // Get the internal pointer of the half edge mesh
      HalfEdgeVec* edges          = &m_HalfEdgeMesh->getEdges( );
      std::vector< HalfEdgeVertex* >* vertices = &m_HalfEdgeMesh->getVertices( );

      HalfEdgeVec limbs[2];

      HalfEdge* e = (*vertices)[mergePoint]->getEdge( )->getPair( );
      HalfEdge* start = e;

      halfEdgeVerbose = true;

      DEBUGLINE;

      // Find the edges which point to the two "limbs" of this contour
      do
      {
         if ( !e->hasFace( ) )
         {
            // This is one of the limbs
            limbStarts[ limbsFound ] = e;

            // Now we need to calculate the length of each of the limbs 
            // because we're going to take the shorter of the two eventually
            limbLengths[ limbsFound ] = 0;

            PRINTVBL( limbsFound );

            HalfEdge* ee = e;
            do
            {
               // Increment the length by one
               ++limbLengths[ limbsFound ];

               limbs[ limbsFound ].push_back( ee );

               // Move onto the next edge
               ee = ee->getNext( );
            }
            while ( ee->getVertex( )->getIndex( ) != mergePoint );

            // Doesn't close the contour otherwise!
            limbs[ limbsFound ].push_back( ee );

            // Need to draw these in as well, otherwise loop isn't properly closed
            ee->setCrossable( false );
            ee->getPair( )->setCrossable( false );

            // ...
            ++limbsFound;
         }

         e = e->getPair( )->getNext( );
      }
      while ( e != start );
      DEBUGLINE;

      // Mark the zero-th limb as uncrossable
      setContourCrossable( limbs[0].begin( ), limbs[0].end( ), false );

      // Righty tighty.  Now we need to set up to perform Dijkstra's algorithm

      // We need to set up a distance matrix, which will be initialized to infinity
      // for each vertex, except for the merge point, which must be set to zero
      std::vector<T> distances;
      distances.reserve( m_Mesh->getNumberOfVertices( ) );
      //for ( MeshIndex v = 0; v < m_Mesh->getNumberOfVertices( ); ++v )
      MESH_EACHVERTEX( m_Mesh, v )
         distances.push_back( std::numeric_limits<T>::infinity( ) );

      distances[mergePoint] = 0;

      // Need to identify the two sets of edges from the merge vertex which will be "forbidden"
      // at various times in the Dijkstra traversal.  Let us define that the incoming edges are 
      // forbidden
      std::set< HalfEdge* > forbiddenEdges;
      DijkQueue queue;

      {
         HalfEdge* e = limbStarts[0];

         enqueEdge( queue, 0, e, false );
         
         while ( true )
         {
            enqueEdge( queue, 0, e, false );
            forbiddenEdges.insert( e->getPair( ) );
            
            if ( e != limbStarts[0] && !e->isCrossable( ) )
               break;

            e = e->getPair( )->getNext( );
         }
      }

      halfEdgeVerbose = false;
      HalfEdgeVec path = loopDijkstra( distances, mergePoint, queue, forbiddenEdges );

      if ( !path.empty( ) )
      {
         unsigned int limbSel = limbLengths[0]<limbLengths[1] ? 0 : 1;

         HalfEdgeVec::iterator beg, end;

         beg = limbs[ limbSel ].begin( );
         end = limbs[ limbSel ].end( );
         setContourCrossable( beg, end, false );

         beg = path.begin( );
         end = path.end( );
         setContourCrossable( beg, end, false );

         std::pair< HalfEdgeVec, HalfEdgeVec > pr( path, limbs[ limbSel ] );
         m_GeneratorPairs.push_back( pr );
      }
   }

   void setContourCrossable( HalfEdgeVec::iterator& beg, HalfEdgeVec::iterator& end, const bool v )
   {
      while ( beg != end )
      {
         (*beg)->setCrossable( v );
         (*beg)->getPair( )->setCrossable( v );

         ++beg;
      }
   }

   void resetCrossables( )
   {
      // Initially, set all edges as crossable
      HalfEdgeVec* edges = &m_HalfEdgeMesh->getEdges( );
      HalfEdgeVec::iterator esb = edges->begin( );
      HalfEdgeVec::iterator ese = edges->end( );

      while ( esb != ese )
         (*esb++)->setCrossable( true );

      // Now go through all of the generator pairs found and make these uncrossable
      HalfEdgeVecPairVec::iterator beg = m_GeneratorPairs.begin( );
      HalfEdgeVecPairVec::iterator end = m_GeneratorPairs.end( );

      while ( beg != end )
      {
         HalfEdgeVec::iterator beg0 = beg->first.begin( );
         HalfEdgeVec::iterator end0 = beg->first.end( );
         setContourCrossable( beg0, end0, false );

         HalfEdgeVec::iterator beg1 = beg->second.begin( );
         HalfEdgeVec::iterator end1 = beg->second.end( );
         setContourCrossable( beg1, end1, false );

         ++beg;
      }
   }

   void tightenGeneratorPairs( )
   {
      resetCrossables( );

      // For each generator pair in turn...
      HalfEdgeVecPairVec::iterator beg = m_GeneratorPairs.begin( );
      HalfEdgeVecPairVec::iterator end = m_GeneratorPairs.end( );

      // Create and allocate our distances vector (we will use this repeatedly to save reallocating it)
      std::vector< T > distances;
      //for ( MeshIndex i = 0; i < m_Mesh->getNumberOfVertices( ); ++i )
      MESH_EACHVERTEX( m_Mesh, i )
         distances.push_back( std::numeric_limits<T>::infinity( ) );

      while ( beg != end )
      {
         // Look at the first of the two contours
         HalfEdgeVec contA = beg->first;
         HalfEdgeVec contB = beg->second;

         tightenContour( contA, contB, distances );
         tightenContour( contB, contA, distances );

         beg->first = contA;
         beg->second = contB;

         setContourCrossable( contA.begin( ), contA.end( ), false );
         setContourCrossable( contB.begin( ), contB.end( ), false );

         lineUpContours( contA, contB );

         m_LastMergePoint = (*contA[0]).getVertex( )->getIndex( );

         beg->first  = contA;
         beg->second = contB;

         // Move to the next pair
         ++beg;
      }
   }

   void lineUpContours( HalfEdgeVec& contA, HalfEdgeVec& contB )
   {
      // Find the common vertex
      HalfEdgeVec::iterator begA, begB;
      HalfEdgeVec::iterator endA, endB;

      begA = contA.begin( );
      endA = contA.end( );

      bool notFound = true;

      while ( begA != endA && notFound )
      {
         begB = contB.begin( );
         endB = contB.end( );

         while ( begB != endB && notFound  )
         {
            if ( (*begA)->getVertex( ) == (*begB)->getVertex( ) )
            {
               // OK: Now we need to twizzle the contours in order that they have the same
               // starting vertex
               twizzle( contA, begA );
               twizzle( contB, begB );

               notFound = false;
            }

            ++begB;
         }

         ++begA;
      }
   }

   void twizzle( HalfEdgeVec& contour, HalfEdgeVec::iterator& from )
   {
      HalfEdgeVec newContour;

      HalfEdgeVec::iterator it = from;
      HalfEdgeVec::iterator to = contour.end( );

      for ( ; it != to; ++it )
         newContour.push_back( *it );

      it = contour.begin( );

      for ( ; it != from; ++it )
         newContour.push_back( *it );

      contour = newContour;
   }

   void flip( const MeshIndex p, const MeshIndex i )
   {
      if ( i == 0 )
         flip( m_GeneratorPairs[p].first );
      else
         flip( m_GeneratorPairs[p].second );
   }

   void flip( HalfEdgeVec& contour )
   {
      std::reverse( contour.begin(), contour.end( ) );

      for ( MeshIndex i = 0; i < contour.size( ); ++i )
         contour[i] = contour[i]->getPair( );
   }

   void tightenContour( HalfEdgeVec& contour, HalfEdgeVec& paired, std::vector< T >& distances )
   {
      // Firstly, we will mark contour as crossable, since we are going to reposition it anyway
      setContourCrossable( contour.begin( ), contour.end( ), true );

      // If we have parallel edges, we have just made them crossable.  Make sure they're not
      setContourCrossable( paired.begin( ), paired.end( ), false );

      std::set< HalfEdge* > forbiddenEdges;

      std::set< HalfEdgeVertex* > pairVerts;
      {
         HalfEdgeVec::iterator beg = paired.begin( );
         HalfEdgeVec::iterator end = paired.end( );

         for ( ; beg != end; ++beg )
         {
            pairVerts.insert( (*beg)->getVertex( ) );
         }
      }

      // The only way that I can think to do this is to find the loop for *every* point
      // on the contour; then pick the shortest one
      {
         HalfEdgeVec::iterator beg = paired.begin( );
         HalfEdgeVec::iterator end = paired.end( );

         HalfEdgeVec bestContour = contour;
         T bestLength = getContourLength( contour );

         MeshIndex cnt = 0;

         while ( beg != end )
         {
            DEBUGMACRO( ++cnt << "/" << paired.size( ) );

            // Set all distances to infinity...
            for ( MeshIndex i = 0; i < distances.size( ); ++i )
               distances[i] = std::numeric_limits<T>::infinity( );

            // Unlabel any edges which are labelled
            relabelEdges( std::numeric_limits<MeshIndex>::max( ) );

            // Set the distance to the current target to zero
            distances[ (*beg)->getVertex( )->getIndex( ) ] = 0;

            HalfEdgeVec newEdge = loopDijkstra( distances, (*beg)->getVertex( ), bestLength );

            if ( !newEdge.empty( ) )
            {
               T thisLength = getContourLength( newEdge );

               if ( thisLength < bestLength )
               {
                  bestContour = newEdge;
                  bestLength = thisLength;

                  PRINTVBL( bestLength );
               }
            }

            ++beg;
         }

         contour = bestContour;
      }
   }

   // Typedefs for notational convenience
   typedef std::pair< HalfEdge*, bool > DirectedEdge;
   typedef std::pair< T, DirectedEdge > DijkPair;
   typedef std::vector< DijkPair      > DijkPairVector;
   typedef std::priority_queue< DijkPair, DijkPairVector, std::greater< DijkPair > > DijkQueue;

   void enqueEdge( DijkQueue& queue, const T offset, HalfEdge* edge, const bool flip, const bool debug = false )
   {
      queue.push( DijkPair( offset + sqrt( getEdgeLengthSquared( edge ) ), 
         DirectedEdge( edge, flip ) ) );
   }

   void printEdgeNoDups( std::ostream& os, std::set< HalfEdge* >& previous, HalfEdge* edge )
   {
      if ( previous.find( edge ) == previous.end( ) )
      {
         os << *edge << std::endl;
         previous.insert( edge );
      }
   }

   void printVertex( std::ostream& os, HalfEdgeVertex* vertex )
   {
      std::set<HalfEdge*> previous;
      printVertexNoDups( os, previous, vertex );
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

   HalfEdgeVec loopDijkstra( std::vector<T>& distances, HalfEdgeVertex* vertex, const T fastBreak = std::numeric_limits<T>::max( ), const bool debug = false )
   {
      // Create a priority queue which we will use to sort the edges into the right order
      DijkQueue queue;

      // Create a set of forbidden edges
      std::set< HalfEdge* > forbiddenEdges;
      
      const MeshIndex startingVertex = vertex->getIndex( );
      
      if ( debug )
      {
         PRINTVBL( vertex->getIndex( ) );
         std::ofstream fs( "aroundmerge.gv" );
         fs << "digraph aroundmerge {" << std::endl;
         fs << "  overlap=scale" << std::endl;
         halfEdgeVerbose = true;
         std::set<HalfEdge*> ss;
         printVertexNoDups( fs, ss, vertex );
         halfEdgeVerbose = false;
         fs << "}" << std::endl;
         fs.close( );
         system( "neato -Tsvg -Lg -oaroundmerge.svg aroundmerge.gv" );
      }

      {
         HalfEdge* start = vertex->getEdge( );
         HalfEdge* ee    = start;

         ee = start;

         while ( ee->isCrossable( ) )
         {
            ee = ee->getNext( )->getPair( );

            // This means that we've got round to the same place again
            if ( ee == start )
               AWTEXCEPTIONTHROW( "This vertex has no uncrossable edges!" );
         }

         // Add this backwards edge into the queue - it's forbidden forwards
         enqueEdge( queue, 0, ee, true );
         
         if ( debug )
            DEBUGMACRO( "Forbidden Edges: xxx" );
         do
         {
            // This is an incoming edge - it's forbidden to come back along here
            forbiddenEdges.insert( ee );

            if ( debug )
               DEBUGMACRO( "    From " << ee->getPair( )->getVertex( )->getIndex( ) << " to " << ee->getVertex( )->getIndex( ) );

            // This is an outgoing edge - push this into the initial queue
            enqueEdge( queue, 0, ee->getNext( ), false );
            
            ee = ee->getNext( )->getPair( );
         }
         while ( ee->isCrossable( ) );
      }

      if ( debug )
         PAUSE;

      return loopDijkstra( distances, startingVertex, queue, forbiddenEdges, fastBreak, debug );
   }
   
   HalfEdgeVec loopDijkstra( std::vector<T>& distances, const MeshIndex startingVertex, DijkQueue& queue, std::set<HalfEdge*> forbiddenEdges, const T fastBreak = std::numeric_limits<T>::max( ), const bool debug = false )
   {
      bool success = false;

      HalfEdge*       backTrackEdge   = 0;
      HalfEdgeVertex* loopCloseVertex = 0;

      std::set< HalfEdge* >                  printedEdges;
      std::map< HalfEdgeVertex*, HalfEdge* > backtrace;

      T maxDistance = 0;

      halfEdgeVerbose = false;
      
      // Right, we're now set up to rock this
      while ( !success && !queue.empty( ) )
      {
         // Look at what's on the top of the queue and pop it
         DijkPair pair = queue.top( );
         queue.pop( );

         // It's not just as simple as looking at what the edge on
         // the top is because we may have to take its pair
         bool      flipEdge    = pair.second.second;

         // Keep a hold of both of these, because we need to know which side of the
         // uncrossable edge to stay
         HalfEdge* queuedEdge  = pair.second.first;
         HalfEdge* realEdge    = flipEdge ? queuedEdge->getPair( ) : queuedEdge;

         bool potentiallyForbidden = realEdge->getVertex( )->getIndex( ) == startingVertex
            || realEdge->getPair( )->getVertex( )->getIndex( ) == startingVertex;

         if ( !potentiallyForbidden || ( forbiddenEdges.find( realEdge ) == forbiddenEdges.end( ) ) )
         {
            HalfEdgeVertex* vertexFrom  = realEdge->getPair( )->getVertex( );
            HalfEdgeVertex* vertexCur   = realEdge->getVertex( );
            MeshIndex       vertexIndex = vertexCur->getIndex( );

            if ( debug )
            {
               DEBUGMACRO( "From " << vertexFrom->getIndex( ) << " to " << vertexCur->getIndex( ) );
               DEBUGMACRO( "Distance = " << pair.first );
            }

            maxDistance = std::max( maxDistance, pair.first );

            if ( vertexIndex == startingVertex )
            {
               if ( !flipEdge )
               {
                  // Record this transition in our backtrace
                  backtrace[ vertexCur ] = realEdge;

                  // We've made it back!
                  backTrackEdge = realEdge;
                  loopCloseVertex = realEdge->getVertex( );
                  success = true;
               }
            }
            else if ( distances[vertexIndex] > pair.first )
            {
               if ( pair.first >= fastBreak )
               {
                  // If we are here, we haven't got back to the start in under the "fast break"
                  // distance, so the closed contour will be longer - may as well break out now
                  break;
               }

               // Record this transition in our backtrace
               backtrace[ vertexCur ] = realEdge;
               
               const T offset = pair.first;
               
               // We may have reached this by a shorter route already
               // Reaching here means that this is the shortest route to this vertex
               distances[vertexIndex] = offset;

               // Now, we need to find the edges from this vertex
               // without crossing an uncrossable edge

               // Work out which of the edges is outgoing from the vertex and which is incoming
               HalfEdge* outgoing = ( flipEdge ) ? queuedEdge : queuedEdge->getPair( );

               // Get start and end edges, then we can just iterate between the two
               HalfEdge* start;
               HalfEdge* end;

               getStartEndEdges( outgoing, flipEdge, start, end );

               // Now, let's actually use these pointers
               do
               {
                  start = start->getPair( );

                  // start is now an incoming edge
                  enqueEdge( queue, offset, start, true );
                  enqueEdge( queue, offset, start->getNext( ), false );

                  start = start->getNext( );
               }
               while ( start != end );
            }
         }
      }
      
      if ( debug )
      {
         std::set<HalfEdge*>::iterator it = printedEdges.begin( );
         std::set<HalfEdge*>::iterator en = printedEdges.end( );

         std::ofstream fs( "aroundmerge.gv" );
         fs << "digraph aroundmerge {" << std::endl;
         fs << "  overlap=scale" << std::endl;
         halfEdgeVerbose = true;
         for ( ; it != en; ++it )
         {
            fs << *(*it) << std::endl;
         }
         halfEdgeVerbose = false;
         fs << "}" << std::endl;
         fs.close( );

         system( "neato -Tsvg -Lg -oaroundmerge.svg aroundmerge.gv" );
      }

      HalfEdgeVec ret;

      if ( success )
      {
         // Use the explicitly stored back-trace to find our way back to the start
         HalfEdge* backTraceEdge = backtrace[ loopCloseVertex ]->getPair( );
         
         doBackTrace( backtrace, ret, backTraceEdge, startingVertex, debug );

         PRINTVBL2( "New path", maxDistance );
      }
      else if ( queue.empty( ) )
      {
         DEBUGMACRO( "Failed to close loop!" );
      }

      if ( debug )
      {
         PAUSE;
      }

      return ret;
   }

   void doBackTrace( std::map< HalfEdgeVertex*, HalfEdge* >& backtrace, HalfEdgeVec& ret, HalfEdge*& backTraceEdge, const MeshIndex startingVertex, const bool debug )
   {
      if ( debug )
         DEBUGMACRO( "Backtrace:" );

      std::set<HalfEdge*> visited;

      while ( true )
      {
         ret.push_back( backTraceEdge );

         if ( visited.find( backTraceEdge ) != visited.end( ) )
            AWTEXCEPTIONTHROW( "I've already seen this edge!" );

         visited.insert( backTraceEdge );

         if ( debug )
         {
            DEBUGMACRO( "    From " << backTraceEdge->getPair( )->getVertex( )->getIndex( ) << " to " << backTraceEdge->getVertex( )->getIndex( ) );
         }

         if ( backTraceEdge->getVertex( )->getIndex( ) == startingVertex )
            break;

         backTraceEdge = backtrace[ backTraceEdge->getVertex( ) ]->getPair( );
      }
      
      if ( debug )
         BLANKLINE;
   }

   void getStartEndEdges( HalfEdge* outgoing, const bool flipEdge, HalfEdge*& start, HalfEdge*& end )
   {
      // Given an edge, we need to find where to start/end the iteration of neighbour edges
      // taking into account the uncrossability of certain edges
      end   = outgoing;
      start = outgoing;

      const bool swapStartEnd = !end->isCrossable( ) && !flipEdge;
      bool movedEnd = false;

      // First off, move end around until either we hit an uncrossable edge or the start
      while ( end->isCrossable( ) )
      {
         movedEnd = true;
         end = end->getPair( )->getNext( );

         if ( end == start )
            break;
      }

      // If end = start and we have moved, that means that we're at a free vertex
      if ( end != start || !movedEnd )
      {
         // Move start backwards until we find the uncrossable edge
         // We know that there is an uncrossable edge to be found
         do
         {
            start = start->getPrevious( )->getPair( );
         }
         while ( start->isCrossable( ) );

         // Now then, we have outgoing edges along the uncrossable contour
         // We need to stick to the correct side of the boundary
      }

      // This makes sure that we stick to the correct side of the contour
      if ( swapStartEnd )
      {
         HalfEdge* tmp = start;
         start = end;
         end = tmp;
      }
   }

   void findNeighbourSet( const HalfEdgeVec& vec, std::set<HalfEdgeVertex*>& contour, std::set<HalfEdgeVertex*>& neigh )
   {
      HalfEdgeVec::const_iterator begin = vec.begin( );
      HalfEdgeVec::const_iterator end   = vec.end( );

      while ( begin != end )
      {
         // Want the set of things on the contour
         contour.insert( (*begin)->getVertex( ) );

         HalfEdge* e = (*begin)->getPair( )->getNext( );

         while ( e->isCrossable( ) )
         {
            neigh.insert( e->getVertex( ) );

            e = e->getPair( )->getNext( );
         }

         ++begin;
      }
   }

   void process( )
   {
      // Calculate the connectivity of the mesh

      Noise<T>::timeSeed( );

      // Pick a triangle on the mesh to start from
      DEBUGMACRO( "FIXED TO FACE ZERO" );
      MeshIndex startingFace = 0; //( ( rand() << 15 ) | rand() ) % m_Mesh->getNumberOfFaces( );

      // Get ready to do the traversal...
      prepareTraversal( startingFace );
      
      m_NSteps = 0;
      // Actually do the traversal!

      {
         TimedBlock block( "Finding generators" );
         {
            {
               TimedBlock block( "Mesh Traversal" );
               while ( stepTraversal( ) );
            }

            MeshIndex counts[5];
            m_HalfEdgeMesh->getCounts( counts );
            PRINTVEC( counts, 5 );

            //DEBUGLINEANDEXIT( 99 );
         }

         {
            std::vector< HalfEdgeVertex* >* vertices = &m_HalfEdgeMesh->getVertices( );
            MeshConnectivity<T>::P conn = MeshConnectivity<T>::getInstance( m_Mesh );

            MeshIndex cntVerticesWithoutEdges = 0;
            BLANKLINE;
            DEBUGMACRO( "Looking for vertices without edges" );
            MESH_EACHVERTEX( m_Mesh, v )
            {
               // Get an outgoing edge from the current vertex
               if ( (*vertices)[v]->getEdge( ) == 0 )
               {
                  if ( cntVerticesWithoutEdges == 0 )
                     DEBUGMACRO( "The following vertices have no edge:-" );

                  DEBUGMACRO( v << "\t" << ( conn->isVertexUsed( v ) ? "true" : "false" ) );

                  ++cntVerticesWithoutEdges;
               }
            }

            if ( cntVerticesWithoutEdges != 0 )
            {
               PRINTVBL( cntVerticesWithoutEdges );
               //DEBUGLINEANDEXIT( 99 );
            }
         }
      }

      DEBUGLINE;
      // Now that we have the generators, we need to optimize them to make them short
      {
         TimedBlock block( "Tightening generators" );
         tightenGeneratorPairs( );
      }
      DEBUGLINE;
   }

   typename Mesh<T>::P             m_Mesh;

   HalfEdgeMesh::P                 m_HalfEdgeMesh;

   std::vector< std::pair< HalfEdgeVec, HalfEdgeVec > > m_GeneratorPairs;

   MeshIndex m_LastMergePoint;

   // I'm not going to use MeshBreaker for now.  I will try to keep it as agnostic to
   // MeshBreaker or not in this implementation.  Instead, I will just use a breadth-first
   // traversal
   typename MeshConnectivity<T>::P m_Conn;
   std::vector<bool>               m_FacesVisited;

   typename FindGenerators<T>::TraversalOrder traversalOrder;
};

template <class T>
AWT::FindGenerators<T>::FindGenerators( typename AWT::Mesh<T>::P mesh, typename const TraversalOrder ord )
{
   m_D = new D;

   m_D->traversalOrder = ord;

   m_D->m_Mesh = mesh;

   m_D->process( );
}

template <class T>
AWT::FindGenerators<T>::FindGenerators( )
{
}

template <class T>
AWT::FindGenerators<T>::~FindGenerators( )
{
   delete m_D;
}

template <class T>
typename AWT::FindGenerators<T>::P AWT::FindGenerators<T>::getInstance( typename AWT::Mesh<T>::P mesh, typename const TraversalOrder ord )
{
   AUTOGETINSTANCE( AWT::FindGenerators<T>, ( mesh, ord ) );
}

template <class T>
GETNAMEMACRO( AWT::FindGenerators<T> );

template <class T>
AWT::MeshIndex AWT::FindGenerators<T>::getNumberOfGeneratorPairs( ) const
{
   return static_cast<AWT::MeshIndex>( m_D->m_GeneratorPairs.size( ) );
}

template <class T>
void AWT::FindGenerators<T>::getGeneratorPair( const AWT::MeshIndex p, AWT::Tuples<AWT::MeshIndex>::P& contA, AWT::Tuples<AWT::MeshIndex>::P& contB )
{
   contA = m_D->edgeStripToContour( m_D->m_GeneratorPairs[p].first );
   contB = m_D->edgeStripToContour( m_D->m_GeneratorPairs[p].second );
}

template <class T>
typename AWT::Mesh<T>::P AWT::FindGenerators<T>::getMesh( )
{
   return m_D->m_Mesh;
}

template <class T>
bool AWT::FindGenerators<T>::isFaceVisited( const AWT::MeshIndex f ) const
{
   return m_D->m_FacesVisited[ f ];
}

template <class T>
AWT::MeshIndex AWT::FindGenerators<T>::getLastMergePoint( ) const
{
   return m_D->m_LastMergePoint;
}

template <class T>
AWT::HalfEdgeMesh::P AWT::FindGenerators<T>::getHalfEdgeMesh( )
{
   return m_D->m_HalfEdgeMesh;
}

template <class T>
void AWT::FindGenerators<T>::getGeneratorPair( const AWT::MeshIndex p, std::vector< AWT::HalfEdge* >& contA, std::vector< AWT::HalfEdge* >& contB )
{
   contA = m_D->m_GeneratorPairs[p].first;
   contB = m_D->m_GeneratorPairs[p].second;
}

template <class T>
void AWT::FindGenerators<T>::flipGenerator(const AWT::MeshIndex p, const AWT::MeshIndex i )
{
   m_D->flip( p, i );
}

template <class T>
const std::vector< AWT::HalfEdge* >& AWT::FindGenerators<T>::getGenerator( const AWT::MeshIndex p, const unsigned int i )
{
   switch ( i )
   {
   case 0:
      return m_D->m_GeneratorPairs[p].first;
   case 1:
      return m_D->m_GeneratorPairs[p].second;
   default:
      AWTEXCEPTIONTHROW( "Index out of bounds" );
   }
}

template <class T>
typename AWT::FindGenerators<T>::TraversalOrder AWT::FindGenerators<T>::getTraversalOrder( ) const
{
   return m_D->traversalOrder;
}

template <class T>
void AWT::FindGenerators<T>::setTraversalOrder( const typename AWT::FindGenerators<T>::TraversalOrder ord )
{
   m_D->traversalOrder = ord;
}

template <class T>
typename AWT::FindGenerators<T>::TraversalOrder AWT::FindGenerators<T>::getTraversalOrder( const char* wstr )
{
   if ( strcmp( wstr, "breadth" ) == 0 )
   {
      return FindGenerators<T>::TRAVERSE_BREADTH;
   }
   else if ( strcmp( wstr, "depth" ) == 0 )
   {
      return FindGenerators<T>::TRAVERSE_DEPTH;
   }
   else if ( strcmp( wstr, "random" ) == 0 )
   {
      return FindGenerators<T>::TRAVERSE_RANDOM;
   }

   DEBUGMACRO( "Specified traversal order not recognised; assuming breadth-first" );
   return FindGenerators<T>::TRAVERSE_BREADTH;
}

template class AWT::FindGenerators<double>;
template class AWT::FindGenerators<float>;