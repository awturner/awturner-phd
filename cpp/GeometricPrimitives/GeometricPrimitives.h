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