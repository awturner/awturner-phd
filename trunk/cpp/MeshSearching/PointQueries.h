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
      static T getDistanceSquared(const T* in_A, const T* in_B, const T in_FastBreak = std::numeric_limits<T>::infinity());
   };
}

#endif // __POINTQUERIES_H__