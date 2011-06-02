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
#include "vnl/algo/vnl_sparse_lu.h"
#include "vnl/algo/vnl_symmetric_eigensystem.h"

#include <exception>
#include <limits>

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

   MT dot(const MT* a, const MT* b, const IndexType K);

   struct Coordinate
   {
      IndexType x, y;
   };

   const Coordinate index_to_xy(const IndexType index, const IndexType mx, const IndexType my);

   enum MoveDirection
   {
      N, NE, E, SE, S, SW, W, NW
   };

   const char* dir_name(const MoveDirection dir);

   const IndexType index_move(const IndexType index, const IndexType mx, const IndexType my, const MoveDirection dir);

   const IndexType xy_to_index(const IndexType x, const IndexType y, const IndexType mx, const IndexType my);

   void neighbours(const IndexType index, const IndexType mx, const IndexType my, DaviesBook::IndexType neighbours[3][3]);

   void neighbours(const IndexType index, const IndexType mx, const IndexType my, DaviesBook::IndexType neighbours[9]);

   const MT wrap(const MT x, const MT lower, const MT upper);

   const MT kronecker(const IndexType a, const IndexType b);

   void bilinear_interpolation(const ImageType& shape_image, const MT h, const MT a[2], MT b[3]);

   void populate_shape_vector(const ImageType& shape_image, const MT h, const Matrix& displacement, Matrix& shape_vector);

   void calculate_mean_shape_vector(const Matrix* shape_vector, const IndexType ns, Matrix& mean_shape_vector);

   const MT calculate_int_triangle_areas(const Matrix& shape_vector, const IndexType mx, const IndexType my, Vector& areas);

   void accumulate_reparameterization(Matrix& cumulative_reparameterization, const Matrix& proposed_reparameterization, const IndexType mx, const IndexType my, const MT h);

   void calculate_eigensystem(const Matrix& mat, Vector& eigenvalues, Matrix& eigenvectors);
   
   void allocate_vectors(Vector*& vec, const IndexType n, const DaviesBook::IndexType len);

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
   };

   MT calculate_L(const Vector& eigenvalues, Vector& dL_by_de, const FluidOptimizationParams& params);

   void save_reparameterization(Matrix& cumulative_reparameterization, const Matrix& grid_points, Matrix& displacement, 
      ImageType& shape_image, const ImageType& Original_Shape_Image, const FluidOptimizationParams& params);

   void write_shape_vectors(const Matrix* shape_vector, const IndexType ns, const IndexType n, const bool pause = true);

   void calculate_covariance(const Matrix* shape_vector, const DaviesBook::FluidOptimizationParams params, Matrix& Covariance);

   void optimise_fluid(ImageType& shape_image_stack, const CImg<bool>& free_node_image, const FluidOptimizationParams& params);
}

#endif // __CHAPTER7_H__