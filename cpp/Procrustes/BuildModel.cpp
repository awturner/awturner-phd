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

#include "Mesh/VTKMeshLoader.h"
#include "Mesh/VTKMeshWriter.h"
#include "Mesh/MeshImpl.h"
#include "Mesh/AreaAveragedNormalCalculator.h"
#include "NonRigidAlignment/CorrespondenceFinder.h"
#include "Mesh/Pose.h"
#include "Useful/ArrayVectorFunctions.h"

#include "vnl/vnl_matrix.h"
#include "vnl/vnl_vector.h"
#include "vnl/algo/vnl_svd.h"
#include "vnl/algo/vnl_symmetric_eigensystem.h"

#include <iomanip>
#include <vector>
#include <algorithm>
#include <signal.h>

using namespace AWT;

typedef double T;
typedef vnl_matrix<T> Matrix;
typedef vnl_vector<T> Vector;

bool align(Matrix& x, const Matrix& y, const bool projectScale)
{
   Matrix ones(x.cols(), 1);
   ones.fill(1);

   // Calculate the centroids of the point clouds
   Matrix mu_x = x * ones / x.cols();
   Matrix mu_y = y * ones / y.cols();

   // Calculate the standard deviations of the point clouds
   T sigma_x = (x - mu_x*ones.transpose()).fro_norm();
   T sigma_y = (y - mu_y*ones.transpose()).fro_norm();

   Matrix covar = (x - mu_x*ones.transpose()) * (y - mu_y*ones.transpose()).transpose();
   vnl_svd<T> svd(covar);

   Matrix R = svd.V()*svd.U().transpose();

   T s;

   if (projectScale)
   {
      // Projects onto the hyperplane tangent to y
      T num = 0, den = 0;

      for (unsigned int r = 0; r < x.rows(); ++r)
      {
         for (unsigned int c = 0; c < x.cols(); ++c)
         {
            num += (y(r,c) - mu_y(r,0)) * (y(r,c) - mu_y(r,0));
            den += (x(r,c) - mu_x(r,0)) * (y(r,c) - mu_y(r,0));
         }
      }

      s = num / den;
   }
   else
   {
      // Projects onto the hypersphere through y
      s = sigma_y / sigma_x;
   }

   R *= s;
   R(3,3) = 1;

   const Matrix t = mu_y - R*mu_x;

   for (int ax = 0; ax < 3; ++ax)
      R(ax,3) = t(ax,0);

   // Transform the x cloud
   x = R*x;

   // Now test to see whether we are converging
   // (remember to unscale the rotation!)
   const T tr = (R(0,0)+R(1,1)+R(2,2)) / s;
   const T costheta = std::min<T>(1, (tr - 1) / 2);

   const T delta = t.fro_norm();

   bool thetaConverged = (1-costheta) < 1e-8;
   bool transConverged = delta < 1e-8;

   bool ret = !(thetaConverged && transConverged);
   
   return ret;
}

T innerProductIntegrate(const Matrix& ma, const Matrix& mb, const Matrix& mean, const Mesh<T>::P& mesh)
{
   // To store the mean and instance vertices
   T vtxA[3][3], vtxB[3][3], vtxMean[3][3];
   MeshIndex vs[3];

   T ret = 0;

   MESH_EACHFACE(mesh, f)
   {
      // Get the vertex indices from the mean mesh
      mesh->getFaceIndices(f, vs);

      // Now get the differences from the shape matrices
      for (MeshIndex i = 0; i < 3; ++i)
      {
         for (MeshIndex ax = 0; ax < 3; ++ax)
         {
            // Get the vertices from matrix mean
            vtxMean[i][ax] = mean(ax, vs[i]);
         
            // Get the vertices from matrix ma and subtract mean
            vtxA[i][ax] = ma(ax, vs[i]) - vtxMean[i][ax];

            // Get the vertices from matrix mb
            vtxB[i][ax] = mb(ax, vs[i]) - vtxMean[i][ax];
         }
      }

      // Shift the second two vertices to be relative to the first vertex
      for (MeshIndex i = 1; i < 3; ++i)
      {
         for (MeshIndex ax = 0; ax < 3; ++ax)
         {
            vtxA[i][ax] -= vtxA[0][ax];
            vtxB[i][ax] -= vtxB[0][ax];

            // Translate the mean vectors too; we need these for triangle area
            vtxMean[i][ax] -= vtxMean[0][ax];
         }
      }

      // Calculate the area of the triangle for scaling
      cross(vtxMean[1], vtxMean[2], vtxMean[0]);
      const T area = normalize(vtxMean[0], 3);

      // This notation comes from the double integration of:-
      //
      // [ (a + s*b + t*c) . (p + s*q + t*r) ]
      //
      // where a,b,c,p,q,r are 3-vectors
      // With limits s = [0..(1-t)] and t = [0..1]
      //
      // This is then scaled by the area of the triangle, since that is the
      // Jacobian determinant.  Because of the linear mapping, this is 
      // constant across the domain of integration

      const T a_p = dot(vtxA[0], vtxB[0], 3);
      const T a_q = dot(vtxA[0], vtxB[1], 3);
      const T a_r = dot(vtxA[0], vtxB[2], 3);
      const T b_p = dot(vtxA[1], vtxB[0], 3);
      const T b_q = dot(vtxA[1], vtxB[1], 3);
      const T b_r = dot(vtxA[1], vtxB[2], 3);
      const T c_p = dot(vtxA[2], vtxB[0], 3);
      const T c_q = dot(vtxA[2], vtxB[1], 3);
      const T c_r = dot(vtxA[2], vtxB[2], 3);

      ret += area * (12*a_p + 2*b_q + 4*c_p + c_q + 4*a_r + b_r + 4*b_p + 4*a_q + 2*c_r) / 24;
   }

   return ret;
}

T innerProduct(const Matrix& a, const Matrix& b, const Matrix& mean, const Mesh<T>::P& mesh)
{
   T ret = 0;

   for (unsigned int r = 0; r < a.rows(); ++r)
   {
      for (unsigned int c = 0; c < a.cols(); ++c)
      {
         ret += (a(r,c) - mean(r,c)) * (b(r,c) - mean(r,c));
      }
   }

   return ret;
}

void saveMesh(const Matrix& mode, const Mesh<T>::P topology, const std::string& filename)
{
   Mesh<T>::P modeMesh = MeshImpl<T>::getInstance(topology->getNumberOfVertices(), topology->getNumberOfFaces());
   T vtx[3];
   MeshIndex vs[3];

   for (unsigned int v = 0; v < mode.cols(); ++v)
   {
      vtx[0] = mode(0, v);
      vtx[1] = mode(1, v);
      vtx[2] = mode(2, v);

      modeMesh->setVertex(v, vtx);
   }

   MESH_EACHFACE(topology, f)
   {
      topology->getFaceIndices(f, vs);
      modeMesh->setFaceIndices(f, vs);
   }

   VTKMeshWriter<T>::write(modeMesh, filename.c_str());
}

void calculateMeanMatrix(const std::vector< Matrix >& shapeMatrices, Matrix& meanShape)
{
   // Recalculate the mean shape
   meanShape.fill(0);

   for (unsigned int i = 0; i < shapeMatrices.size(); ++i)
      meanShape += shapeMatrices[i];

   meanShape /= shapeMatrices.size();
}

int main(int argc, char** argv)
{
   if (argc < 3)
   {
      std::cerr << "Syntax: BuildModel <At least 2 meshes>" << std::endl;
      DEBUGLINEANDEXIT(1);
   }

   Mesh<T>::P refMesh;
   std::vector< Matrix > shapeMatrices;

   // Rather than doing this in terms of meshes, let's do it in terms of matrices
   for (int i = 1; i < argc; ++i)
   {
      DEBUGMACRO("Loading from " << argv[i]);
      Mesh<T>::P mesh = VTKMeshLoader<T>::load(argv[i]);
      
      if (i == 1)
      {
         // We need this as a reference coordinate system, plus for its topology
         refMesh = mesh;
      }
      else
      {
         if (refMesh->getNumberOfVertices() != mesh->getNumberOfVertices())
         {
            DEBUGMACRO("Wrong number of vertices");
            DEBUGLINEANDEXIT(1);
         }
         
         if (refMesh->getNumberOfFaces() != mesh->getNumberOfFaces())
         {
            DEBUGMACRO("Wrong number of faces");
            DEBUGLINEANDEXIT(1);
         }
      }

      Matrix shapeMatrix(4, mesh->getNumberOfVertices());
      T vtx[3];
      MESH_EACHVERTEX(mesh, v)
      {
         mesh->getVertex(v, vtx);

         for (int ax = 0; ax < 3; ++ax)
            shapeMatrix(ax,v) = vtx[ax];

         shapeMatrix(3,v) = 1;
      }

      shapeMatrices.push_back(shapeMatrix);
   }

   Matrix meanShape = shapeMatrices[0];
   Matrix refShape  = shapeMatrices[0];

   // Repeat until convergence
   bool keepGoing = true;
   for (unsigned int iter = 0; keepGoing; ++iter)
   {
      PRINTVBL(iter);
      keepGoing = false;

      // Align each shape to the current estimate of the mean
      for (unsigned int s = 0; s < shapeMatrices.size(); ++s)
         keepGoing |= align(shapeMatrices[s], meanShape, true);

      calculateMeanMatrix(shapeMatrices, meanShape);
      
      // Align meanShape to the reference shape
      keepGoing |= align(meanShape, refShape, false);
   }

   // Recalculate the mean shape (probably don't need to, just make sure)
   calculateMeanMatrix(shapeMatrices, meanShape);

   // Calculate the inner products of the shapes
   Matrix ata(shapeMatrices.size(), shapeMatrices.size());
   {
      for (unsigned int i = 0; i < shapeMatrices.size(); ++i)
         for (unsigned int j = i; j < shapeMatrices.size(); ++j)
            ata(j, i) = ata(i, j) = innerProductIntegrate(shapeMatrices[i], shapeMatrices[j], meanShape, refMesh);
   }

   std::cerr << std::setprecision(3);
   
   // ...and calculate the eigendecomposition
   vnl_symmetric_eigensystem<T> eig(ata);

   T eigSum = eig.D.diagonal().sum();

   unsigned int lastCol = shapeMatrices.size();
   T cumSum = 0;
   while (lastCol > 1 && cumSum < 0.97*eigSum)
   {
      --lastCol;

      cumSum += eig.D(lastCol);

      PRINTVBL(cumSum / eigSum);
   }

   BLANKLINE;
   PRINTVBL(cumSum / eigSum);
   PRINTVBL(lastCol);

   DEBUGLINEANDEXIT(1);

   std::vector< Matrix > shapeModes;
   unsigned int col = shapeMatrices.size();

   // Save the mean mesh as the "zero-th" mode
   shapeModes.push_back(meanShape);

   while (col-- > lastCol)
   {
      Matrix mode(shapeMatrices[0].rows(), shapeMatrices[0].cols());
      mode.fill(0);

      // Take the weighted sum of the shape matrices
      for (unsigned int i = 0; i < shapeMatrices.size(); ++i)
         mode += eig.V(i, col)*(shapeMatrices[i] - meanShape);

      shapeModes.push_back(mode);
   }

   //// Calculate the Mahalanobis distances between shapes
   //{
   //   // Don't use the 0-th mode
   //   Matrix modeMatrix(3*shapeMatrices[0].cols(), shapeModes.size()-1);

   //   for (unsigned int s = 1; s < shapeModes.size(); ++s)
   //   {
   //      for (unsigned int r = 0; r < shapeMatrices[0].cols(); ++r)
   //      {
   //         for (unsigned int c = 0; c < 3; ++c)
   //         {
   //            modeMatrix(3*r + c, s-1) = shapeModes[s](c, r) - meanShape(c, r);
   //         }
   //      }
   //   }

   //   vnl_svd<T> svdWeights(modeMatrix);
   //   std::vector< Vector > weights;
   //   for (unsigned int s = 0; s < shapeMatrices.size(); ++s)
   //   {
   //      Vector x(3*shapeMatrices[0].cols());
   //      for (unsigned int c = 0; c < shapeMatrices[0].cols(); ++c)
   //      {
   //         for (unsigned int ax = 0; ax < 3; ++ax)
   //         {
   //            x(3*c + ax) = shapeMatrices[s](ax, c) - meanShape(ax, c);
   //         }
   //      }

   //      Vector w = svdWeights.solve(x);
   //      PRINTVBL(w);
   //      weights.push_back(w);
   //   }

   //   for (unsigned int r = 0; r < 7; ++r)
   //   {
   //      for (unsigned int c = 7; c < 14; ++c)
   //      {
   //         T mahal = (weights[r] - weights[c]).magnitude();

   //         DEBUGMACRO(r << "\t" << c << "\t" << mahal);
   //      }
   //      BLANKLINE;
   //   }
   //   DEBUGLINEANDEXIT(1);
   //}

   for (unsigned int m = 0; m < shapeModes.size(); ++m)
   {
      std::stringstream sstr;
      sstr << "mode." << m << ".vtk";

      const std::string filename = sstr.str();

      saveMesh(shapeModes[m], refMesh, sstr.str());

      DEBUGMACRO("Written mode " << m << " to " << filename.c_str());
   }

   for (unsigned int m = 1; m < shapeModes.size(); ++m)
   {
      {
         std::stringstream sstr;
         sstr << "mode." << m << ".plus.vtk";

         Matrix plus = shapeModes[0] + shapeModes[m];
         saveMesh(plus, refMesh, sstr.str());
      }

      {
         std::stringstream sstr;
         sstr << "mode." << m << ".minus.vtk";

         Matrix minus = shapeModes[0] - shapeModes[m];
         saveMesh(minus, refMesh, sstr.str());
      }
   }
}