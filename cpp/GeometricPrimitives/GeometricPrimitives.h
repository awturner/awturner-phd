#ifndef __PRIMITIVES_GEOMETRICPRIMITIVES_H__
#define __PRIMITIVES_GEOMETRICPRIMITIVES_H__

#include "Point.h"
#include "Plane.h"
#include "Line.h"
#include "Sphere.h"
#include "Triangle.h"
#include "AxisAlignedBox.h"
#include "GeometryQueries.h"
#include "Assembly.h"

namespace AWT
{
   namespace GeometricPrimitives
   {
      typedef Point<double>           DoublePoint;
      typedef Plane<double>           DoublePlane;
      typedef Line<double>            DoubleLine;
      typedef Sphere<double>          DoubleSphere;
      typedef Triangle<double>        DoubleTriangle;
      typedef AxisAlignedBox<double>  DoubleAxisAlignedBox;
      typedef GeometryQueries<double> DoubleQueries;

      typedef Point<float>            FloatPoint;
      typedef Plane<float>            FloatPlane;
      typedef Line<float>             FloatLine;
      typedef Sphere<float>           FloatSphere;
      typedef Triangle<float>         FloatTriangle;
      typedef AxisAlignedBox<float>   FloatAxisAlignedBox;
      typedef GeometryQueries<float>  FloatQueries;

   }
}

#endif // __PRIMITIVES_GEOMETRICPRIMITIVES_H__