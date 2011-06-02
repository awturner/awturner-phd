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
#include "IntersectLineWithBox.h"


double AWT::intersectLineWithPlane( double* pointOnPlane, double* planeNormal, double* pointOnLine, double* lineDirection, double* out_Point )
{
   double num = 0;
   double den = 0;

   for ( int i = 0; i < 3; ++i )
   {
      num += ( pointOnPlane[i] - pointOnLine[i] ) * planeNormal[i];
      den += lineDirection[i] * planeNormal[i];
   }

   double lambda = num / den;

   for ( int i = 0; i < 3; ++i )
      out_Point[i] = pointOnLine[i] + lambda*lineDirection[i];

   return lambda;
}

void AWT::intersectLineWithBox( double* bounds, double* pointOnLine, double* direction, double** out_Points, unsigned int& out_NPoints )
{
   double pointOnPlane[3];
   double normal[3];
   double intersection[3];
   
   out_NPoints = 0;

   for ( int p = 0; p < 2; ++p )
   {
      for ( int i = 0; i < 3; ++i )
         pointOnPlane[i] = bounds[2*i + p];

      for ( int q = 0; q < 3; ++q )
      {
         int r;
         for ( r = 0; r < 3; ++r )
            normal[r] = ( q == r ) ? (( p == 0 ) ? -1 : 1 ) : 0;

         if ( (0 < intersectLineWithPlane( pointOnPlane, normal, pointOnLine, direction, out_Points[out_NPoints] )) || true )
         {
            for ( r = 0; r < 3; ++r )
            {
               if ( r == q )
                  continue;

               if ( out_Points[out_NPoints][r] < bounds[2*r+0] || out_Points[out_NPoints][r] > bounds[2*r+1] )
                  break;
            }

            if ( r == 3 )
               if ( ++out_NPoints == 2 )
                  return;
         }
      }
   }
}
