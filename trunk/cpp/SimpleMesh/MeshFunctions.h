#ifndef __SIMPLEMESHFUNCTIONS_H__
#define __SIMPLEMESHFUNCTIONS_H__

#include "Mesh.h"

namespace AWT
{
   namespace SimpleMesh
   {
      template <class T>
      void ensureSize( vnl_matrix<T>& mat, const unsigned int rows, const unsigned int cols )
      {
         if ( mat.rows() != rows || mat.cols() != cols )
            mat.set_size( rows, cols );
      }

      bool checkHomogeneous( const Points& points, const double val = 1 );

      void affineTransform( Points& points, const Transformation& trans );

      void getFaceVertices( Mesh::P mesh, const Index f, Point vertices[3] );

      Point calculateFaceNormal( Mesh::P mesh, const Index f, const bool normalize = true );

      void calculateFaceNormals( Mesh::P mesh, Points& normals, const bool normalize = true );
      void calculateVertexNormals( Mesh::P mesh, Points& normals, const bool normalize = true );

      double calculateSurfaceArea( Mesh::P mesh );
      double calculateFaceArea( Mesh::P mesh, const Index f );

      Point calculateCentroid( Mesh::P mesh );

      ConvexWeights calculateFaceWeights( Mesh::P mesh, const Index f, const Point x );
      Point calculatePoint( Mesh::P mesh, const Index f, const ConvexWeights w );

      Coordinate dot(const Point& a, const Point& b, const Index n);
   }
}

#endif // __SIMPLEMESHFUNCTIONS_H__