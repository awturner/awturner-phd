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
#ifndef __CHAPTER7_H__
#define __CHAPTER7_H__

#define NOMINMAX

#include "CImg.h"
#include "vnl/vnl_vector.h"
#include "vnl/vnl_matrix.h"
#include "vnl/vnl_sparse_matrix.h"
#include "vnl/algo/vnl_lsqr.h"
#include "vnl/algo/vnl_sparse_lu.h"
#include "vnl/algo/vnl_symmetric_eigensystem.h"
#include "vnl/vnl_linear_system.h"

#include <exception>
#include <limits>

#include "Useful/PrintMacros.h"

using namespace cimg_library;

namespace DaviesBook
{
   typedef unsigned int IndexType;
   typedef unsigned int CounterType;

   typedef double MT;

   typedef CImg<MT>                      ImageType;
   typedef vnl_vector<MT>                Vector;
   typedef vnl_matrix<MT>                Matrix;
   typedef vnl_symmetric_eigensystem<MT> EigenSystem;
   typedef vnl_sparse_matrix<MT>         SparseMatrix;
   typedef vnl_sparse_lu                 SparseLU;

   const IndexType castToIndex(const MT x);

   // Calculate inner product between vectors a and b, both of length K
   MT dot(const MT* a, const MT* b, const IndexType K);

   // Calculate the cross product of 3-vectors a and b, storing the result in c
   void cross(const MT a[3], const MT b[3], MT c[3]);

   // Struct to hold an (x,y) coordinate pair
   struct Coordinate
   {
      IndexType x, y;
   };

   // Directions to move from the current node
   enum MoveDirection
   {
      N, NE, E, SE, S, SW, W, NW
   };

   struct FluidOptimizationParams;

   // Convenience method to get a name instead of a number
   const char* dir_name(const MoveDirection dir);

   // Convert from an index into an (x,y) coordinate
   const Coordinate index_to_xy(const IndexType index, const IndexType mx, const IndexType my);

   // Convert from an (x,y) coordinate to an index
   const IndexType xy_to_index(const IndexType x, const IndexType y, const IndexType mx, const IndexType my);

   // Move from the given index in a direction
   const IndexType index_move(const IndexType index, const IndexType mx, const IndexType my, const MoveDirection dir);

   // Get the indices of the neighbouring vertices as a 1d array
   void neighbours(const IndexType index, const IndexType mx, const IndexType my, DaviesBook::IndexType neighbours[9]);

   // Wrap x into the range [lower..upper] by adding/subtracting some integer multiple of (upper-lower)
   const MT wrap(const MT x, const MT lower, const MT upper);

   const MT floorquantize(const MT x, const MT q);

   // Kronecker delta, i.e. (a == b) ? 1 : 0
   const MT kronecker(const IndexType a, const IndexType b);

   // Do wrapped bilinear interpolation on the image
   template <unsigned char K>
   void bilinear_interpolation(const ImageType& shape_image, const MT h, const MT a[2], MT b[K]);

   // Create the shape vector from the given shape image, given displacements on the grid positions
   void populate_shape_vector(const ImageType& shape_image, const MT h, const Matrix& displacement, Matrix& shape_vector);

   // Calculate the average shape vector
   void calculate_mean_shape_vector(const Matrix* shape_vector, const IndexType ns, Matrix& mean_shape_vector);

   // Calculate the area of triangles surrounding each vertex
   const MT calculate_int_triangle_areas(const Matrix& shape_vector, const IndexType mx, const IndexType my, Vector& areas);

   // Calculate the sorted eigendecomposition of the given matrix
   void calculate_eigensystem(const Matrix& mat, Vector& eigenvalues, Matrix& eigenvectors);
   
   struct FluidOptimizationParams
   {
      FluidOptimizationParams()
      {
         h = 1;
         visc_mu = visc_lambda = 1;

         niterations = 1500;

         eps = 1e-8;
         Delta = 1e-5;
         timestep_mult = 1e-3;

         regularize = true;
      }

      MT h;              // Grid spacing
      MT visc_mu;        // Viscosity Parameter
      MT visc_lambda;    // Viscosity Parameter
      MT eps;            // Avoid singularity in the eigenvalues
      MT Delta;          // How much to move by in the numerical estimation of gradient
      MT timestep_mult;  // Time step size
      MT lambda_cut;     // For Thodberg's expression, the cut-off lambda

      IndexType niterations; // How many iterations of optimization

      IndexType ref; // Which is the reference shape image

      IndexType mx; // x dimension of shape image
      IndexType my; // y dimension of shape image

      IndexType ns; // Number of shapes

      IndexType iterations_between_D_rebuild; // The number of iterations which pass without rebuilding D

      bool regularize;
   };

   void calculate_D(SparseMatrix& D, const Matrix* shape_vector, const bool* free_node, const FluidOptimizationParams& params);

   void calculate_D(SparseMatrix& D, const Matrix& mean_shape_vector, const bool* free_node, const FluidOptimizationParams& params);

   // Calculate the cost function given a set of shape vectors
   MT calculate_L(const Matrix* shape_vector, const DaviesBook::FluidOptimizationParams& params, const bool report = false);

   // Calculate the cost function and gradient wrt each eigenvalue
   MT calculate_L(const Vector& eigenvalues, Vector& dL_by_de, const FluidOptimizationParams& params);

   // Calculate the gradient in a slow way (i.e. rebuild model and calculate L)
   void calculate_gradient_slow(const IndexType i, const ImageType& shape_image_stack, const Matrix* shape_vector, const Matrix& displacement, const FluidOptimizationParams& params, Vector& gradient);

   // Calculate the gradient in a fast way (i.e. exploiting chain rule factorization)
   void calculate_gradient(const ImageType& shape_image_stack, const Matrix* shape_vector, const Matrix* displacement, const FluidOptimizationParams& params, Vector* gradient, const IndexType icalc = std::numeric_limits<IndexType>::max());

   // Incrementally update the cumulative reparametrization
   void accumulate_reparametrization(Matrix& cumulative_reparametrization, const Matrix& proposed_reparametrization, const DaviesBook::FluidOptimizationParams&);

   void save_reparametrization(Matrix& cumulative_reparametrization, const Matrix& grid_points, Matrix& displacement, 
      ImageType& shape_image, const ImageType& Original_Shape_Image, const FluidOptimizationParams& params);

   void write_shape_vectors(const Matrix* shape_vector, const IndexType ns, const IndexType n, const bool pause = true);

   void calculate_covariance(const Matrix* shape_vector, const DaviesBook::FluidOptimizationParams params, Matrix& Covariance);

   void optimise_fluid(ImageType& shape_image_stack, const CImg<bool>& free_node_image, const FluidOptimizationParams& params);

   template <class T>
   void swap(T& a, T& b)
   {
      T c = a;
      a = b;
      b = c;
   }

   // Randomize the order of an array, with mod-bias
   template <class T>
   void randomlyPermuteArray(T* arr, const unsigned int n)
   {
      for (unsigned int i = 0; i < n; ++i) swap(arr[i + (rand()%(n-i)) ], arr[i]);
   }
}

#endif // __CHAPTER7_H__