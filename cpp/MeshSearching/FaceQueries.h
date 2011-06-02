#ifndef __FACEQUERIES_H__
#define __FACEQUERIES_H__

#include "ObjectEKDTree/OEKDTreeData.h"

#include "Mesh/Mesh.h"

namespace AWT
{
   template <class T>
   class FaceQueries
   {
   public:
      static void getNormal                ( const typename OEKDTree::OEKDTreeData<T,3>::P in_Data, const int in_Index, T* out_Normal );
      static void getNearestPointOnTriangle( const T* in_TestPoint, const typename OEKDTree::OEKDTreeData<T,3>::P in_Data, const int in_Index, T* out_Nearest );
      static void getNearestPointOnTriangle( const T* in_TestPoint, const T* pnt0, const T* pnt1, const T* pnt2, T* out_Nearest );
      static void getCentroid              ( const typename OEKDTree::OEKDTreeData<T,3>::P in_Data, const int in_Index, T* out_Centroid );
      static bool rayIntersectsTriangle    ( const typename OEKDTree::OEKDTreeData<T,3>::P in_Data, const int in_Index, const T* origin, const T* direction, T* intersectionPoint, T* distance );
   };
}

#endif // __FACEQUERIES_H__