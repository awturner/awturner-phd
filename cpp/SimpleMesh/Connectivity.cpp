#include "Connectivity.h"

#include "Useful/SetIterator.h"

#include "Mesh/TuplesImpl.h"
#include <vector>
#include <set>

struct AWT::SimpleMesh::Connectivity::D
{
   Mesh::P mesh;
   Tuples<Index>::P adjFaces;

   std::vector<std::vector<Index>>   facesUsingVertex;
   std::vector<std::set<Index>>      adjVertices;
};

AWT::SimpleMesh::Connectivity::Connectivity( AWT::SimpleMesh::Mesh::P mesh )
{
   m_D = new D;

   Index indices[] = { INVALID_INDEX, INVALID_INDEX, INVALID_INDEX };

   m_D->adjFaces = TuplesImpl<Index>::getInstance( 3, indices, mesh->nf );
   
   m_D->mesh = mesh;

   const Index searches[] = { 1142, 1038, 1035 };

   // This builds a huge list of the faces which use each vertex
   {
      m_D->facesUsingVertex.clear( );

      for ( Index v = 0; v < mesh->nv; ++v )
      {
         // Create a holder for this vertex's using cells
         m_D->facesUsingVertex.push_back( std::vector<Index>( ) );
         m_D->adjVertices.push_back( std::set<Index>( ) );
      }

      for ( Index f = 0; f < mesh->nf; ++f )
      {
         SimpleMesh::Face vs = mesh->getFace( f );

         for ( int i = 0; i < 3; ++i )
         {
            m_D->adjVertices[ vs[i] ].insert( vs[(i+1)%3] );
            m_D->adjVertices[ vs[i] ].insert( vs[(i+2)%3] );

            m_D->adjVertices[ vs[(i+1)%3] ].insert( vs[i] );
            m_D->adjVertices[ vs[(i+2)%3] ].insert( vs[i] );
         }

         m_D->facesUsingVertex[ vs[0] ].push_back( f );
         m_D->facesUsingVertex[ vs[1] ].push_back( f );
         m_D->facesUsingVertex[ vs[2] ].push_back( f );

         /*
         bool found = false;
         for ( int a = 0; a < 3 && !found; ++a )
            for ( int b = 0; b < 3 && !found; ++b )
               found = vs[a] == searches[b];

         if ( found )
         {
            PRINTVBL( f );
            PRINTVEC( vs, 3 );
         }
         */
      }
   }

   /*
   for ( Index i = 0; i < 3; ++i )
   {
      DEBUGMACRO( "Faces using " << searches[i] );
      std::vector<Index>& fuv = m_D->facesUsingVertex[ searches[i] ];

      std::vector<Index>::iterator it = fuv.begin();
      std::vector<Index>::iterator en = fuv.end();
      while ( it != en )
         std::cerr << "  " << *it++;
      std::cerr << std::endl;
   }
   */

   DEBUGLINE;
}

AWT::SimpleMesh::Connectivity::~Connectivity( )
{
   delete m_D;
}

AWT::SimpleMesh::Connectivity::P AWT::SimpleMesh::Connectivity::getInstance( AWT::SimpleMesh::Mesh::P mesh )
{
   AUTOGETINSTANCE( AWT::SimpleMesh::Connectivity, ( mesh ) );
}

GETNAMEMACRO( AWT::SimpleMesh::Connectivity );

AWT::SimpleMesh::Mesh::P AWT::SimpleMesh::Connectivity::getMesh( )
{
   return m_D->mesh;
}

bool AWT::SimpleMesh::Connectivity::isVertexUsed( const AWT::SimpleMesh::Index v ) const
{
   return !m_D->facesUsingVertex[v].empty( );
}

bool AWT::SimpleMesh::Connectivity::isVertexOnBoundary( const AWT::SimpleMesh::Index v ) const
{
   // Get all of the faces using this vertex
   std::vector<Index> edges;

   std::vector<Index>::const_iterator iter    = m_D->facesUsingVertex[v].begin( );
   std::vector<Index>::const_iterator iterEnd = m_D->facesUsingVertex[v].end( );

   std::set<Index> flaggedVertices;

   // This method requires that the edges are all oriented in the same direction!
   while ( iter != iterEnd )
   {
      SimpleMesh::Face vs = m_D->mesh->getFace( *iter );

      // Search for the test vertex
      for ( int i = 0; i < 3; ++i )
      {
         // Go through and flag the edge on and off - each vertex can only
         // be visited twice, and it being visited once means that the cycle 
         // must go through the test vertex, i.e. the vertex is on
         // the edge
         if ( vs[i] == v )
         {
            for ( int offset = 1; offset <= 2; ++offset )
            {
               std::set<Index>::iterator f = flaggedVertices.find( vs[(i+offset)%3] );

               if ( f != flaggedVertices.end( ) )
                  flaggedVertices.erase( f );
               else
                  flaggedVertices.insert( vs[(i+offset)%3] );
            }
         }
      }
      
      ++iter;
   }
   
   return !flaggedVertices.empty();
}

bool AWT::SimpleMesh::Connectivity::isFaceOnBoundary( const AWT::SimpleMesh::Index f ) const
{
   Index vs[10];

   return getAdjacentFaces( f, vs ) != 3;
}

AWT::Container::Iterator<AWT::SimpleMesh::Index>::P AWT::SimpleMesh::Connectivity::getAdjacentVertices( const AWT::SimpleMesh::Index v ) const
{
   return Container::SetIterator<Index>::getInstance( m_D->adjVertices[v] );
}

AWT::Container::Iterator<AWT::SimpleMesh::Index>::P AWT::SimpleMesh::Connectivity::getFacesUsingVertex( const AWT::SimpleMesh::Index v ) const
{
   return Container::STLIterator<Index,std::vector<Index>>::getInstance( m_D->facesUsingVertex[v] );
}

AWT::SimpleMesh::Index AWT::SimpleMesh::Connectivity::getAdjacentFaces( const AWT::SimpleMesh::Index face, AWT::SimpleMesh::Index* vs ) const
{
   // Get the vertices of this face
   SimpleMesh::Face faceVertexIndices = m_D->mesh->getFace( face );

   Index ret = 0;

   for ( Index fvi = 0; fvi < 3; ++fvi )
   {
      std::vector<Index>::iterator facesUsingVertexItr = m_D->facesUsingVertex[ faceVertexIndices[fvi] ].begin( );
      std::vector<Index>::iterator endFacesUsingVertex = m_D->facesUsingVertex[ faceVertexIndices[fvi] ].end( );

      for ( ; facesUsingVertexItr != endFacesUsingVertex; ++facesUsingVertexItr )
      {
         // Don't want to consider the test face
         if ( *facesUsingVertexItr == face )
            continue;

         // Get the vertices which this face uses
         SimpleMesh::Face otherFaceVertexIndices = m_D->mesh->getFace( *facesUsingVertexItr );

         // Now, need to find if two of otherFacesVertexIndices equal two of faceVertexIndices
         // We have three edges in a, which we need to test against each of the 3 edges x 2 
         // (including flipping) in b

         bool isAdj = false;

         // Cyclically permute otherFaceVertexIndices to get common vertex into zero pos
         if ( otherFaceVertexIndices[0] != faceVertexIndices[fvi] )
         {
            Index swapIdx = ( otherFaceVertexIndices[1] == faceVertexIndices[fvi] ) ? 1 : 2;
            Index tmp;

            tmp = otherFaceVertexIndices[0];
            otherFaceVertexIndices[0] = otherFaceVertexIndices[swapIdx];
            otherFaceVertexIndices[swapIdx] = tmp;

            tmp = otherFaceVertexIndices[1];
            otherFaceVertexIndices[1] = otherFaceVertexIndices[2];
            otherFaceVertexIndices[2] = tmp;
         }

         // Now we know that otherFaceVertexIndices[0] == faceVertexIndices[fvi]
         // So now we need to know if 

         if ( otherFaceVertexIndices[1] == faceVertexIndices[(fvi+1)%3] )
            isAdj = true;
         else if ( otherFaceVertexIndices[1] == faceVertexIndices[(fvi+2)%3] )
            isAdj = true;
         else if ( otherFaceVertexIndices[2] == faceVertexIndices[(fvi+1)%3] )
            isAdj = true;
         else if ( otherFaceVertexIndices[2] == faceVertexIndices[(fvi+2)%3] )
            isAdj = true;

         if ( isAdj )
         {
            // Make sure that we've not added it already
            bool found = false;
            for ( unsigned int i = 0; i < ret; ++i )
               found |= vs[i] == *facesUsingVertexItr;

            areFacesAdjacent( *facesUsingVertexItr, face );

            if ( !found && areFacesAdjacent( *facesUsingVertexItr, face ) )
            {
               //PRINTVBL( *facesUsingVertexItr );
               vs[ret++] = *facesUsingVertexItr;
            }
         }

      }
   }

   // Do a sanity check to make sure that all faces returned do contain two of the reference vertices
   for ( Index r = 0; r < ret && false; ++r )
   {
      SimpleMesh::Face rvs = m_D->mesh->getFace( vs[r] );

      int count = 0;

      for ( int i = 0; i < 3; ++i )
         for ( int j = 0; j < 3; ++j )
         {
            if ( rvs[i] == faceVertexIndices[j] )
               ++count;
         }

      if ( count != 2 )
      {
         PRINTVEC( faceVertexIndices, 3 );
         PRINTVEC( rvs, 3 );
         PRINTVBL( count );
         DEBUGLINE;
         AWTEXCEPTIONTHROW( "!!!!!" );
      }
   }
   return ret;
}

AWT::SimpleMesh::Index AWT::SimpleMesh::Connectivity::getBoundaryNeighbours( const AWT::SimpleMesh::Index v, AWT::SimpleMesh::Index* ns ) const
{
   if ( !isVertexOnBoundary( v ) )
      return 0;

   Container::Iterator<Index>::P it = getAdjacentVertices( v );

   Index ret = 0;

   Index fs[2];

   while ( it->hasNext() )
   {
      Index vv = it->next();

      // We are only interested in points on the boundary, but...
      if ( isVertexOnBoundary( vv ) )
      {
         // ... we need to filter further, because although they are
         // both on /an/ edge, they might not be on the /same/ edge.
         if ( getFacesUsingEdge( v, vv, fs ) == 1 )
         {
            ns[ret++] = vv;
         }
      }
   }

   return ret;
}

AWT::SimpleMesh::Index AWT::SimpleMesh::Connectivity::getFacesUsingEdge(const AWT::SimpleMesh::Index va, const AWT::SimpleMesh::Index vb, AWT::SimpleMesh::Index *fs) const
{
   std::vector<Index>::iterator iter    = m_D->facesUsingVertex[va].begin();
   std::vector<Index>::iterator iterEnd = m_D->facesUsingVertex[va].end();

   Index ret = 0;

   while ( iter != iterEnd )
   {
      SimpleMesh::Face vis = m_D->mesh->getFace( *iter );

      if ( vis[0] == vb || vis[1] == vb || vis[2] == vb )
      {
         // Only store if we need to, otherwise it's just counting
         if ( fs != 0 )
            fs[ret] = *iter;

         ++ret;
      }

      ++iter;
   }

   return ret;
}

AWT::SimpleMesh::Index AWT::SimpleMesh::Connectivity::getFaceIndex( const AWT::SimpleMesh::Index va, const AWT::SimpleMesh::Index vb, const AWT::SimpleMesh::Index vc ) const
{
   Index vs[10];
   const Index nf = getFacesUsingEdge( va, vb, vs );

   //PRINTVBL( va );
   //PRINTVBL( vb );
   //PRINTVBL( vc );
   //PRINTVEC( vs, nf );

   for ( Index i = 0; i < nf; ++i )
   {
      SimpleMesh::Face vvs = m_D->mesh->getFace( vs[i] );

      //PRINTVEC( vvs, 3 );

      unsigned int count = 0;
      for ( int ax = 0; ax < 3; ++ax )
      {
         if ( vvs[ax] == vc )
            return vs[i];
      }

   }

   return INVALID_INDEX;
}

bool AWT::SimpleMesh::Connectivity::areFacesAdjacent( const AWT::SimpleMesh::Index a, const AWT::SimpleMesh::Index b ) const
{
   SimpleMesh::Face ais = m_D->mesh->getFace( a );
   SimpleMesh::Face bis = m_D->mesh->getFace( b );

   int count = 0;
   for ( int j = 0; j < 3; ++j )
      for ( int i = 0; i < 3; ++i )
         if ( ais[i] == bis[j] )
            ++count;

   return ( count == 2 );
}

AWT::SimpleMesh::Index AWT::SimpleMesh::Connectivity::aFaceUsingVertex( const AWT::SimpleMesh::Index v ) const
{
   return m_D->facesUsingVertex[v][0];
}