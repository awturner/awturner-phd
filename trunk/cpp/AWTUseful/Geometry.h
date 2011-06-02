#ifndef __AWT_GEOMETRY_H__
#define __AWT_GEOMETRY_H__

#include "AWTUseful/Matrices.h"
#include <cmath>

namespace AWT
{
   namespace Geometry2D
   {
      double distance( const double* in_p0, const double* in_p1, const int in_npoints )
      {
         double distanceSq = 0.0;
         double v;

         for ( int i = 0; i < in_npoints; i++ )
         {
            v = in_p1[i] - in_p0[i];
            distanceSq += v*v;
         }

         v = sqrt( distanceSq * distanceSq );
         return v;
      }

      bool calculateCircleIntersects( double* in_p0, double in_r0, 
         double* in_p1, double in_r1, double* out_p3, double* out_p4 )
      {
         // http://local.wasp.uwa.edu.au/~pbourke/geometry/2circle/

         in_r0 = abs( in_r0 );
         in_r1 = abs( in_r1 );

         double dx = in_p1[0] - in_p0[0];
         double dy = in_p1[1] - in_p0[1];

         double d = sqrt( dx*dx + dy*dy );

         if ( d == 0 || ( d > in_r0 + in_r1 ) || ( d < abs( in_r0 - in_r1 ) ) )
            return false;

         double a = ( in_r0*in_r0 - in_r1*in_r1 + d*d ) / ( 2 * d );

         if ( a > in_r0 )
            return false;

         double h = sqrt( in_r0*in_r0 - a*a ) / d;

         double p2x = in_p0[0] + dx * a / d;
         double p2y = in_p0[1] + dy * a / d;

         out_p3[0] = p2x + h * dy;
         out_p3[1] = p2y - h * dx;

         out_p4[0] = p2x - h * dy;
         out_p4[1] = p2y + h * dx;

         return true;
      }
      
      void lineThroughPoints( double* in_point0, double* in_point1,
         double* out_line )
      {
         AWT::Vector<double,3>::crossProduct( in_point0, in_point1, out_line );
      }

      void calculateLineIntersects( double* in_line0, double* in_line1,
         double* out_intersect )
      {
         AWT::Vector<double,3>::crossProduct( in_line0, in_line1, out_intersect );
      }
   }
}

#endif // __AWT_GEOMETRY_H__