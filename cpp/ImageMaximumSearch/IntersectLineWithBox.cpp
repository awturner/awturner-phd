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
