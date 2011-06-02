#include "VerticesNearestVertexSearch.h"

#include "ObjectEKDTree/OEKDTreeBranch.h"

#include "MeshSearching/BoxQueries.h"
#include "MeshSearching/PointQueries.h"

template <class T>
struct AWT::VerticesNearestVertexSearch<T>::D
{
   T         m_NearestPoint[3];
   int       m_NearestPointIndex;
   
   T m_InitMaxDistance2;
   T m_MaxDistance2;
   T m_TestPoint[3];

   typename VerticesTreeData<T>::P m_Data;
};

template <class T>
typename AWT::VerticesNearestVertexSearch<T>::P AWT::VerticesNearestVertexSearch<T>::getInstance( )
{
   AUTOGETINSTANCE( VerticesNearestVertexSearch<T>, ( ) );
}

template <class T>
AWT::VerticesNearestVertexSearch<T>::VerticesNearestVertexSearch( const T in_MaxDistance )
{
   m_D = new D;
   m_D->m_InitMaxDistance2 = in_MaxDistance*in_MaxDistance;

   reset( );
}

template <class T>
AWT::VerticesNearestVertexSearch<T>::~VerticesNearestVertexSearch( )
{
   delete m_D;
}

template <class T>
GETNAMEMACRO( AWT::VerticesNearestVertexSearch<T> );

template <class T>
void AWT::VerticesNearestVertexSearch<T>::calculateBoxDistanceBounds2( typename AWT::OEKDTree::OEKDTreeBranch<T,3>* in_Branch, SqDistBounds<T>& bounds ) const
{
   boxChecked();
   bounds.lower = BoxQueries<T>::getClosestDistance2( m_D->m_TestPoint, in_Branch, m_D->m_MaxDistance2 );
   bounds.upper = std::numeric_limits<T>::max();
}

template <class T>
bool AWT::VerticesNearestVertexSearch<T>::shouldCheck( typename AWT::OEKDTree::OEKDTreeBranch<T,3>* in_Branch, const SqDistBounds<T>& bounds ) const
{
   return bounds.lower < m_D->m_MaxDistance2;
}

template <class T>
void AWT::VerticesNearestVertexSearch<T>::setTreeData( typename VerticesTreeData<T>::P data )
{
   m_D->m_Data = data;
}

template <class T>
typename AWT::VerticesTreeData<T>::P AWT::VerticesNearestVertexSearch<T>::getTreeData( )
{
   return m_D->m_Data;
}

template <class T>
void AWT::VerticesNearestVertexSearch<T>::checkObject( const int in_Index )
{
   objectChecked();
   
   T point[3];
   m_D->m_Data->getPosition( in_Index, point );

   T dist2 = PointQueries<T>::getDistanceSquared( m_D->m_TestPoint, point, m_D->m_MaxDistance2 );
   if ( dist2 >= m_D->m_MaxDistance2 )
      return;
   
   m_D->m_MaxDistance2 = dist2;

   for ( int d = 0; d < 3; ++d )
      m_D->m_NearestPoint[d] = point[d];

   m_D->m_NearestPointIndex = in_Index;
}

template <class T>
void AWT::VerticesNearestVertexSearch<T>::setTestPoint( const T* in_TestPoint  )
{
   for ( int i = 0; i < 3; i++ )
      m_D->m_TestPoint[i] = in_TestPoint[i];
}

template <class T>
void AWT::VerticesNearestVertexSearch<T>::getTestPoint( T* out_TestPoint ) const
{
   for ( int i = 0; i < 3; i++ )
      out_TestPoint[i] = m_D->m_TestPoint[i];
}

template <class T>
void AWT::VerticesNearestVertexSearch<T>::reset( )
{
   VerticesSearch<T>::reset();

   m_D->m_NearestPointIndex = -1;
   m_D->m_MaxDistance2 = m_D->m_InitMaxDistance2;
}

template <class T>
AWT::MeshIndex AWT::VerticesNearestVertexSearch<T>::getNearestPoint( T* out_Point )const
{
   if ( out_Point != 0 )
   {
      for ( int d = 0; d < 3; ++d )
         out_Point[d] = m_D->m_NearestPoint[d];
   }

   return m_D->m_NearestPointIndex;
}

template <class T>
void AWT::VerticesNearestVertexSearch<T>::setData( typename AWT::OEKDTree::OEKDTreeData<T,3>::P data )
{
   VerticesTreeData<T>* castData = dynamic_cast<VerticesTreeData<T>*>( *data );
   if (castData == 0)
      AWTEXCEPTIONTHROW( "Invalid tree data!" );

   m_D->m_Data.set( castData );
}

template <class T>
typename AWT::VerticesTreeData<T>::P AWT::VerticesNearestVertexSearch<T>::getData( )
{
   return m_D->m_Data;
}

template class AWT::VerticesNearestVertexSearch<double>;
template class AWT::VerticesNearestVertexSearch<float>;
