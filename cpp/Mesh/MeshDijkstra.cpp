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
#include "MeshDijkstra.h"

#include "Mesh.h"
#include "Tuples.h"
#include "TuplesImpl.h"
#include "MeshConnectivity.h"
#include "MeshSearching/PointQueries.h"
#include "Useful/Iterator.h"



#include <queue>
#include <vector>
#include <limits>

template <class T>
struct AWT::MeshDijkstra<T>::D
{
   // Implemented with reference to
   // http://gillou.cjb.net/research/files/Dijkstra%20optimized%20implementation%20via%20STL%20library.html
   void calculate( )
   {
      typedef std::pair<T,MeshIndex> ii;
      typedef std::vector<ii>        vii;
      typedef std::vector<vii>       vvii;

      std::priority_queue<ii, vii, std::greater<ii>> Q;

      // Initialize the queue with all of the initial data
      for ( MeshIndex v = 0, vmax = m_PointData->getNumberOfPoints(); v < vmax; ++v )
      {
         T val = m_PointData->getPointElement( v, 0 );

         if ( val < std::numeric_limits<T>::max( ) )
            Q.push( ii( val, v ) );
      }

      // Calculate the connectivity so we know which vertices have edges between them
      MeshConnectivity<T>::P meshConn = MeshConnectivity<T>::getInstance( m_Mesh );

      while ( !Q.empty( ) )
      {
         // Fetch the top element, i.e. smallest onward distance
         ii top = Q.top( );
         Q.pop( );

         MeshIndex v = top.second;
         T         d = top.first;

         //DEBUGMACRO( "I am at vertex " << v << ", where the distance is " << d );
         
         if ( d <= m_PointData->getPointElement( v, 0 ) )
         {
            // Loop through all of the vertices adjacent to this one
            typename Container::Iterator<MeshIndex>::P it = meshConn->getAdjacentVertices( v );

            while ( it->hasNext( ) )
            {
               MeshIndex vn = it->next( );

               T newDist = d + calculateDistance( v, vn );

               if ( newDist < m_PointData->getPointElement( vn, 0 ) )
               {
                  //DEBUGMACRO( "\tVertex " << vn << " is currently " << m_PointData->getPointElement( vn, 0 ) << "; I will try to update it to " << newDist );
                  m_PointData->setPointElement( vn, 0, newDist );
                  Q.push( ii( newDist, vn ) );
               }
            }
         }

         //system( "pause" );
      }
   }

   T calculateDistance( MeshIndex va, MeshIndex vb )
   {
      T vtxA[3], vtxB[3];

      m_Mesh->getVertex( va, vtxA );
      m_Mesh->getVertex( vb, vtxB );

      return sqrt( PointQueries<T>::getDistanceSquared( vtxA, vtxB ) );
   }

   typename Mesh<T>::P   m_Mesh;
   typename Tuples<T>::P m_PointData;
};

template <class T>
AWT::MeshDijkstra<T>::MeshDijkstra( typename Mesh<T>::P mesh, typename Tuples<T>::P initialData )
{
   if ( initialData->getTupleSize( ) != 1 )
      AWTEXCEPTIONTHROW( "Initial data must have only 1 element per point" );

   m_D = new D;

   m_D->m_Mesh = mesh;

   m_D->m_PointData = initialData;

   // Actually do the calculation
   m_D->calculate( );
}

template <class T>
AWT::MeshDijkstra<T>::~MeshDijkstra( )
{
   delete m_D;
}

template <class T>
typename AWT::MeshDijkstra<T>::P AWT::MeshDijkstra<T>::getInstance( typename Mesh<T>::P mesh, typename Tuples<T>::P initialData )
{
   AUTOGETINSTANCE( AWT::MeshDijkstra<T>, ( mesh, initialData ) );
}

template <class T>
GETNAMEMACRO( AWT::MeshDijkstra<T> );

template <class T>
typename AWT::Tuples<T>::P AWT::MeshDijkstra<T>::getPointData( )
{
   return m_D->m_PointData;
}

template class AWT::MeshDijkstra<double>;
template class AWT::MeshDijkstra<float>;