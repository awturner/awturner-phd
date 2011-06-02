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
/*
#ifndef __SURFACE_H__
#define __SURFACE_H__

#include "Points.h"

namespace AWT
{
   template <class CoordinateType>
   class Surface : public Points<CoordinateType>
   {
   public:
      virtual GeometryIndex getNumberOfFaces() const = 0;

      virtual void             getFace(const GeometryIndex faceIndex, GeometryIndex* outVertexIndices) const = 0;
      virtual void             getFaceVertex(const GeometryIndex faceIndex, const GeometryIndex faceVertexIndex, double* outVertexLocation) const = 0;

      virtual void             getFaceNormal(const GeometryIndex faceIndex, double* outFaceNormal) const = 0;
      virtual void             getVertexNormal(const GeometryIndex vertexIndex, CoordinateType* outVertexNormal) const = 0;

      virtual bool             findClosestOnSurface(const double* inTrialPoint, GeometryIndex& outIndex, double* outPoint) const = 0;
   };
}

#endif // __SURFACE_H__
*/