#include "GeometricPrimitives.h"

namespace AWT
{
   namespace GeometricPrimitives
   {
      template Point<double>;
      template Plane<double>;
      template Line<double>;
      template Sphere<double>;
      template Triangle<double>;
      template AxisAlignedBox<double>;
      template GeometryQueries<double>;
      
      template Assembly<double, Point<double>>;

      template Point<float>;
      template Plane<float>;
      template Line<float>;
      template Sphere<float>;
      template Triangle<float>;
      template AxisAlignedBox<float>;
      template GeometryQueries<float>;
   }
}