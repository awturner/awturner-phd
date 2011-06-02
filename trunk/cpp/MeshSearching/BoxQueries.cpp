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
#include "BoxQueries.h"

#include "Mesh/Pose.h"

#include "MimlabMaths/SimpleFunctions.h"

#include "ObjectEKDTree/OEKDTreeBranch.h"

#include "ObjectEKDTree/AxisAlignedBoundingBox.h"

#include <algorithm>

using namespace Mimlab::Matrices;

template <class T>
T AWT::BoxQueries<T>::getClosestDistance2( const T* in_TestPoint, typename AxisAlignedBoundingBox<T,3>* in_Branch, const T fastBreak )
{
   T dist2 = 0;
   T tmp;

   for ( int ax = 0; ax < 3; ax++ )
   {
      tmp = std::min( std::max( in_TestPoint[ax], in_Branch->getMinimumBound( ax ) ), in_Branch->getMaximumBound( ax ) );

      tmp -= in_TestPoint[ax];

      dist2 += tmp*tmp;

      if ( dist2 > fastBreak )
         break;
   }

   return dist2;
}

template <class T>
T AWT::BoxQueries<T>::getFarthestDistance2( const T* in_TestPoint, typename AxisAlignedBoundingBox<T,3>* in_Branch, const T fastBreak )
{
   T dist2 = 0;
   T tmp;

   // Check the 8 vertices to see which is farthest away
   // Get the 8 vertices of the box and see which side of the plane they are on
   for ( int v = 0; v < 8; ++v )
   {
      T thisDist2 = 0;

      for ( int ax = 0; ax < 3; ++ax )
      {
         tmp = (v & (1 << ax)) ? in_Branch->getMinimumBound(ax) : in_Branch->getMaximumBound(ax) - in_TestPoint[ax];

         thisDist2 += tmp*tmp;

         if (thisDist2 > fastBreak)
            return thisDist2;
      }

      dist2 = std::max<T>(dist2, thisDist2);
   }

   return dist2;
}

template <class T>
bool AWT::BoxQueries<T>::rayIntersectsBox( const T* in_Origin, const T* in_Direction, typename AxisAlignedBoundingBox<T,3>* in_Branch, T* distance )
{
   T bounds[6];

   for ( int ax = 0; ax < 3; ++ax )
   {
      bounds[2*ax+0] = in_Branch->getMinimumBound( ax );
      bounds[2*ax+1] = in_Branch->getMaximumBound( ax );
   }

   T bestDistance = std::numeric_limits<T>::infinity( );
   T pnt[3];
   bool ret = false;

   for ( int ax = 0; ax < 3; ++ax )
   {
      if ( in_Direction[ax] == 0 )
         continue;

      for ( int side = 0; side < 2; ++side )
      {
         T den = in_Direction[ax];
         T num = in_Origin[ax] + bounds[2*ax + side];

         T lam = -num / den;

         bool isGood = true;
         for ( int axx = 0; axx < 3 && isGood; ++axx )
         {
            pnt[axx] = in_Origin[axx] + in_Direction[axx] * lam;

            if ( pnt[axx] < bounds[2*axx+0] || pnt[axx] > bounds[2*axx+1] )
               isGood = false;
         }

         if ( isGood )
         {
            ret = true;

            if ( abs( lam ) < bestDistance )
               bestDistance = abs( lam );
         }
      }
   }

   if ( distance != 0 )
      *distance = bestDistance;

   return ret;
}

template <class T>
bool AWT::BoxQueries<T>::boxesIntersect( const typename AxisAlignedBoundingBox<T,3>::P in_A, const typename AWT::Pose<T>::P in_PoseA, const typename AxisAlignedBoundingBox<T,3>::P in_B, const typename AWT::Pose<T>::P in_PoseB, bool debug )
{
   // Calculate the vector displacement between the boxes
   T centroidA[3];
   T centroidB[3];
   
   T a[3];
   T b[3];

   for ( int ax = 0; ax < 3; ++ax )
   {
      a[ax] = ( in_A->getMaximumBound( ax ) - in_A->getMinimumBound( ax ) ) / 2;
      b[ax] = ( in_B->getMaximumBound( ax ) - in_B->getMinimumBound( ax ) ) / 2;

      centroidA[ax] = ( in_A->getMaximumBound( ax ) + in_A->getMinimumBound( ax ) ) / 2;
      centroidB[ax] = ( in_B->getMaximumBound( ax ) + in_B->getMinimumBound( ax ) ) / 2;
   }

   T projCentroidA[3], projCentroidB[3];

   in_PoseA->project( centroidA, projCentroidA );
   in_PoseB->project( centroidB, projCentroidB );

   T d[] = {
      projCentroidB[0] - projCentroidA[0],
      projCentroidB[1] - projCentroidA[1],
      projCentroidB[2] - projCentroidA[2],
   };

   vnl_matrix_fixed<T,4,4> aa = in_PoseA->getMatrix( );
   vnl_matrix_fixed<T,4,4> bb = in_PoseB->getMatrix( );

   vnl_matrix_fixed<T,4,4> c = aa.transpose( ) * bb;

   if ( debug )
   {
      PRINTVBL( c );

      std::cerr << "d = [" << projCentroidA[0] << " " << projCentroidB[0] << ";" << std::endl;
      std::cerr << projCentroidA[1] << " " << projCentroidB[1] << ";" << std::endl;
      std::cerr << projCentroidA[2] << " " << projCentroidB[2] << "];" << std::endl;

      PRINTVEC( d, 3 );
   }

   T R0, R1, R;

#define OBBOBBTEST( dR0, dR1, dR ) { R0 = dR0; R1 = dR1; R = dR; \
   /*if ( debug ) std::cerr << "% " << R0 << " " << R1 << " " << (R0+R1) << " " << R << std::endl; \*/ \
   if ( R > R0 + R1 ) return false; }

   T AdotD[] = {
      aa[0][0]*d[0] + aa[1][0]*d[1] + aa[2][0]*d[2],
      aa[0][1]*d[0] + aa[1][1]*d[1] + aa[2][1]*d[2],
      aa[0][2]*d[0] + aa[1][2]*d[1] + aa[2][2]*d[2],
   };

   T BdotD[] = {
      bb[0][0]*d[0] + bb[1][0]*d[1] + bb[2][0]*d[2],
      bb[0][1]*d[0] + bb[1][1]*d[1] + bb[2][1]*d[2],
      bb[0][2]*d[0] + bb[1][2]*d[1] + bb[2][2]*d[2],
   };

   OBBOBBTEST( a[0], b[0]*abs(c[0][0]) + b[1]*abs(c[0][1]) + b[2]*abs(c[0][2]), abs( AdotD[0] ) );
   OBBOBBTEST( a[1], b[0]*abs(c[1][0]) + b[1]*abs(c[1][1]) + b[2]*abs(c[1][2]), abs( AdotD[1] ) );
   OBBOBBTEST( a[2], b[0]*abs(c[2][0]) + b[1]*abs(c[2][1]) + b[2]*abs(c[2][2]), abs( AdotD[2] ) );

   OBBOBBTEST( a[0]*abs(c[0][0]) + a[1]*abs(c[0][1]) + a[2]*abs(c[0][2]), b[0], abs( BdotD[0] ) );
   OBBOBBTEST( a[0]*abs(c[1][0]) + a[1]*abs(c[1][1]) + a[2]*abs(c[1][2]), b[1], abs( BdotD[1] ) );
   OBBOBBTEST( a[0]*abs(c[2][0]) + a[1]*abs(c[2][1]) + a[2]*abs(c[2][2]), b[2], abs( BdotD[2] ) );

   OBBOBBTEST( a[1]*abs(c[2][0]) + a[2]*abs(c[1][0]), b[1]*abs(c[0][2]) + b[2]*abs(c[0][1]), abs( c[1][0]*AdotD[2] - c[2][0]*AdotD[1] ) );
   OBBOBBTEST( a[1]*abs(c[2][1]) + a[2]*abs(c[1][1]), b[2]*abs(c[0][0]) + b[0]*abs(c[0][2]), abs( c[1][1]*AdotD[2] - c[2][1]*AdotD[1] ) );
   OBBOBBTEST( a[1]*abs(c[2][2]) + a[2]*abs(c[1][2]), b[0]*abs(c[0][1]) + b[1]*abs(c[0][0]), abs( c[1][2]*AdotD[2] - c[2][2]*AdotD[1] ) );

   OBBOBBTEST( a[2]*abs(c[0][0]) + a[0]*abs(c[2][0]), b[1]*abs(c[1][2]) + b[2]*abs(c[1][1]), abs( c[2][0]*AdotD[0] - c[0][0]*AdotD[2] ) );
   OBBOBBTEST( a[2]*abs(c[0][1]) + a[0]*abs(c[2][1]), b[2]*abs(c[1][0]) + b[0]*abs(c[1][2]), abs( c[2][1]*AdotD[0] - c[0][1]*AdotD[2] ) );
   OBBOBBTEST( a[2]*abs(c[0][2]) + a[0]*abs(c[2][2]), b[0]*abs(c[1][1]) + b[1]*abs(c[1][0]), abs( c[2][2]*AdotD[0] - c[0][2]*AdotD[2] ) );

   OBBOBBTEST( a[0]*abs(c[1][0]) + a[1]*abs(c[0][0]), b[1]*abs(c[2][2]) + b[2]*abs(c[2][1]), abs( c[0][0]*AdotD[1] - c[1][0]*AdotD[0] ) );
   OBBOBBTEST( a[0]*abs(c[1][1]) + a[1]*abs(c[0][1]), b[2]*abs(c[2][0]) + b[0]*abs(c[2][2]), abs( c[0][1]*AdotD[1] - c[1][1]*AdotD[0] ) );
   OBBOBBTEST( a[0]*abs(c[1][2]) + a[1]*abs(c[0][2]), b[0]*abs(c[2][1]) + b[1]*abs(c[2][0]), abs( c[0][2]*AdotD[1] - c[1][2]*AdotD[0] ) );

#undef OBBOBBTEST

   return true;
}

template class AWT::BoxQueries<double>;
template class AWT::BoxQueries<float>;