#include "MeshConnectivity.h"



#include "Useful/SetIterator.h"

#include "Mesh.h"
#include "TuplesImpl.h"
#include <limits>
#include <vector>
#include <set>

template <class T>
struct AWT::MeshConnectivity<T>::D
{
   typename Mesh<T>::P m_Mesh;
   typename Tuples<MeshIndex>::P m_AdjacentFaces;

   std::vector<std::vector<MeshIndex>>   m_FacesUsingVertex;
   std::vector<std::set<MeshIndex>>      m_AdjacentVertices;
};

template <class T>
AWT::MeshConnectivity<T>::MeshConnectivity( typename AWT::Mesh<T>::P mesh )
{
   m_D = new D;

   MeshIndex indices[] = { std::numeric_limits<MeshIndex>::max( ), std::numeric_limits<MeshIndex>::max( ), std::numeric_limits<MeshIndex>::max( ) };

   m_D->m_AdjacentFaces = TuplesImpl<MeshIndex>::getInstance( 3, indices, mesh->getNumberOfFaces( ) );
   
   m_D->m_Mesh = mesh;

   // This builds a huge list of the faces which use each vertex
   {
      m_D->m_FacesUsingVertex.clear( );

      //for ( MeshIndex v = 0; v < mesh->getNumberOfVertices( ); ++v )
      MESH_EACHVERTEX( mesh, v )
      {
         // Create a holder for this vertex's using cells
         m_D->m_FacesUsingVertex.push_back( std::vector<MeshIndex>( ) );
         m_D->m_AdjacentVertices.push_back( std::set<MeshIndex>( ) );
      }

      MeshIndex vs[3];
      //for ( MeshIndex v = 0; v < mesh->getNumberOfFaces( ); ++v )
      MESH_EACHFACE( mesh, f )
      {
         mesh->getFaceIndices( f, vs );

         for ( int i = 0; i < 3; ++i )
         {
            m_D->m_AdjacentVertices[ vs[i] ].insert( vs[(i+1)%3] );
            m_D->m_AdjacentVertices[ vs[i] ].insert( vs[(i+2)%3] );

            m_D->m_AdjacentVertices[ vs[(i+1)%3] ].insert( vs[i] );
            m_D->m_AdjacentVertices[ vs[(i+2)%3] ].insert( vs[i] );
         }

         m_D->m_FacesUsingVertex[ vs[0] ].push_back( f );
         m_D->m_FacesUsingVertex[ vs[1] ].push_back( f );
         m_D->m_FacesUsingVertex[ vs[2] ].push_back( f );
      }
   }
   
 
}

template <class T>
AWT::MeshConnectivity<T>::~MeshConnectivity( )
{
   delete m_D;
}

template <class T>
typename AWT::MeshConnectivity<T>::P AWT::MeshConnectivity<T>::getInstance( typename AWT::Mesh<T>::P mesh )
{
   AUTOGETINSTANCE( AWT::MeshConnectivity<T>, ( mesh ) );
}

template <class T>
GETNAMEMACRO( AWT::MeshConnectivity<T> );

template <class T>
typename AWT::Mesh<T>::P AWT::MeshConnectivity<T>::getMesh( )
{
   return m_D->m_Mesh;
}

template <class T>
bool AWT::MeshConnectivity<T>::isVertexUsed( const AWT::MeshIndex v ) const
{
   return !m_D->m_FacesUsingVertex[v].empty( );
}

template <class T>
bool AWT::MeshConnectivity<T>::isVertexOnBoundary( const AWT::MeshIndex v ) const
{
   // Get all of the faces using this vertex
   MeshIndex vs[3];
   std::vector<MeshIndex> edges;

   std::vector<MeshIndex>::const_iterator iter    = m_D->m_FacesUsingVertex[v].begin( );
   std::vector<MeshIndex>::const_iterator iterEnd = m_D->m_FacesUsingVertex[v].end( );

   std::set<MeshIndex> flaggedVertices;

   // This method requires that the edges are all oriented in the same direction!
   while ( iter != iterEnd )
   {
      m_D->m_Mesh->getFaceIndices( *iter, vs );

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
               std::set<MeshIndex>::iterator f = flaggedVertices.find( vs[(i+offset)%3] );

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

template <class T>
bool AWT::MeshConnectivity<T>::isFaceOnBoundary( const AWT::MeshIndex face ) const
{
   MeshIndex vs[3];
   return getAdjacentFaces( face, vs ) != 3;
}

template <class T>
typename AWT::Container::Iterator<AWT::MeshIndex>::P AWT::MeshConnectivity<T>::getAdjacentVertices( const AWT::MeshIndex v ) const
{
   return Container::SetIterator<MeshIndex>::getInstance( m_D->m_AdjacentVertices[v] );
}

template <class T>
typename AWT::Container::Iterator<AWT::MeshIndex>::P AWT::MeshConnectivity<T>::getFacesUsingVertex( const AWT::MeshIndex v ) const
{
   return Container::STLIterator<MeshIndex,std::vector<MeshIndex>>::getInstance( m_D->m_FacesUsingVertex[v] );
}

template <class T>
AWT::MeshIndex AWT::MeshConnectivity<T>::getAdjacentFaces( const AWT::MeshIndex face, AWT::MeshIndex* vs ) const
{
   // Get the vertices of this face
   MeshIndex faceVertexIndices[3];
   m_D->m_Mesh->getFaceIndices( face, faceVertexIndices );

   MeshIndex ret = 0;

   MeshIndex otherFaceVertexIndices[3];

   for ( MeshIndex fvi = 0; fvi < 3/* && ret < 3*/; ++fvi )
   {
      std::vector<MeshIndex>::iterator facesUsingVertexItr = m_D->m_FacesUsingVertex[ faceVertexIndices[fvi] ].begin( );
      std::vector<MeshIndex>::iterator endFacesUsingVertex = m_D->m_FacesUsingVertex[ faceVertexIndices[fvi] ].end( );

      for ( ; facesUsingVertexItr != endFacesUsingVertex && ret < 3; ++facesUsingVertexItr )
      {
         // Don't want to consider the test face
         if ( *facesUsingVertexItr == face )
            continue;

         // Get the vertices which this face uses
         m_D->m_Mesh->getFaceIndices( *facesUsingVertexItr, otherFaceVertexIndices );

         // Now, need to find if two of otherFacesVertexIndices equal two of faceVertexIndices
         // We have three edges in a, which we need to test against each of the 3 edges x 2 
         // (including flipping) in b

         bool isAdj = false;

         // Cyclically permute otherFaceVertexIndices to get common vertex into zero pos
         if ( otherFaceVertexIndices[0] != faceVertexIndices[fvi] )
         {
            MeshIndex swapIdx = ( otherFaceVertexIndices[1] == faceVertexIndices[fvi] ) ? 1 : 2;
            MeshIndex tmp;

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

            if ( !found && areFacesAdjacent( *facesUsingVertexItr, face ) )
            {
               //PRINTVBL( *facesUsingVertexItr );
               vs[ret++] = *facesUsingVertexItr;
            }
         }
      }
   }

   // Do a sanity check to make sure that all faces returned do contain two of the reference vertices
   for ( MeshIndex r = 0; r < ret && false; ++r )
   {
      MeshIndex rvs[3];
      m_D->m_Mesh->getFaceIndices( vs[r], rvs );

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

template <class T>
AWT::MeshIndex AWT::MeshConnectivity<T>::getBoundaryNeighbours( const AWT::MeshIndex v, AWT::MeshIndex* ns ) const
{
   if ( !isVertexOnBoundary( v ) )
      return 0;

   Container::Iterator<MeshIndex>::P it = getAdjacentVertices( v );

   MeshIndex ret = 0;

   MeshIndex fs[2];

   while ( it->hasNext() )
   {
      MeshIndex vv = it->next();

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

template <class T>
AWT::MeshIndex AWT::MeshConnectivity<T>::getFacesUsingEdge(const AWT::MeshIndex va, const AWT::MeshIndex vb, AWT::MeshIndex *fs) const
{
   // Get all of the faces using va
   std::vector<MeshIndex>* fuva = &m_D->m_FacesUsingVertex[va];
   // Filter this set to find the ones using vb also

   std::vector<MeshIndex>::iterator iter    = m_D->m_FacesUsingVertex[va].begin();
   std::vector<MeshIndex>::iterator iterEnd = m_D->m_FacesUsingVertex[va].end();

   MeshIndex ret = 0;

   MeshIndex vis[3];

   while ( iter != iterEnd )
   {
      m_D->m_Mesh->getFaceIndices( *iter, vis );

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

template <class T>
AWT::MeshIndex AWT::MeshConnectivity<T>::getFaceIndex( const AWT::MeshIndex va, const AWT::MeshIndex vb, const AWT::MeshIndex vc ) const
{
   MeshIndex vs[10], vvs[3];
   const MeshIndex nf = getFacesUsingEdge( va, vb, vs );

   //PRINTVBL( va );
   //PRINTVBL( vb );
   //PRINTVBL( vc );
   //PRINTVEC( vs, nf );

   for ( MeshIndex i = 0; i < nf; ++i )
   {
      m_D->m_Mesh->getFaceIndices( vs[i], vvs );

      //PRINTVEC( vvs, 3 );

      unsigned int count = 0;
      for ( int ax = 0; ax < 3; ++ax )
      {
         if ( vvs[ax] == vc )
            return vs[i];
      }

   }

   return std::numeric_limits<MeshIndex>::max( );
}

template <class T>
bool AWT::MeshConnectivity<T>::areFacesAdjacent( const AWT::MeshIndex a, const AWT::MeshIndex b ) const
{
   MeshIndex ais[3], bis[3];

   m_D->m_Mesh->getFaceIndices( a, ais );
   m_D->m_Mesh->getFaceIndices( b, bis );

   int count = 0;
   for ( int j = 0; j < 3; ++j )
      for ( int i = 0; i < 3; ++i )
         if ( ais[i] == bis[j] )
            ++count;

   return ( count == 2 );
}

template class AWT::MeshConnectivity<double>;
template class AWT::MeshConnectivity<float>;