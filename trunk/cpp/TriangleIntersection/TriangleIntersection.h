#ifndef __TRIANGLEINTERSECTION_H__
#define __TRIANGLEINTERSECTION_H__

namespace AWT
{
   template <class T>
   class TriangleIntersection
   {
      bool intersects( T u0[3], T u1[3], T u2[3],
         T v0[3], T v1[3], T v2[3],
         T* intersection0, T* intersection1 );
   };
}

#endif __TRIANGLEINTERSECTION_H__