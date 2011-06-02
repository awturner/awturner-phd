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
      virtual GeometryIndex getNumberOfFaces( ) const = 0;

      virtual void             getFace( const GeometryIndex faceIndex, GeometryIndex* outVertexIndices ) const = 0;
      virtual void             getFaceVertex( const GeometryIndex faceIndex, const GeometryIndex faceVertexIndex, double* outVertexLocation ) const = 0;

      virtual void             getFaceNormal( const GeometryIndex faceIndex, double* outFaceNormal ) const = 0;
      virtual void             getVertexNormal( const GeometryIndex vertexIndex, CoordinateType* outVertexNormal ) const = 0;

      virtual bool             findClosestOnSurface( const double* inTrialPoint, GeometryIndex& outIndex, double* outPoint ) const = 0;
   };
}

#endif // __SURFACE_H__
*/