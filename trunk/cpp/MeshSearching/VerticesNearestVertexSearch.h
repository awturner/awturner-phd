#ifndef __POINTSNEARESTPOINTSSEARCH_H__
#define __POINTSNEARESTPOINTSSEARCH_H__

#include "MeshSearching/VerticesSearch.h"
#include "MeshSearching/VerticesTreeData.h"

#include <limits>

#include "ObjectEKDTree/OEKDTreeBranch.h"
#include "MeshSearching/VerticesTreeData.h"

namespace AWT
{
   template <class T>
   class VerticesNearestVertexSearch : public VerticesSearch<T>
   {
   public:
      typedef ManagedAutoPointer< VerticesNearestVertexSearch<T> > P;
      static P getInstance( );

   protected:
      VerticesNearestVertexSearch( const T in_MaxDistance = std::numeric_limits<T>::infinity( ) );
      virtual ~VerticesNearestVertexSearch( );

   public:

      virtual void calculateBoxDistanceBounds2( typename OEKDTreeBranch<T,3>* in_Branch, SqDistBounds<T>& bounds  ) const;
      virtual bool shouldCheck( typename AWT::OEKDTree::OEKDTreeBranch<T,3>* in_Branch, const SqDistBounds<T>& bounds ) const;

      virtual void checkObject( const int in_Index );

      virtual void setTreeData( typename VerticesTreeData<T>::P data );
      virtual typename VerticesTreeData<T>::P getTreeData( );

      virtual void setTestPoint( const T* in_TestPoint  );
      virtual void getTestPoint( T* out_TestPoint ) const;

      virtual MeshIndex getNearestPoint( T* out_NearestPoint = 0 ) const;

      virtual void reset( );

      virtual std::string getClassName( ) const;

      void setData( typename OEKDTreeData<T,3>::P in_Data );
      typename typename VerticesTreeData<T>::P getData( );

   protected:
      struct D;
      D* m_D;
   };
}

#endif __POINTSNEARESTPOINTSSEARCH_H__