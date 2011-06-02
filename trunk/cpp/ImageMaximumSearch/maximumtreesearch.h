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
#ifndef __MAXIMUMTREESEARCH_H__
#define __MAXIMUMTREESEARCH_H__

#include "CImg.h"
#include "ImageMaximumTree.h"
#include "IntersectLineWithBox.h"
#include "WeightFunction.h"

namespace AWT
{
   template <class T>
   class MaximumTreeSearch
   {
   public:
      static bool findMaximum( cimg_library::CImg<T>& im, double* pointOnLine, double* lineDirection, WeightFunction<double>* weightFunc, double threshold, double* out_point );

      static bool findMaximum( cimg_library::CImg<T>& im, double* pointOnLine, double* lineDirection, WeightFunction<double>* weightFunc, double threshold, ImageMaximumTree<T>* tree, double* out_point );

      static void setDebugImage( cimg_library::CImg<T>* im )
      {
         debugIm = im;
      }

   protected:
      static cimg_library::CImg<T>* debugIm;

      static void findMaximumRecursive( cimg_library::CImg<T>& im, double* pointOnLine, double* lineDirection, WeightFunction<double>* weightFunc, ImageMaximumTree<T>* tree, T& bestValue, double* out_point );
   };
}

template <class T>
cimg_library::CImg<T>* AWT::MaximumTreeSearch<T>::debugIm = 0;

template <class T>
bool AWT::MaximumTreeSearch<T>::findMaximum( cimg_library::CImg<T>& im, double* pointOnLine, double* lineDirection, WeightFunction<double>* weightFunc, double threshold, double* out_point )
{
   ImageMaximumTree<T>* tree = ImageMaximumTree<T>::buildTree( im );

   bool ret = findMaximum( im, pointOnLine, lineDirection, weightFunc, threshold, tree, out_point );

   delete tree;

   return ret;
}

template <class T>
bool AWT::MaximumTreeSearch<T>::findMaximum( cimg_library::CImg<T>& im, double* pointOnLine, double* lineDirection, WeightFunction<double>* weightFunc, double threshold, ImageMaximumTree<T>* tree, double* out_point )
{
   T bestValue = threshold; //-std::numeric_limits<T>::infinity( );
   findMaximumRecursive( im, pointOnLine, lineDirection, weightFunc, tree, bestValue, out_point );

   // Check if bestValue has changed and is not NaN
   return bestValue != 0 && bestValue == bestValue;
}

template <class T>
void AWT::MaximumTreeSearch<T>::findMaximumRecursive( cimg_library::CImg<T>& im, double* pointOnLine, double* lineDirection, WeightFunction<double>* weightFunc, ImageMaximumTree<T>* tree, T& bestValue, double* out_point )
{
   double** intersectionPoint = new double*[2];
   unsigned int nintersectionPoint;

   if ( debugIm != 0 )
   {
      T red[3]   = { 255,   0,   0 };
      T white[3] = { 255, 255, 255 };
      debugIm->draw_rectangle( tree->getMinimumBound( 0 ), tree->getMinimumBound( 1 ), 
         tree->getMaximumBound( 0 ), tree->getMaximumBound( 1 ), 
         red, 1, -1 );
   }

   intersectionPoint[0] = new double[3];
   intersectionPoint[1] = new double[3];

   if ( tree->getChild( 0 ) != 0 )
   {
      int order[] = { 0, 1 };
      double bestLambda[] = { 0, 0 };
      double bounds[6];

      // Let's calculate the upper bound on the maximum value here
      for ( int i = 0; i < 2; ++i )
      {
         ImageMaximumTree<T>* child = tree->getChild( order[i] );

         for ( int j = 0; j < 3; ++j )
         {
            bounds[2*j+0] = child->getMinimumBound( j );
            bounds[2*j+1] = child->getMaximumBound( j );
         }

         intersectLineWithBox( bounds, pointOnLine, lineDirection, intersectionPoint, nintersectionPoint );

         // If there were no intersections, then we aren't interested in looking in this child
         if ( nintersectionPoint == 0 )
         {
            bestLambda[i] = -std::numeric_limits<double>::infinity( );
            continue;
         }

         // Look at the intersections calculated, and find which one is closer
         int bestIntersection = -1;

         if ( pointOnLine[0] >= bounds[0] && pointOnLine[0] < bounds[1] && 
            pointOnLine[1] >= bounds[2] && pointOnLine[1] < bounds[3] && 
            pointOnLine[2] >= bounds[4] && pointOnLine[2] < bounds[5] )
         {
            // The test point is inside the box
            bestLambda[i] = 0;
         }
         else
         {
            bestLambda[i] = std::numeric_limits<double>::infinity( );
            // Work out which of the intersection points is closer
            for ( int is = 0; is < nintersectionPoint; ++is )
            {
               double lambda = 0;
               double tmp = 0;

               for ( int d = 0; d < 3; ++d )
               {
                  lambda += ( intersectionPoint[is][d] - pointOnLine[d] ) * lineDirection[d];
               }

               if ( is == 0 || abs(lambda) < bestLambda[i] )
               {
                  bestLambda[i] = lambda;
                  bestIntersection = is;
               }
            }
         }

         // Now we have which one is closest, calculate the upper bound on max value
         bestLambda[i] = weightFunc->calculate( bestLambda[i] ) * child->getValue( );
      }

      // Swap the order around
      if ( bestLambda[0] < bestLambda[1] )
         order[1] = 1 - ( order[0] = 1 );

      for ( int i = 0; i < 2; ++i )
      {
         ImageMaximumTree<T>* child = tree->getChild( order[i] );

         if ( bestLambda[order[i]] > bestValue )
         {
            for ( int j = 0; j < 3; ++j )
            {
               bounds[2*j+0] = child->getMinimumBound( j );
               bounds[2*j+1] = child->getMaximumBound( j );
            }

            double tmpPoint[] = {0,0,0};
            T tmpBest = bestValue;
            findMaximumRecursive( im, pointOnLine, lineDirection, weightFunc, child, tmpBest, tmpPoint );

            if ( tmpBest > bestValue )
            {
               bestValue = tmpBest;
               for ( int j = 0; j < 3; ++j )
                  out_point[j] = tmpPoint[j];
            }
         }
      }
   }
   else
   {
      double bounds[6];
      T interp = 0;
      bool flag = false;

      T tmpBestValue = -std::numeric_limits<T>::max( );
      
      int minBounds[3];
      int maxBounds[3];

      for ( int i = 0; i < 3; ++i )
      {
         minBounds[i] = static_cast<int>( tree->getMinimumBound( i ) + 0.5 );
         maxBounds[i] = static_cast<int>( tree->getMaximumBound( i ) + 0.5 );
      }

      for ( int z = minBounds[2]; z < maxBounds[2]; ++z )
         for ( int y = minBounds[1]; y < maxBounds[1]; ++y )
            for ( int x = minBounds[0]; x < maxBounds[0]; ++x )
            {

               for ( int i = 0; i < 2; ++i )
               {
                  bounds[0] = x - 0.5 + i;
                  bounds[1] = bounds[0] + 1;

                  for ( int j = 0; j < 2; ++j )
                  {
                     bounds[2] = y - 0.5 + j;
                     bounds[3] = bounds[2] + 1;

                     for ( int k = 0; k < 2; ++k )
                     {
                        bounds[4] = z - 0.5 + k;
                        bounds[5] = bounds[4] + 1;

                        intersectLineWithBox( bounds, pointOnLine, lineDirection, intersectionPoint, nintersectionPoint );

                        for ( int m = 0; m < nintersectionPoint; ++m )
                        {
                           im.linear_at3( intersectionPoint[m][0], intersectionPoint[m][1], intersectionPoint[m][2], 0, interp );

                           if ( interp > tmpBestValue )
                           {
                              tmpBestValue = interp;
                              out_point[0] = intersectionPoint[m][0];
                              out_point[1] = intersectionPoint[m][1];
                              out_point[2] = intersectionPoint[m][2];
                              flag = true;
                           }
                        }
                     }
                  }
               }
            }

      bestValue = tmpBestValue;
      
      if ( !flag )
      {
         for ( int i = 0; i < 3; ++i )
            out_point[i] = ( tree->getMinimumBound( i ) + tree->getMaximumBound( i ) ) / 2;

         bestValue = tree->getValue( );
      }
   }

   delete intersectionPoint[0];
   delete intersectionPoint[1];
   delete intersectionPoint;
}

#endif // __MAXIMUMTREESEARCH_H__