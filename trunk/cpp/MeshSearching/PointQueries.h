#ifndef __POINTQUERIES_H__
#define __POINTQUERIES_H__

#include <limits>

namespace AWT
{
   /*! \class PointQueries
      \brief Class to perform calculations relating to points

      This provides ubiqutious calculations for points.
      */
   template <class T>
   class PointQueries
   {
   public:
      /**
      * \brief Gets the distance squared between two points
      *
      * Calculates the square of the Euclidean distance between
      * two points.  The optional in_FastBreak argument specifies
      * a distance at which to terminate the calculation if 
      * exceeded.  This is useful in e.g. nearest neighbour searching
      * so that you don't need to complete the calculation since
      * the cumulative distance squared never decreases as you add
      * axes.
      *
      * @param in_A The first point
      * @param in_B The second point
      * @param in_FastBreak Fast break condition (optional)
      * @return The singleton instance
      */
      static T getDistanceSquared( const T* in_A, const T* in_B, const T in_FastBreak = std::numeric_limits<T>::infinity( ) );
   };
}

#endif // __POINTQUERIES_H__