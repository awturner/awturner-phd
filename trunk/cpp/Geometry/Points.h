#ifndef __POINTS_H__
#define __POINTS_H__

namespace AWT
{
   typedef unsigned long GeometryIndex;
   typedef unsigned char DataChannelIndex;
   typedef unsigned int  DataPointer;

   template <class CoordinateType>
   class Points
   {
   public:
      virtual GeometryIndex    getNumberOfVertices( ) const = 0;

      virtual void             getVertex( const GeometryIndex vertexIndex, CoordinateType* outVertexLocation ) const = 0;

      virtual bool             findClosestVertex( const CoordinateType* inTrialPoint, GeometryIndex& outIndex ) const = 0;
   };
}

#endif // __POINTS_H__
