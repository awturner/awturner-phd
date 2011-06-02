#ifndef __BOXQUERIES_H__
#define __BOXQUERIES_H__

namespace Mimlab
{
   namespace Matrices
   {
      template <class T>
      class Matrix;
   }
}

#include "ObjectEKDTree/AxisAlignedBoundingBox.h"
#include "Mesh/Pose.h"

namespace AWT
{
   template <class T>
   class BoxQueries
   {
   public:
      static T getClosestDistance2( const T* in_TestPoint, typename AxisAlignedBoundingBox<T,3>* in_Branch, const T fastBreak );
      static T getFarthestDistance2( const T* in_TestPoint, typename AxisAlignedBoundingBox<T,3>* in_Branch, const T fastBreak );
      static bool rayIntersectsBox( const T* in_Origin, const T* in_Direction, typename AxisAlignedBoundingBox<T,3>* in_Branch, T* distance );

      static bool boxesIntersect( const typename AxisAlignedBoundingBox<T,3>::P in_A, const typename Pose<T>::P in_PoseA, 
      const typename AxisAlignedBoundingBox<T,3>::P in_B, const typename Pose<T>::P in_PoseB, bool debug = false );
   };
}

#endif // __BOXQUERIES_H__