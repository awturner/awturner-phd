#include "MeshBreaker.h"

#include "Mesh/MeshConnectivity.h"
#include "Mesh/TuplesImpl.h"
#include "Mesh/MeshFunctions.h"

#include "Useful/Noise.h"

#include <vector>
#include <fstream>

#include "Contour.h"
#include "Edge.h"

#include "MeshBreakerHistory.h"

template <class T>
struct AWT::MeshBreaker<T>::D
{
   std::string getSymbolName( const Symbol s )
   {
      switch ( s )
      {
      case SYM_C:
         return "SYM_C";
      case SYM_L:
         return "SYM_L";
      case SYM_E:
         return "SYM_E";
      case SYM_R: 
         return "SYM_R";
      case SYM_S:
         return "SYM_S";
      default:
         return "SYM_UNDEF";
      }
   }

   void enque( const MeshIndex f, const MeshIndex g )
   {
      m_FacesQueue.push_back( std::pair<MeshIndex,MeshIndex>( f, g ) );
   }

   bool peek( MeshIndex& f, MeshIndex& g ) const
   {
      if ( isFinished( ) )
         return false;

      f = m_FacesQueue.back().first;
      g = m_FacesQueue.back().second;

      return true;
   }

   bool deque( )
   {
      if ( m_FacesQueue.empty() )
         return false;

      m_FacesQueue.pop_back();

      return true;
   }

   bool isFinished( ) const
   {
      return m_FacesQueue.empty();
   }

   bool isFaceVisited( const MeshIndex f ) const
   {
      return m_FacesVisited->getPointElement( f, 0 );
   }

   void setFaceVisited( const MeshIndex f )
   {
      m_FacesVisited->setPointElement( f, 0, true );
   }

   bool isVertexVisited( const MeshIndex v ) const
   {
      return m_VerticesVisited->getPointElement( v, 0 );
   }

   // No need for second argument, you never mark it "unvisited" again
   void setVertexVisited( const MeshIndex v )
   {
      m_VerticesVisited->setPointElement( v, 0, true );
   }

   void getNeighbourFaces( const MeshIndex f, const MeshIndex g, MeshIndex& lFace, MeshIndex& rFace, MeshIndex& nonSharedVertex )
   {
      // Paranoid sanity check!
      testAdjacency( f, g, __LINE__ );

      MeshIndex fis[3], gis[3], ais[3];

      m_Mesh->getFaceIndices( f, fis );
      m_Mesh->getFaceIndices( g, gis );

      // Identify the non-shared vertex in g
      // i.e. the vertex in g which isn't in f
      int notShared;
      for ( notShared = 0; notShared < 3; ++notShared )
      {
         bool found = false;

         for ( int j = 0; j < 3 && !found; ++j )
         {
            if ( gis[notShared] == fis[j] )
               found = true;
         }

         if ( !found )
            break;
      }

      nonSharedVertex = gis[notShared];

      // Initialize these to NOTFOUND
      lFace = NOTFOUND;
      rFace = NOTFOUND;

      // Get all of the faces adjacent to g
      // One of these should be f, the other (max) two are potential next steps
      
      MeshIndex adjFaces[3], nadj;
      nadj = m_Conn->getAdjacentFaces( g, adjFaces );

      if ( g == 360436 )
      {
         MeshIndex vs[3];

         PRINTVBL( f );
         m_Mesh->getFaceIndices( f, vs );
         PRINTVEC( vs, 3 );

         PRINTVBL( g );
         m_Mesh->getFaceIndices( g, vs );
         PRINTVEC( vs, 3 );

         PRINTVBL( nonSharedVertex );

         m_DebugFaces.clear( );

         for ( MeshIndex i = 0; i < nadj; ++i )
         {
            m_DebugFaces.push_back( adjFaces[i] );

            PRINTVBL( adjFaces[i] );
            m_Mesh->getFaceIndices( adjFaces[i], vs );
            PRINTVEC( vs, 3 );
            PRINTVBL2( "Visited?", isFaceVisited( adjFaces[i] ) ? "visited" : "unvisited" );
         }
      }

      // Now, we can identify "left" and "right" adjacent faces of the faces
      for ( MeshIndex adj = 0; adj < nadj; ++adj )
      {
         testAdjacency( g, adjFaces[adj], __LINE__ );

         // This also handles the case that adjFaces[adj]==f, since f must have been visited
         if ( isFaceVisited( adjFaces[adj] ) )
            continue;

         m_Mesh->getFaceIndices( adjFaces[adj], ais );

         {
            // Sanity check: make sure that face adjFaces[adj] contains nonSharedVertex
            bool foundNonShared = false;

            for ( int j = 0; j < 3; ++j )
            {
               if ( ais[j] == nonSharedVertex )
                  foundNonShared = true;
               else if ( ais[j] == gis[(notShared+1)%3] )
                  lFace = adjFaces[adj];
               else if ( ais[j] == gis[(notShared+2)%3] )
                  rFace = adjFaces[adj];
            }

            // Sanity check: make sure that face adjFaces[adj] contains nonSharedVertex
            if ( !foundNonShared )
            {
               continue;

               std::ofstream os( "meshbreaker.gv" );
               os << "digraph meshbreaker {" << std::endl;
               os << "  overlap=scale" << std::endl;

               m_Mesh->getFaceIndices( g, ais );

               // Print out the base face
               os << "/* Base face */" << std::endl;

               os << "\"f" << g << "\" " << std::endl;
               os << "\"v" << ais[0] << "\" " << std::endl;
               os << "\"v" << ais[1] << "\" " << std::endl;
               os << "\"v" << ais[2] << "\" " << std::endl;

               os << "\"f" << g << "\" -> \"v" << ais[0] << "\"" << std::endl;
               os << "\"f" << g << "\" -> \"v" << ais[1] << "\"" << std::endl;
               os << "\"f" << g << "\" -> \"v" << ais[2] << "\"" << std::endl;
               os << "\"v" << ais[0] << "\" -> \"v" << ais[1] << "\"" << std::endl;
               os << "\"v" << ais[1] << "\" -> \"v" << ais[2] << "\"" << std::endl;
               os << "\"v" << ais[2] << "\" -> \"v" << ais[0] << "\"" << std::endl;

               // Print out the adjacent face
               for ( unsigned int j = 0; j < nadj; ++j )
               {
                  m_Mesh->getFaceIndices( adjFaces[j], ais );

                  os << "/* Face " << j << "*/" << std::endl;
                  os << "\"f" << adjFaces[j] << "\" -> \"v" << ais[0] << "\"" << std::endl;
                  os << "\"f" << adjFaces[j] << "\" -> \"v" << ais[1] << "\"" << std::endl;
                  os << "\"f" << adjFaces[j] << "\" -> \"v" << ais[2] << "\"" << std::endl;
                  
                  os << "\"v" << ais[0] << "\" -> \"v" << ais[1] << "\"" << std::endl;
                  os << "\"v" << ais[1] << "\" -> \"v" << ais[2] << "\"" << std::endl;
                  os << "\"v" << ais[2] << "\" -> \"v" << ais[0] << "\"" << std::endl;
               }

               os << "\"f" << f << "\" [color=blue]" << std::endl;
               os << "\"f" << g << "\" [color=red]" << std::endl;

               os << "\"v" << nonSharedVertex << "\" [color=green]" << std::endl;

               os << "}" << std::endl;
               os.close( );

               system( "neato -omeshbreaker.svg -Tsvg -Lg meshbreaker.gv" );

               DEBUGMACRO( f << "\t" << g );
               PRINTVEC( fis, 3 );
               PRINTVEC( gis, 3 );
               PRINTVBL( nonSharedVertex );
               BLANKLINE;
               PRINTVEC( ais, 3 );
               BLANKLINE;
               PRINTVEC( adjFaces, nadj );

               for ( unsigned int j = 0; j < nadj; ++j )
               {
                  m_Mesh->getFaceIndices( adjFaces[j], ais );
                  PRINTVBL2( "Face    : ", adjFaces[j] );
                  PRINTVEC2( "Vertices: ", ais, 3 );
               }

               AWTEXCEPTIONTHROW_STREAM( "Could not find shared vertex; sanity check failed " << __LINE__ );
            }
         }
      }
   }

   void setFaceBranch( const MeshIndex f, const MeshIndex branchIndex )
   {
      m_FaceBranch->setPointElement( f, 0, branchIndex );
   }

   MeshIndex getFaceBranch( const MeshIndex f ) const
   {
      return m_FaceBranch->getPointElement( f, 0 );
   }

   Symbol step( Symbol previousSymbol, MeshIndex& lastF, MeshIndex& lastG )
   {
      //if ( previousSymbol == SYM_E )
      //   return SYM_UNDEF;

      MeshIndex f, g;

      // If we are empty, then there is no symbol, shouldn't have called this
      if ( !peek( f, g ) )
         return SYM_UNDEF;

      setFaceVisited( g );

      lastF = f;
      lastG = g;

      // Sanity Check: make sure that there are two vertices in common
      testAdjacency( f, g, __LINE__ );

      // Get the faces which adjoin g on the left and right of the opposite vertex
      MeshIndex lFace, rFace, nonSharedVertex;
      getNeighbourFaces( f, g, lFace, rFace, nonSharedVertex );

      if ( g == 360436 )
      {
         MeshIndex vs[3];

         PRINTVBL( lFace );
         if ( lFace != NOTFOUND )
         {
            m_Mesh->getFaceIndices( lFace, vs );
            PRINTVEC( vs, 3 );
         }

         PRINTVBL( rFace );
         if ( rFace != NOTFOUND )
         {
            m_Mesh->getFaceIndices( rFace, vs );
            PRINTVEC( vs, 3 );
         }

         PRINTVBL( nonSharedVertex );
      }

      // Right: if we didn't find both a right and a left face, then we're on the edge
      // Guide things by pretending that we can't go across the edge because it's already been visited
      // This means that the non-shared vertex must be marked visited

      Symbol ret;

      bool vertexUnvisited = 
         !isVertexVisited( nonSharedVertex ) 
         && lFace != NOTFOUND && rFace != NOTFOUND
         && !m_Conn->isVertexOnBoundary( nonSharedVertex );

      setVertexVisited( nonSharedVertex );

      // If we haven't visited the non-shared vertex, this is a case of C - go to the right face
      if ( vertexUnvisited )
      {
         enque( g, rFace );
         ret = SYM_C;
      }
      else if ( lFace != NOTFOUND && rFace == NOTFOUND )
      {
         enque( g, lFace );
         ret = SYM_R;
      }
      else if ( lFace == NOTFOUND && rFace != NOTFOUND )
      {
         enque( g, rFace );
         ret = SYM_L;
      }
      else if ( lFace != NOTFOUND && rFace != NOTFOUND )
      {
         // This is the split case.  We need to start a new pair of branches here
         // Queue them up in reverse order (stack is LIFO)
         enque( g, lFace );
         enque( g, rFace );

         ret = SYM_S;
      }
      else
      {
         ret = SYM_E;
      }

      typename MeshBreakerHistory<T>::P hist = MeshBreakerHistory<T>::getInstance( ret, f, g, nonSharedVertex, lFace, rFace );
      m_History.push_back( hist );

      if ( g == 360436 )
      {
         DEBUGMACRO( getSymbolName( ret ) );
         m_FacesQueue.clear( );
      }

      // Keep on dequeuing until either we are empty or we find an unvisited face
      while ( peek( f, g ) && isFaceVisited( g ) )
         deque( );

      return ret;
   }

   bool testAdjacency( const MeshIndex a, const MeshIndex b, const int callLine )
   {
      bool ret = m_Conn->areFacesAdjacent( a, b );

      if ( !ret )
      {
         MeshIndex ais[3], bis[3];

         m_Mesh->getFaceIndices( a, ais );
         m_Mesh->getFaceIndices( b, bis );

         PRINTVEC( ais, 3 );
         PRINTVEC( bis, 3 );
         AWTEXCEPTIONTHROW_STREAM( "Adjacency check failed " << callLine );
      }

      return ret;
   }

   void handleBranch( const Symbol sym, const MeshIndex lastF, const MeshIndex lastG )
   {
      // Set the index of lastG's branch
      //setFaceBranch( lastG, static_cast<MeshIndex>( m_Branches.size() - 1 ) );
      setFaceBranch( lastG, 0 );

      const bool cond = false;

      if ( cond )
      {
         std::cerr << lastF << " " << lastG << " " << getSymbolName( sym ) << std::endl;
      }
      
      // Put the common edge mid-point into the line (draws nicer on non-planar surfaces)
      MeshIndex fis[3], gis[3];

      m_Mesh->getFaceIndices( lastF, fis );
      m_Mesh->getFaceIndices( lastG, gis );

      if ( cond )
      {
         std::cerr << "\t" << fis[0] << " " << fis[1] << " " << fis[2] << std::endl;
         std::cerr << "\t" << gis[0] << " " << gis[1] << " " << gis[2] << std::endl;
      }

      bool commonF[] = { false, false, false };
      bool commonG[] = { false, false, false };

      for ( int i = 0; i < 3; ++i )
      {
         for ( int j = 0; j < 3; ++j )
         {
            if ( fis[i] == gis[j] )
               commonF[i] = commonG[j] = true;
         }
      }

      MeshIndex commonVs[2]; MeshIndex cvi = 0;
      T vtxA[3], vtxB[3];
      T centroid[3];

      if ( m_Branches.back()->getNumberOfPoints( ) == 0 )
      {
         MeshFunctions<T>::calculateFaceCentroid( m_Mesh, lastF, centroid );
         m_Branches.back()->addPoint( centroid );
      }
      
      for ( int i = 0; i < 3; ++i )
         if ( commonF[i] ) commonVs[cvi++] = fis[i];
      m_Mesh->getVertex( commonVs[0], vtxA );
      m_Mesh->getVertex( commonVs[1], vtxB );
      for ( int i = 0; i < 3; ++i )
         vtxA[i] = ( vtxA[i] + vtxB[i] ) / 2;
      m_Branches.back()->addPoint( vtxA );

      // Push the centroid of lastG onto the active branch
      
      MeshFunctions<T>::calculateFaceCentroid( m_Mesh, lastG, centroid );
      m_Branches.back()->addPoint( centroid );

      if ( sym == SYM_E || sym == SYM_S )
      {
         if ( cond )
         {
            std::cerr << "NEW BRANCH" << std::endl;
         }

         m_Branches.push_back( TuplesImpl<T>::getInstance( 3, 1 ) );

         setFaceBranch( lastG, (sym == SYM_E) ? 1 : 2 );
      }
   }

   void process( )
   {
      Noise<MeshIndex>::timeSeed( );

      bool def = false;

      m_Conn = MeshConnectivity<T>::getInstance( m_Mesh );

      // Set up arrays to record whether we have visited the face/vertex or not
      m_VerticesVisited = TuplesImpl<bool>::getInstance( 1, &def, m_Mesh->getNumberOfVertices() );
      m_FacesVisited    = TuplesImpl<bool>::getInstance( 1, &def, m_Mesh->getNumberOfFaces() );

      // Set up array to hold which branch the face was visited by
      m_FaceBranch = TuplesImpl<MeshIndex>::getInstance( 1, m_Mesh->getNumberOfFaces() );

      // Initialize all vertices to unvisited
      //for ( MeshIndex v = 0; v < m_Mesh->getNumberOfVertices(); ++v )
      MESH_EACHVERTEX( m_Mesh, v )
         m_VerticesVisited->setPointElement( v, 0, false );

      // Initialize all faces to unvisited
      //for ( MeshIndex f = 0; f < m_Mesh->getNumberOfFaces(); ++f )
      MESH_EACHFACE( m_Mesh, f )
      {
         m_FacesVisited->setPointElement( f, 0, false );
         m_FaceBranch->setPointElement( f, 0, UNVISITED );
      }

      // We will use this variously to hold face indices
      MeshIndex fs[3];

      // Pick starting vertices
      MeshIndex f, g;

#if 1
      DEBUGMACRO( "random starting face" );
      MeshIndex nadj;
      do
      {
         // Pick a random face in the mesh
         f = ( rand() << 15 | rand() ) % m_Mesh->getNumberOfFaces();

         // Test to see how many adjacent faces we have
         nadj = m_Conn->getAdjacentFaces( f, fs );
      }
      while ( nadj == 0 );

      // Now pick a random adjacent face
      g = fs[rand()%nadj];

#else
      // For now, keep it non-random
      DEBUGMACRO( "non-random starting face" );
      f = 171673;
      g = 171672;
#endif

      // Mark face f and its vertices as visited
      //m_FacesVisited->setPointElement( f, 0, true );
      m_Mesh->getFaceIndices( f, fs );
      for ( int j = 0; j < 3; ++j )
         setVertexVisited( fs[j] );

      // Start a new branch at f
      T centroid[3];
      MeshFunctions<T>::calculateFaceCentroid( m_Mesh, f, centroid );

      Tuples<T>::P branch = TuplesImpl<T>::getInstance( 3, m_Mesh->getNumberOfFaces() );
      branch->addPoint( centroid );

      m_Branches.push_back( branch );

      // Now call process on these two faces
      setFaceBranch( f, 0 );
      setFaceVisited( f );
      enque( f, g );

      int iters = 0;

      Symbol sym = SYM_UNDEF;

      EdgeBuilder::P eb = EdgeBuilder::getInstance( );

      MeshIndex fis[3];
      m_Mesh->getFaceIndices( f, fis );

      // Make sure that there is nothing in m_Contours
      m_Contours.clear();

      // Put the first triangle into the set of contours
      Contour::insert( m_Contours, eb, fis, false );
      
      // Now we should have a single contour with three edges
      
      try
      {
         unsigned int stops = 0;

         do
         {
            MeshIndex lastF, lastG;

            // Do the meshbreaker step, recording the last f and g
            sym = step( sym, lastF, lastG );

            if ( sym != SYM_UNDEF )
            {
               handleBranch( sym, lastF, lastG );

               size_t oldS = m_Contours.size();

               m_Mesh->getFaceIndices( lastG, fis );
               Contour::insert( m_Contours, eb, fis, sym == SYM_S );

               PRINTVBL( m_Contours.size( ) );

               if ( sym == SYM_S && m_Contours.size() < oldS )
               {
                  break;
               }
            }

            if ( false && sym != SYM_UNDEF )
            {
               // Add in the most recently found triangle to the contour set
               m_Mesh->getFaceIndices( lastG, fis );

               // Keep the old size, so we can check if a merge occurred
               std::size_t oldSize = m_Contours.size( );

               std::size_t oldSSize = m_Contours.back( )->getLength( );

               if ( false )
               {
                  // Temporarily add the new contour so we can see
                  Contour::P tmpCont = Contour::getInstance( eb, fis, 3 );
                  m_Contours.push_back( tmpCont );

                  std::ofstream os( "foo.gv" );
                  Contour::printContours( os, m_Contours );
                  os.close( );
                  Contour::renderGraph( "foo.gv", "foo.svg" );

                  m_Contours.pop_back( );
               }

               Contour::insert( m_Contours, eb, fis, sym == SYM_S );

               PRINTVEC( fis, 3 );

               if ( false )
               {
                  std::ofstream os( "foo2.gv" );
                  Contour::printContours( os, m_Contours );
                  os.close( );
                  Contour::renderGraph( "foo2.gv", "foo2.svg" );

                  if ( m_Contours.back( )->getLength( ) == oldSSize )
                     PAUSE;
               }

               // It's only a merge event if it's also a split (SYM_S)
               // Otherwise it's an end (SYM_E)
               if ( sym == SYM_S && m_Contours.size( ) < oldSize )
               {
                  DEBUGMACRO( "Merge event!" );
                  PRINTVBL( getSymbolName( sym ) );
                  break;
               }
            }

         }
         while ( sym != SYM_UNDEF && !isFinished( ) );

         PRINTVBL2( "Last sym", getSymbolName( sym ) );
         PRINTVBL2( "Empty?", 1*m_FacesQueue.empty() );
      }
      catch (AWT::Exception ex)
      {
         DEBUGMACRO( ex );
      }

      while ( m_Branches.back( )->getNumberOfPoints( ) == 0 )
         m_Branches.pop_back( );
   }

   static const MeshIndex NOTFOUND, UNVISITED;

   std::vector< std::pair<MeshIndex,MeshIndex> > m_FacesQueue;

   std::vector< Contour::P >            m_Contours;

   typename Mesh<T>::P                  m_Mesh;
   typename Tuples<bool>::P             m_FacesVisited;
   typename Tuples<bool>::P             m_VerticesVisited;
   typename MeshConnectivity<T>::P      m_Conn;

   typename Tuples<MeshIndex>::P        m_FaceBranch;

   std::vector< typename Tuples<T>::P > m_Branches;

   std::vector< typename MeshBreakerHistory<T>::P > m_History;
   std::vector< MeshIndex >                         m_DebugFaces;
};

template <class T>
const AWT::MeshIndex AWT::MeshBreaker<T>::D::NOTFOUND = std::numeric_limits<MeshIndex>::max();

template <class T>
const AWT::MeshIndex AWT::MeshBreaker<T>::D::UNVISITED = std::numeric_limits<MeshIndex>::max();

template <class T>
AWT::MeshBreaker<T>::MeshBreaker( typename Mesh<T>::P mesh )
{
   m_D = new D;

   m_D->m_Mesh = mesh;

   m_D->process( );
}

template <class T>
AWT::MeshBreaker<T>::~MeshBreaker( )
{
   delete m_D;
}

template <class T>
typename AWT::MeshBreaker<T>::P AWT::MeshBreaker<T>::getInstance( typename Mesh<T>::P mesh )
{
   AUTOGETINSTANCE( AWT::MeshBreaker<T>, ( mesh ) );
}

template <class T>
GETNAMEMACRO( AWT::MeshBreaker<T> );

template <class T>
unsigned int AWT::MeshBreaker<T>::getNumberOfBranches( ) const
{
   return static_cast<unsigned int>( m_D->m_Branches.size() );
}

template <class T>
typename AWT::Tuples<T>::P AWT::MeshBreaker<T>::getBranch( unsigned int i )
{
   return m_D->m_Branches[i];
}

#include "Mesh/MeshImpl.h"

template <class T>
typename AWT::Mesh<T>::P AWT::MeshBreaker<T>::getRegion( const unsigned int r )
{
   AWT::Mesh<T>::P ret = AWT::MeshImpl<T>::getInstance( m_D->m_Mesh->getNumberOfVertices(), m_D->m_Mesh->getNumberOfFaces() );

   AWT::Tuples<MeshIndex>::P faces = AWT::TuplesImpl<MeshIndex>::getInstance( 3, m_D->m_Mesh->getNumberOfFaces() );

   MeshIndex vs[3];
   //for ( MeshIndex f = 0; f < m_D->m_Mesh->getNumberOfFaces(); ++f )
   MESH_EACHFACE( m_D->m_Mesh, f )
   {
      if ( m_D->getFaceBranch( f ) == r )
      {
         m_D->m_Mesh->getFaceIndices( f, vs );
         faces->addPoint( vs );
      }
   }

   ret->setVertices( m_D->m_Mesh->getVertices() );
   ret->setFaces( faces );

   return ret;
}

template <class T>
unsigned int AWT::MeshBreaker<T>::getNumberOfContours( ) const
{
   return static_cast<unsigned int>( m_D->m_Contours.size() );
}

template <class T>
typename AWT::Tuples<T>::P AWT::MeshBreaker<T>::getContour( unsigned int i )
{
   AWT::Tuples<MeshIndex>::P contourIndices = m_D->m_Contours[i]->getContour( );

   AWT::Tuples<T>::P ret = AWT::TuplesImpl<T>::getInstance( 3, contourIndices->getNumberOfPoints() );

   T vtx[3];

   for ( MeshIndex i = 0, imax = contourIndices->getNumberOfPoints(); i < imax; ++i )
   {
      m_D->m_Mesh->getVertex( contourIndices->getPointElement( i, 0 ), vtx );
      ret->addPoint( vtx );
   }

   return ret;
}

template <class T>
AWT::MeshIndex AWT::MeshBreaker<T>::getHistoryLength( ) const
{
   return static_cast<AWT::MeshIndex>( m_D->m_History.size( ) );
}

template <class T>
void AWT::MeshBreaker<T>::printHistoryItem( AWT::MeshIndex h ) const
{
   typename MeshBreakerHistory<T>::P hist = m_D->m_History[h];

   std::cerr << "History: Step " << h << std::endl;
   std::cerr << "\t" << "F   = " << hist->getF( ) << std::endl;
   std::cerr << "\t" << "G   = " << hist->getG( ) << std::endl;
   std::cerr << "\t" << "sym = " << hist->getSymbol( ) << std::endl;
   std::cerr << "\t" << "LN  = " << hist->getLeftNeighbour( ) << std::endl;
   std::cerr << "\t" << "RN  = " << hist->getRightNeighbour( ) << std::endl;
   std::cerr << "\t" << "DV  = " << hist->getDistalVertex( ) << std::endl;

   BLANKLINE;
}

template <class T>
typename AWT::MeshBreaker<T>::Symbol AWT::MeshBreaker<T>::getHistorySymbol( AWT::MeshIndex h ) const
{
   return m_D->m_History[h]->getSymbol( );
}

template <class T>
AWT::MeshIndex AWT::MeshBreaker<T>::getHistoryFace( AWT::MeshIndex h ) const
{
   return m_D->m_History[h]->getG( );
}

template <class T>
AWT::MeshIndex AWT::MeshBreaker<T>::getHistoryLeftNeighbour( AWT::MeshIndex h ) const
{
   return m_D->m_History[h]->getLeftNeighbour( );
}

template <class T>
AWT::MeshIndex AWT::MeshBreaker<T>::getHistoryRightNeighbour( AWT::MeshIndex h ) const
{
   return m_D->m_History[h]->getRightNeighbour( );
}

template <class T>
typename AWT::Mesh<T>::P AWT::MeshBreaker<T>::getMesh( )
{
   return m_D->m_Mesh;
}

template <class T>
typename AWT::MeshConnectivity<T>::P AWT::MeshBreaker<T>::getConnectivity( )
{
   return m_D->m_Conn;
}

template <class T>
AWT::MeshIndex AWT::MeshBreaker<T>::getNumberOfDebugFaces( ) const
{
   return static_cast<MeshIndex>( m_D->m_DebugFaces.size( ) );
}

template <class T>
AWT::MeshIndex AWT::MeshBreaker<T>::getDebugFace( const MeshIndex i ) const
{
   return m_D->m_DebugFaces[i];
}

template class AWT::MeshBreaker<double>;
template class AWT::MeshBreaker<float>;