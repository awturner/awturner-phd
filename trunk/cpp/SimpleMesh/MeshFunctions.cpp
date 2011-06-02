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
#include "MeshFunctions.h"

#include "vnl/algo/vnl_svd.h"

#include "Useful/ArrayVectorFunctions.h"

using namespace AWT;

bool AWT::SimpleMesh::checkHomogeneous( const SimpleMesh::Points& points, const double val )
{
   const Index r = points.rows() - 1;

   for ( Index i = 0; i < points.cols(); ++i )
      if ( points( i, r ) != val )
         return false;

   return true;
}

void AWT::SimpleMesh::affineTransform( SimpleMesh::Points& points, const Transformation& trans )
{
   if ( trans( 3, 0 ) != 0 || trans( 3, 1 ) != 0 || trans( 3, 2 ) != 0 || trans( 3, 3 ) != 1 )
      throw "Non-affine transformation!";

   points = trans.as_matrix() * points;
}

void AWT::SimpleMesh::getFaceVertices( Mesh::P mesh, const Index f, Point vertices[3] )
{
   Face face = mesh->getFace( f );

   for ( Index i = 0; i < 3; ++i )
      vertices[i] = mesh->getVertex( face[i] );
}

AWT::SimpleMesh::Point AWT::SimpleMesh::calculateFaceNormal( SimpleMesh::Mesh::P mesh, const SimpleMesh::Index f, const bool normalize )
{
   Point vertices[3];

   // Get the vertices
   getFaceVertices( mesh, f, vertices );

   // Shift relative to the zero-th vertex
   vertices[1] -= vertices[0];
   vertices[2] -= vertices[0];

   // Take the cross-product of the two shifted vertices
   cross<double>( vertices[1].data_block(), vertices[2].data_block(), vertices[0].data_block() );

   // Make sure that there's a zero at the end
   vertices[0][3] = 0;

   if ( normalize )
      vertices[0].normalize();

   return vertices[0];
}

void AWT::SimpleMesh::calculateFaceNormals( Mesh::P mesh, SimpleMesh::Points& normals, const bool normalize )
{
   ensureSize( normals, 4, mesh->nf );

   Point vertices[3];

   for ( Index f = 0; f < mesh->nf; ++f )
   {
      // Get the vertices
      getFaceVertices( mesh, f, vertices );

      // Shift relative to the zero-th vertex
      vertices[1] -= vertices[0];
      vertices[2] -= vertices[0];

      // Take the cross-product of the two shifted vertices
      cross<double>( vertices[1].data_block(), vertices[2].data_block(), vertices[0].data_block() );

      // Make sure that there's a zero at the end
      vertices[0][3] = 0;

      if ( normalize )
         vertices[0].normalize();

      normals.set_column( f, vertices[0] );
   }
}

void AWT::SimpleMesh::calculateVertexNormals( Mesh::P mesh, SimpleMesh::Points& vertexNormals, const bool normalize )
{
   SimpleMesh::Points faceNormals;

   vertexNormals.set_size(4,mesh->nv);

   calculateFaceNormals( mesh, faceNormals, false );

   // Initialize the vertex vertexNormals matrix
   vertexNormals.fill( 0 );

   for ( Index f = 0; f < mesh->nf; ++f )
   {
      // Get the normal for this face and calculate its magnitude (cf the face area)
      const Point nml = faceNormals.get_column( f );

      // Get the vertices
      Face face = mesh->getFace( f );

      for ( Index i = 0; i < 3; ++i )
      {
         Index v = face[i];

         vertexNormals.set_column( v, vertexNormals.get_column( v ) + nml );
      }
   }

   if ( normalize )
   {
      for ( Index v = 0; v < mesh->nv; ++v )
         vertexNormals.set_column( v, vertexNormals.get_column( v ).normalize( ) );
   }
}

double AWT::SimpleMesh::calculateSurfaceArea( Mesh::P mesh )
{
   double ret = 0;

   for ( Index f = 0; f < mesh->nf; ++f )
      ret += calculateFaceArea( mesh, f );

   return ret;
}

double AWT::SimpleMesh::calculateFaceArea( Mesh::P mesh, const Index f )
{
   // Get the vertices
   Point vertices[3];
   getFaceVertices( mesh, f, vertices );

   // Shift relative to the zero-th vertex
   vertices[1] -= vertices[0];
   vertices[2] -= vertices[0];

   // Take the cross-product of the two shifted vertices
   cross<double>( vertices[1].data_block(), vertices[2].data_block(), vertices[0].data_block() );

   // Make sure that there's a zero at the end
   vertices[0][3] = 0;

   return vertices[0].magnitude( ) / 2;
}

AWT::SimpleMesh::Point AWT::SimpleMesh::calculateCentroid( AWT::SimpleMesh::Mesh::P mesh )
{
   Point ret;
   double den = 0;

   ret.fill( 0 );

   Point vertices[3];

   for ( Index f = 0; f < mesh->nf; ++f )
   {
      const double area = calculateFaceArea( mesh, f );

      getFaceVertices( mesh, f, vertices );

      ret += area * ( vertices[0] + vertices[1] + vertices[2] ) / 3.0;
      den += area;
   }

   return ret / den;
}

AWT::SimpleMesh::ConvexWeights AWT::SimpleMesh::calculateFaceWeights( AWT::SimpleMesh::Mesh::P mesh, const AWT::SimpleMesh::Index f, const AWT::SimpleMesh::Point x )
{
   // Get the face vertices
   Point vertices[3];
   getFaceVertices( mesh, f, vertices );

   Matrix mat(4,2);

   mat.set_column( 0, vertices[1] - vertices[0] );
   mat.set_column( 1, vertices[2] - vertices[0] );

   ConvexWeights ret;
   vnl_svd<double>( mat ).solve( ( x - vertices[0] ).data_block(), ret.data_block() );
   
   return ret;
}

AWT::SimpleMesh::Point AWT::SimpleMesh::calculatePoint( Mesh::P mesh, const Index f, const ConvexWeights w )
{
   Point vertices[3];
   getFaceVertices( mesh, f, vertices );

   return ( 1 - w(0) - w(1) ) * vertices[0] + w(0) * vertices[1] + w(1) * vertices[2];
}

AWT::SimpleMesh::Coordinate AWT::SimpleMesh::dot(const AWT::SimpleMesh::Point& a, const AWT::SimpleMesh::Point& b, const AWT::SimpleMesh::Index n)
{
   return AWT::dot<Coordinate>( a.data_block(), b.data_block(), n );
}