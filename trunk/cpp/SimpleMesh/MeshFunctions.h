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
#ifndef __SIMPLEMESHFUNCTIONS_H__
#define __SIMPLEMESHFUNCTIONS_H__

#include "Mesh.h"

namespace AWT
{
   namespace SimpleMesh
   {
      template <class T>
      void ensureSize(vnl_matrix<T>& mat, const unsigned int rows, const unsigned int cols)
      {
         if (mat.rows() != rows || mat.cols() != cols)
            mat.set_size(rows, cols);
      }

      bool checkHomogeneous(const Points& points, const double val = 1);

      void affineTransform(Points& points, const Transformation& trans);

      void getFaceVertices(Mesh::P mesh, const Index f, Point vertices[3]);

      Point calculateFaceNormal(Mesh::P mesh, const Index f, const bool normalize = true);

      void calculateFaceNormals(Mesh::P mesh, Points& normals, const bool normalize = true);
      void calculateVertexNormals(Mesh::P mesh, Points& normals, const bool normalize = true);

      double calculateSurfaceArea(Mesh::P mesh);
      double calculateFaceArea(Mesh::P mesh, const Index f);

      Point calculateCentroid(Mesh::P mesh);

      ConvexWeights calculateFaceWeights(Mesh::P mesh, const Index f, const Point x);
      Point calculatePoint(Mesh::P mesh, const Index f, const ConvexWeights w);

      Coordinate dot(const Point& a, const Point& b, const Index n);
   }
}

#endif // __SIMPLEMESHFUNCTIONS_H__