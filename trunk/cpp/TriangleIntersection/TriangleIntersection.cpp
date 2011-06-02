#include "TriangleIntersection.h"
#include "TriangleIntersection_Moller.h"

template <class T>
bool AWT::TriangleIntersection<T>::intersects( T u0[3], T u1[3], T u2[3],
         T v0[3], T v1[3], T v2[3],
         T* intersection0, T* intersection1 )
{
   int coplanar;
   int result = MollerTriangleIntersection<T>::tri_tri_intersect_with_isectline( u0, u1, u2, v0, v1, v2, &coplanar, intersection0, intersection1 );

   return result != 0;
}

template class AWT::TriangleIntersection<double>;
template class AWT::TriangleIntersection<float>;