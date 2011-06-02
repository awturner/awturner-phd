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
#ifndef __TYPEDEFS_H__
#define __TYPEDEFS_H__

#include "vnl/vnl_vector.h"
#include "vnl/vnl_matrix.h"
#include "vnl/vnl_diag_matrix.h"

#include "vnl/vnl_vector_fixed.h"
#include "vnl/vnl_matrix_fixed.h"

#include <vector>

namespace AWT
{
   namespace SimpleMesh
   {
      typedef unsigned int Index;
      typedef double Coordinate;
      typedef double Weight;

      typedef vnl_vector<Coordinate> Vector;
      typedef vnl_matrix<Coordinate> Matrix;
      typedef vnl_matrix<Index> IMatrix;

      typedef vnl_diag_matrix<Coordinate> DiagMatrix;

      typedef vnl_vector_fixed<Coordinate,4> Point;
      typedef vnl_vector_fixed<Coordinate,2> TexCoord;
      typedef vnl_vector_fixed<Index,3>  Face;

      typedef Matrix Points;
      typedef Matrix TexCoords;
      typedef IMatrix Faces;

      extern const Index  INVALID_INDEX;
      extern const Coordinate FAR_FAR_AWAY;
      extern const Coordinate INVALID_COORDINATE;

      typedef vnl_vector_fixed<Weight,2> ConvexWeights;

      typedef vnl_matrix_fixed<Coordinate,4,4> Transformation;
      
      void calculateTransformation(const Vector& v, Transformation& mat);

      struct PointIndexWeights
      {
         PointIndexWeights();

         Point p;
         Index i;
         ConvexWeights w;
      };
      typedef std::vector<PointIndexWeights> PIWs;

      struct Correspondence
      {
         PointIndexWeights modelPoint;
         Point modelNormal;

         PointIndexWeights fragPoint;
         Index whichFragment;

         Coordinate lengthSquared;

         bool valid() const
         {
            const Coordinate m = modelPoint.p.squared_magnitude();
            const Coordinate f = fragPoint.p.squared_magnitude();

            return whichFragment != INVALID_INDEX && m == m && f == f;
         }

         bool operator<(const Correspondence& other) const
         {
            return lengthSquared < other.lengthSquared;
         }
      };
      typedef std::vector<Correspondence> Correspondences;

      struct LameckerMeasures
      {
         LameckerMeasures();

         double mean;
         double rms;
         double max;
         double aod;

         double modelScale;
      };

      Vector matrixToPose(const Transformation& mat);
      Transformation poseToMatrix(const Vector& pose);
   }
}

std::ostream& operator<<(std::ostream& os, const AWT::SimpleMesh::Correspondence& self);

#endif // __TYPEDEFS_H__