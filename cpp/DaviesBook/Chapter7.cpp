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
#include "Chapter7.h"

#include "Useful/PrintMacros.h"
#include <iostream>
#include <fstream>
#include <signal.h>

#include "Useful/OneTimeMessage.h"

#include "vnl/vnl_sparse_matrix_linear_system.h"

bool keepIterating = true;

void handleSignal(int sig)
{
   keepIterating = false;
}

// This is a debugging function to convert a direction into a "readable" string
const char* DaviesBook::dir_name(const DaviesBook::MoveDirection dir)
{
   switch (dir)
   {
   case N:  return "N";
   case NE: return "NE";
   case NW: return "NW";
   case W:  return "W";
   case E:  return "E";
   case S:  return "S";
   case SE: return "SE";
   case SW: return "SW";
   default:
      return "Unknown direction";
   }
}

DaviesBook::MT DaviesBook::dot(const MT* a, const MT* b, const IndexType K)
{
   MT ret = 0;

   for (IndexType i = 0; i < K; ++i)
      ret += a[i]*b[i];

   return ret;
}

void DaviesBook::cross(const DaviesBook::MT a[3], const DaviesBook::MT b[3], DaviesBook::MT c[3])
{
   c[0] = a[1]*b[2]-a[2]*b[1];
   c[1] = a[2]*b[0]-a[0]*b[2];
   c[2] = a[0]*b[1]-a[1]*b[0];
}

const DaviesBook::MT DaviesBook::wrap(const DaviesBook::MT x, const DaviesBook::MT lower, const DaviesBook::MT upper)
{
   return x - floor((x - lower) / (upper - lower)) * (upper - lower);
}

const DaviesBook::MT DaviesBook::floorquantize(const DaviesBook::MT x, const DaviesBook::MT q)
{
   return q * floor(x / q);
}

const DaviesBook::Coordinate DaviesBook::index_to_xy(const DaviesBook::IndexType index, const DaviesBook::IndexType mx, const DaviesBook::IndexType my)
{
   Coordinate ret;
   ret.x = index / my;
   ret.y = index % my;
   return ret;
}

const DaviesBook::IndexType DaviesBook::xy_to_index(const DaviesBook::IndexType x, const DaviesBook::IndexType y, const DaviesBook::IndexType /*mx*/, const DaviesBook::IndexType my)
{
   return x * my + y;
}

const DaviesBook::IndexType DaviesBook::index_move(const DaviesBook::IndexType k, const DaviesBook::IndexType mx, const DaviesBook::IndexType my, const DaviesBook::MoveDirection dir)
{
   // Calculate the current grid x and y
   const Coordinate coord = index_to_xy(k, mx, my);

   IndexType xnew = coord.x, ynew = coord.y;

   switch (dir)
   {
   case W:
   case NW:
   case SW:
      xnew = ((mx + coord.x) - 1) % mx;
      break;
   case E:
   case NE:
   case SE:
      xnew = (coord.x + 1     ) % mx;
      break;
   }

   switch (dir)
   {
   case N:
   case NE:
   case NW:
      ynew = ((my + coord.y) - 1) % my;
      break;
   case S:
   case SE:
   case SW:
      ynew = (coord.y + 1     ) % my;
      break;
   }

   if (xnew >= mx)
   {
      throw std::exception("xnew >= mx");
   }

   if (ynew >= my)
   {
      throw std::exception("ynew >= my");
   }
   
   return xy_to_index(xnew, ynew, mx, my);
}

#define NB_NW 0
#define NB_N  1
#define NB_NE 2

#define NB_W  3
#define NB_C  4
#define NB_E  5

#define NB_SW 6
#define NB_S  7
#define NB_SE 8

void DaviesBook::neighbours(const DaviesBook::IndexType index, const DaviesBook::IndexType mx, 
                                  const DaviesBook::IndexType my, DaviesBook::IndexType neighbours[9])
{
   neighbours[NB_NW] = index_move(index, mx, my, NW);
   neighbours[NB_N ] = index_move(index, mx, my, N );
   neighbours[NB_NE] = index_move(index, mx, my, NE);

   neighbours[NB_W ] = index_move(index, mx, my, W );
   neighbours[NB_C ] = index;
   neighbours[NB_E ] = index_move(index, mx, my, E );

   neighbours[NB_SW] = index_move(index, mx, my, SW);
   neighbours[NB_S ] = index_move(index, mx, my, S );
   neighbours[NB_SE] = index_move(index, mx, my, SE);
}

template <unsigned char K>
void DaviesBook::bilinear_interpolation(const ImageType& shape_image, const MT h, const MT a[2], MT b[K])
{
   // Scale into the image's coordinate frame and wrap around - this is explicitly assuming toroidal topology
   MT ah[] = { wrap(a[0]/h, 0, shape_image.dimx()), wrap(a[1]/h, 0, shape_image.dimy()) };

   IndexType x0 = static_cast<IndexType>(ah[0]);
   IndexType y0 = static_cast<IndexType>(ah[1]);

   // Wrap around if necessary
   IndexType x1 = (x0 + 1) % shape_image.dimx();
   IndexType y1 = (y0 + 1) % shape_image.dimy();

   // Calculate the fractional parts
   MT dx = ah[0] - x0;
   MT dy = ah[1] - y0;

   MT v00, v01, v10, v11;

   // Do the bilinear interpolation
   for (IndexType ax = 0; ax < K; ++ax)
   {
      v00 = shape_image(x0, y0, ax);
      v01 = shape_image(x1, y0, ax);

      v10 = shape_image(x0, y1, ax);
      v11 = shape_image(x1, y1, ax);

      b[ax] = (1 - dy) * ((1 - dx) * v00 + dx * v01) + dy * ((1 - dx) * v10 + dx * v11);
   }
}

void DaviesBook::populate_shape_vector(const ImageType& shape_image, const MT h, const Matrix& displacement, Matrix& shape_vector)
{
   const IndexType n = shape_image.dimx() * shape_image.dimy();

   // Do a little bit of size checking
   if (displacement.rows() != n)
      throw std::exception("Displacement matrix is the wrong size");

   shape_vector.set_size(n, 3);

   const IndexType mx = shape_image.dimx();
   const IndexType my = shape_image.dimy();

   // Now get down to work
   MT sample_point[2];
   MT vtx[3];
   for (IndexType j = 0; j < n; ++j)
   {
      // Assume that the points go in column major order, i.e. down columns first
      const Coordinate grid_pos = index_to_xy(j, mx, my);

      sample_point[0] = grid_pos.x * h - displacement(j, 0);
      sample_point[1] = grid_pos.y * h - displacement(j, 1);

      bilinear_interpolation<3>(shape_image, h, sample_point, vtx);

      shape_vector(j, 0) = vtx[0];
      shape_vector(j, 1) = vtx[1];
      shape_vector(j, 2) = vtx[2];
   }
}

void DaviesBook::calculate_mean_shape_vector(const DaviesBook::Matrix* shape_vector, const IndexType ns, DaviesBook::Matrix& mean_shape_vector)
{
   mean_shape_vector.set_size(shape_vector[0].rows(), shape_vector[0].cols());
   mean_shape_vector.fill(0);

   for (IndexType i = 0; i < ns; ++i)
      mean_shape_vector += shape_vector[i];

   mean_shape_vector /= ns;
}

const DaviesBook::MT DaviesBook::calculate_int_triangle_areas(const DaviesBook::Matrix& shape_vector, const DaviesBook::IndexType mx, const DaviesBook::IndexType my, DaviesBook::Vector& areas)
{
   // Number of points
   const IndexType n = mx * my;

   // Assume an implicit triangulation of the plane
   IndexType triangles[] = {
      NB_N , NB_NE, NB_C ,  // COUNT THIS IN TOTAL SURFACE AREA
      NB_NE, NB_E , NB_C ,  // COUNT THIS IN TOTAL SURFACE AREA
      NB_C , NB_E , NB_S ,
      NB_C , NB_S , NB_SW,
      NB_W , NB_C , NB_SW,
      NB_N , NB_C , NB_W 
   };

   MT p[3][3];

   MT ret = 0;

   for (IndexType i = 0; i < n; ++i)
   {
      // Reset the area
      areas(i) = 0;

      // Get the adjacent indices
      IndexType movedIndexes[9];
      neighbours(i, mx, my, movedIndexes);
      
      // For each of the 6 surrounding triangles
      for (IndexType t = 0; t < 6; ++t)
      {
         // Get the coordinates of the vertices
         for (IndexType v = 0; v < 3; ++v)
            for (IndexType ax = 0; ax < 3; ++ax)
            {
               p[v][ax] = shape_vector(movedIndexes[ triangles[3*t+v] ], ax);
            }

         // Relative to the 0-th vertex
         for (IndexType ax = 0; ax < 3; ++ax)
         {
            p[1][ax] -= p[0][ax];
            p[2][ax] -= p[0][ax];
         }

         // Calculate the cross product
         cross(p[1], p[2], p[0]);

         // Calculate half the magnitude of this
         MT this_area = sqrt(dot(p[0], p[0], 3)) / 2;

         // Add to the totals
         areas(i) += this_area;

         if (t < 2)
            ret += this_area;
      }
   }

   return ret;
}

const DaviesBook::MT DaviesBook::kronecker(const DaviesBook::IndexType a, const DaviesBook::IndexType b)
{
   return (a == b) ? 1 : 0;
}

void DaviesBook::calculate_D(DaviesBook::SparseMatrix& D, const DaviesBook::Matrix* shape_vector, const bool* free_node, const DaviesBook::FluidOptimizationParams& params)
{
   Matrix mean_shape_vector;

   calculate_mean_shape_vector(shape_vector, params.ns, mean_shape_vector);

   calculate_D(D, mean_shape_vector, free_node, params);
}

void DaviesBook::calculate_D(DaviesBook::SparseMatrix& D, const DaviesBook::Matrix& mean_shape_vector, const bool* free_node, const DaviesBook::FluidOptimizationParams& params)
{
   using namespace DaviesBook;

   const IndexType n = mean_shape_vector.rows();

   // 8.1 Initialize by setting all elements of D to zero
   //     - vnl's sparse matrix is by default all zero, so do nothing

   // 8.2 Let...
   const MT hh = params.h * params.h;

   const MT a = (2*params.visc_mu + params.visc_lambda) /    hh;
   const MT b =     params.visc_mu                        /    hh;
   const MT c = (  params.visc_mu + params.visc_lambda) / (4*hh);
   const MT d = (  params.visc_mu + params.visc_lambda) /    hh;
   const MT e = (3*params.visc_mu + params.visc_lambda) / (4*hh);

   IndexType k_nb[9];

   // 8.4 Populate the matrix using a finite difference scheme
   MT grad_i[3], grad_j[3], metric[2][2], inverse_metric[2][2];
   for (IndexType k = 0; k < n; ++k)
   {
      if (free_node[k])
      {
         // Calculate the neighbours
         neighbours(k, params.mx, params.my, k_nb);

         // Calculate the gradient of the shape function in the i and j directions and
         // store them in three-dimensional vectors
         for (IndexType ax = 0; ax < 3; ++ax)
         {
            grad_i[ax] = (mean_shape_vector(k_nb[NB_E ], ax) - mean_shape_vector(k_nb[NB_W ], ax)) / (2 * params.h);
            grad_j[ax] = (mean_shape_vector(k_nb[NB_S ], ax) - mean_shape_vector(k_nb[NB_N ], ax)) / (2 * params.h);
         }

         // Calculate the 2x2 matrix Metric - corresponds to g in (8.32)
         // - I'm almost calculating this, just doing a slight optimization in not dividing by h^3
         //MT h3 = params.h * params.h * params.h;
         metric[0][0]                = dot(grad_i, grad_i, 3);// / h3;
         metric[0][1] = metric[1][0] = dot(grad_i, grad_j, 3);// / h3;
         metric[1][1]                = dot(grad_j, grad_j, 3);// / h3;

         // Calculate the inverse of the metric
         MT idet = 1 / (metric[0][0]*metric[1][1] - metric[0][1]*metric[1][0]);

         if (abs(idet) == std::numeric_limits<MT>::infinity() || idet != idet)
         {
            std::ofstream fs("meanshapevector.m");
            fs << "msv = [" << std::endl;
            for (IndexType k = 0; k < n; ++k)
               fs << mean_shape_vector(k, 0) << " " << mean_shape_vector (k, 1) << " " << mean_shape_vector(k, 2) << std::endl;
            fs << "]'; scatter3x(msv, 'b.');" << std::endl;
            fs.close();

            throw std::exception("singularity in mapping!");
         }

         // Calculate the inverse
         inverse_metric[0][0] =  metric[1][1] * idet;
         inverse_metric[0][1] = -metric[0][1] * idet;
         inverse_metric[1][0] = -metric[1][0] * idet;
         inverse_metric[1][1] =  metric[0][0] * idet;

         // Fill in the finite difference operations for the i component
         {
            const MT aa = a * inverse_metric[0][0];
            const MT bb = b * inverse_metric[1][1];
            const MT cc = c * inverse_metric[0][0];
            const MT dd = d * inverse_metric[0][1];
            const MT ee = e * inverse_metric[0][1];

            D(    k,     k            ) = - 2*aa - 2*bb; // v_i(i,j)
            D(    k,     k_nb[NB_W ]) =  aa;           // v_i(i-1,j)
            D(    k,     k_nb[NB_E ]) =  aa;           // v_i(i+1,j)
            D(    k,     k_nb[NB_N ]) =  bb;           // v_i(i,j-1)
            D(    k,     k_nb[NB_S ]) =  bb;           // v_i(i,j+1)
            D(    k,     k_nb[NB_SE]) =  ee;           // v_i(i+1,j+1)
            D(    k,     k_nb[NB_SW]) = -ee;           // v_i(i-1,j+1)
            D(    k,     k_nb[NB_NE]) = -ee;           // v_i(i-1,j-1)
            D(    k,     k_nb[NB_NW]) =  ee;           // v_i(i+1,j-1)
            D(    k, n + k            ) = -2*dd;         // v_j(i,j)
            D(    k, n + k_nb[NB_S ]) =  dd;           // v_j(i,j+1)
            D(    k, n + k_nb[NB_N ]) =  dd;           // v_j(i,j-1)
            D(    k, n + k_nb[NB_SE]) =  cc;           // v_j(i+1,j+1)
            D(    k, n + k_nb[NB_SW]) = -cc;           // v_j(i-1,j+1)
            D(    k, n + k_nb[NB_NE]) = -cc;           // v_j(i+1,j-1)
            D(    k, n + k_nb[NB_NW]) =  cc;           // v_j(i-1,j-1)
         }

         // Now do the same for the j component
         {
            const MT aa = a * inverse_metric[1][1];
            const MT bb = b * inverse_metric[0][0];
            const MT cc = c * inverse_metric[1][1];
            const MT dd = d * inverse_metric[1][0];
            const MT ee = e * inverse_metric[1][0];

            D(n + k, n + k            ) = - 2*aa - 2*bb; // v_j(i,j)
            D(n + k, n + k_nb[NB_N ]) = aa;            // v_j(i-1,j)
            D(n + k, n + k_nb[NB_S ]) = aa;            // v_j(i+1,j)
            D(n + k, n + k_nb[NB_W ]) = bb;            // v_j(i,j-1)
            D(n + k, n + k_nb[NB_E ]) = bb;            // v_j(i,j+1)
            D(n + k, n + k_nb[NB_SE]) = ee;            // v_j(i+1,j+1)
            D(n + k, n + k_nb[NB_SW]) = -ee;           // v_j(i-1,j+1)
            D(n + k, n + k_nb[NB_NE]) = -ee;           // v_j(i-1,j-1)
            D(n + k, n + k_nb[NB_NW]) = ee;            // v_j(i+1,j-1)
            D(n + k,     k            ) = -2*dd;         // v_i(i,j)
            D(n + k,     k_nb[NB_S ]) = dd;            // v_i(i,j+1)
            D(n + k,     k_nb[NB_N ]) = dd;            // v_i(i,j-1)
            D(n + k,     k_nb[NB_SE]) = cc;            // v_i(i+1,j+1)
            D(n + k,     k_nb[NB_SW]) = -cc;           // v_i(i-1,j+1)
            D(n + k,     k_nb[NB_NE]) = -cc;           // v_i(i+1,j-1)
            D(n + k,     k_nb[NB_NW]) = cc;            // v_i(i-1,j-1)
         }
      }
      else
      {
         // 8.5 - Fill in the values for the fixed nodes
         D(k, k) = D(k+n, k+n) = 1;

         // Not going to do the inner loop here for 2 reasons:-
         // 1. D(k,...) = kronecker Delta already
         // 2. It doesn't matter whether D(...,k) is kronecker Delta,
         //    because the gradient at this point is zero, so there
         //    will be no effect from this point, mustn't further decouple
      }
   }
}

void DaviesBook::accumulate_reparametrization(DaviesBook::Matrix& cumulative_reparametrization, const DaviesBook::Matrix& proposed_reparametrization, const DaviesBook::FluidOptimizationParams& params)
{
   // Reform the cumulative reparametrization into an image
   ImageType current_offset(params.mx, params.my, 1, 2);

   for (IndexType y = 0; y < params.my; ++y)
   {
      for (IndexType x = 0; x < params.mx; ++x)
      {
         IndexType j = xy_to_index(x, y, params.mx, params.my);

         current_offset(x, y, 0, 0) = cumulative_reparametrization(j, 0);
         current_offset(x, y, 0, 1) = cumulative_reparametrization(j, 1);
      }
   }

   //std::cerr.precision(3);
   //DEBUGMACRO("cumulative_reparametrization = ");
   //for (IndexType ax = 0; ax < 2; ++ax)
   //{
   //   for (IndexType y = 0; y < params.my; ++y)
   //   {
   //      for (IndexType x = 0; x < params.mx; ++x)
   //      {
   //         IndexType j = xy_to_index(x, y, params.mx, params.my);
   //         std::cerr << "(" << cumulative_reparametrization(j, ax) << ")\t";
   //      }
   //      std::cerr << std::endl;
   //   }
   //   std::cerr << std::endl;
   //}

   //DEBUGMACRO("proposed_reparametrization = ");
   //for (IndexType ax = 0; ax < 2; ++ax)
   //{
   //   for (IndexType y = 0; y < params.my; ++y)
   //   {
   //      for (IndexType x = 0; x < params.mx; ++x)
   //      {
   //         IndexType j = xy_to_index(x, y, params.mx, params.my);
   //         std::cerr << "(" << proposed_reparametrization(j, ax) << ")\t";
   //      }
   //      std::cerr << std::endl;
   //   }
   //   std::cerr << std::endl;
   //}
   
   // Now resample at each of the grid points
   MT a[2];
   MT b[2];

   for (IndexType y = 0; y < params.my; ++y)
   {
      for (IndexType x = 0; x < params.mx; ++x)
      {
         IndexType j = xy_to_index(x, y, params.mx, params.my);

         // Treat the grid spacing as unity
         a[0] = proposed_reparametrization(j, 0);
         a[1] = proposed_reparametrization(j, 1);

         bilinear_interpolation<2>(current_offset, 1, a, b);

         // Handle wrapping
         {
            // Get the scaled coordinates
            MT aa[] = { a[0] / params.h, a[1] / params.h };

            // Get the deltas
            MT dx = aa[0] - floor(aa[0]);
            MT dy = aa[1] - floor(aa[1]);

            b[0] += floorquantize(aa[0], params.mx)*(1 - dx) + floorquantize(aa[0]+1, params.mx)*dx;
            b[1] += floorquantize(aa[1], params.my)*(1 - dy) + floorquantize(aa[1]+1, params.my)*dy;

            //std::cerr.precision(10);
            //DEBUGMACRO("(" << x << "," << y << ") -> (" << dx << "," << dy << ")");
            //PRINTVEC(a, 2);
            //PRINTVEC(aa, 2);
            //BLANKLINE;
            ///*
            //PRINTVBL(aa[0]);
            //PRINTVBL(floorquantize(aa[0], params.mx));
            //PRINTVBL(floorquantize(aa[0]+1, params.mx));
            //PRINTVBL(aa[1]);
            //PRINTVBL(floorquantize(aa[1], params.my));
            //PRINTVBL(floorquantize(aa[1]+1, params.my));
            //BLANKLINE;
            //*/
            //std::cerr.precision(3);

         }

         /*
         // The cumulative reparametrizations remain unwrapped
         // since this can be handled by the shape image interpolation
         for (IndexType ax = 0; ax < 2; ++ax)
         {
            MT delta = a[ax] - floor(a[ax]);
            IndexType size = (ax==0) ? params.mx : params.my;

            if (a[ax] < 0)
               b[ax] -= (1-delta) * size;
            else if (a[ax] >= (params.mx-1) )
               b[ax] += ( delta) * size;
         }
         */

         // Update the cumulative_reparametrization matrix
         cumulative_reparametrization(j, 0) = b[0];
         cumulative_reparametrization(j, 1) = b[1];
      }
   }

   //DEBUGMACRO("cumulative_reparametrization = ");
   //for (IndexType ax = 0; ax < 2; ++ax)
   //{
   //   for (IndexType y = 0; y < params.my; ++y)
   //   {
   //      for (IndexType x = 0; x < params.mx; ++x)
   //      {
   //         IndexType j = xy_to_index(x, y, params.mx, params.my);
   //         std::cerr << "(" << cumulative_reparametrization(j, ax) << ")\t";
   //      }
   //      std::cerr << std::endl;
   //   }
   //   std::cerr << std::endl;
   //}
}

DaviesBook::MT maxRms = 0;
DaviesBook::MT maxAbv = 0;

void DaviesBook::save_reparametrization(DaviesBook::Matrix& cumulative_reparametrization, 
                                         const DaviesBook::Matrix& grid_points, 
                                         DaviesBook::Matrix& displacement, ImageType& shape_image, 
                                         const ImageType& Original_Shape_Image, 
                                         const DaviesBook::FluidOptimizationParams& params)
{
   DEBUGMACRO("===== Saving Reparametrization");
   // Number of points in shape image
   const IndexType n = params.mx * params.my;

   // Get a deformed copy of the shape vector
   Matrix test_shape_vector[2];

   // This works out what we think the shape looks like right now
   populate_shape_vector(shape_image, params.h, displacement, test_shape_vector[0]);

   // 2.6.3 - Resample the shape image according to the accumulated reparametrization and new displacement
   accumulate_reparametrization(cumulative_reparametrization, grid_points-displacement, params);
   
   // 2.6.3.2 - Resample the original shape image to create a new shape image, sampled 
   //           according to Cumulative_Reparametrization
   MT a[2], b[3];
   for (IndexType j = 0; j < n; ++j)
   {
      // Calculate the x and y position
      const Coordinate coord = index_to_xy(j, params.mx, params.my);

      // Get the position from the cumulative reparametrization
      a[0] = cumulative_reparametrization(j, 0);
      a[1] = cumulative_reparametrization(j, 1);

      bilinear_interpolation<3>(Original_Shape_Image, params.h, a, b);

      // Update the shape image
      shape_image(coord.x, coord.y, 0) = b[0];
      shape_image(coord.x, coord.y, 1) = b[1];
      shape_image(coord.x, coord.y, 2) = b[2];
   }

   // 2.6.4 - Set all the elements of the displacement vector of the i-th training example to
   //         zero
   displacement.fill(0);

   // This works out what we think the shape looks after the accumulation - displacement is now
   // zero, but should give the same shape as test_shape_vector[0] if all went according to plan
   populate_shape_vector(shape_image, params.h, displacement, test_shape_vector[1]);

   MT rms = (test_shape_vector[1]-test_shape_vector[0]).fro_norm() / sqrt(static_cast<MT>(test_shape_vector[0].rows()));
   MT abv = (test_shape_vector[1]-test_shape_vector[0]).absolute_value_max();
   
   PRINTVBL(rms);
   PRINTVBL(abv);

   maxRms = std::max(rms, maxRms);
   maxAbv = std::max(abv, maxAbv);
   
   PRINTVBL(maxRms);
   PRINTVBL(maxAbv);
   BLANKLINE;
}

void DaviesBook::calculate_eigensystem(const DaviesBook::Matrix& mat, DaviesBook::Vector& eigenvalues, DaviesBook::Matrix& eigenvectors)
{
   EigenSystem eig(mat);

   const IndexType ns = mat.rows();

   eigenvalues.set_size(ns);
   eigenvectors.set_size(ns, ns);

   const bool flip = eig.get_eigenvalue(0) < eig.get_eigenvalue(ns-1);

   for (IndexType a = 0; a < ns; ++a)
   {
      IndexType idx = (flip) ? ((ns-1)-a) : a;

      eigenvalues(a) = std::max<MT>(0, eig.get_eigenvalue(idx));

      Vector eigv = eig.get_eigenvector(idx);

      for (IndexType r = 0; r < ns; ++r)
         eigenvectors(r, a) = eigv(r);
   }
}

void printf_matrix(const DaviesBook::Matrix& m)
{
   using namespace DaviesBook;

   for (IndexType r = 0; r < m.rows(); ++r)
   {
      for (IndexType c = 0; c < m.cols(); ++c)
      {
         printf("%f\t", m(r,c));
      }
      printf("\n");
   }
}

void DaviesBook::calculate_covariance(const DaviesBook::Matrix* shape_vector, const DaviesBook::FluidOptimizationParams params, DaviesBook::Matrix& Covariance)
{
   const IndexType n = shape_vector[0].rows();

   // Calculate the mean shape
   Matrix mean_shape;
   calculate_mean_shape_vector(shape_vector, params.ns, mean_shape);

   // Calculate triangle areas on the mean shape
   Vector int_area(n);
   MT total_area = calculate_int_triangle_areas(mean_shape, params.mx, params.my, int_area);

   // Make sure it is the correct size
   Covariance.set_size(params.ns, params.ns);

   // Calculate the Covariance matrix
   for (IndexType i = 0; i < params.ns; ++i)
   {
      for (IndexType j = i; j < params.ns; ++j)
      {
         Covariance(i, j) = 0;

         for (IndexType k = 0; k < n; ++k)
         {
            MT increment = 0;

            for (IndexType ax = 0; ax < 3; ++ax)
               increment += (shape_vector[i](k, ax) - mean_shape(k, ax)) * (shape_vector[j](k, ax) - mean_shape(k, ax));

            Covariance(i, j) += int_area(k) * increment;
         }

         Covariance(j, i) = Covariance(i, j);
      }
   }

   Covariance /= total_area;
}

DaviesBook::MT DaviesBook::calculate_L(const Matrix* shape_vector, const DaviesBook::FluidOptimizationParams& params, const bool report)
{
   Matrix Covariance;
   Matrix eigenvector;
   Vector eigenvalue;
   Vector dL_by_d_lam_a;

   calculate_covariance(shape_vector, params, Covariance);

   calculate_eigensystem(Covariance, eigenvalue, eigenvector);
   
   return calculate_L(eigenvalue, dL_by_d_lam_a, params);
}

DaviesBook::MT DaviesBook::calculate_L(const DaviesBook::Vector& eigenvalues, DaviesBook::Vector& dL_by_de, const DaviesBook::FluidOptimizationParams& params)
{
   MT ret = 0;

   dL_by_de.set_size(eigenvalues.size()-1);

   IndexType noise_counter = 0;
   for (IndexType i = 0; i < eigenvalues.size()-1; ++i)
   {
      if (eigenvalues(i) < eigenvalues(i+1))
         throw std::exception("Eigenvalues are not in order!");

      MT scaled_eig = eigenvalues(i) / params.lambda_cut;

      // Thodberg's form
      if (scaled_eig >= 1)
      {
         ret += 1 + log(scaled_eig);
         dL_by_de(i) = 1 / eigenvalues(i); // No need for twiddle, it's never too small
      }
      else if (scaled_eig > 1e-8)
      {
         ++noise_counter;

         ret += scaled_eig;
         dL_by_de(i) = 1 / params.lambda_cut;
      }
      else
      {
         ++noise_counter;

         ret += 1e-8;
         dL_by_de(i) = 0;
      }

      // Basic form
      //ret += log(scaled_eig);
      //dL_by_de(i) = 1 / (scaled_eig + params.eps);
   }

   //DEBUGMACRO(noise_counter << "/" << (eigenvalues.size()-1));

   return ret;
}

void DaviesBook::calculate_gradient_slow(const IndexType i, const ImageType& shape_image_stack, const Matrix* shape_vector, const Matrix& displacement, const FluidOptimizationParams& params, Vector& gradient)
{
   /*
   Vector* shape_vector_copy = new Vector[ params.ns ];

   // Copy all the other shape vectors
   for (IndexType s = 0; s < params.ns; ++s)
      shape_vector_copy[s] = shape_vector[s];
   
   Matrix displacement_copy = displacement;

   Matrix Covariance(params.ns, params.ns);
   Matrix eigenvector(params.ns, params.ns);
   Vector eigenvalue(params.ns);

   MT deltas[] = { -params.Delta, params.Delta };
   MT Ls[2];

   for (IndexType v = 0; v < displacement_copy.size(); ++v)
   {
      for (IndexType d = 0; d < 2; ++d)
      {
         // Perturb the displacement, and then repopulate the shape vector
         displacement_copy[v] = displacement[v] + deltas[d]*params.Delta;
         populate_shape_vector(shape_image_stack.get_shared_channel(i), params.h, displacement_copy, shape_vector_copy[i]);

         // Now re-evaluate the covariance matrix based on the updated shape vector
         calculate_covariance(shape_vector_copy, params, Covariance);

         calculate_eigensystem(Covariance, eigenvalue, eigenvector);
         
         Vector dL_by_d_lam_a(params.ns-1);

         Ls[d] = calculate_L(eigenvalue, dL_by_d_lam_a, params);
      }

      gradient[v] = (Ls[1] - Ls[0]) / (deltas[1] - deltas[0]);
      
      displacement_copy[v] = displacement[v];
   }

   delete [] shape_vector_copy;
   */
}

void DaviesBook::calculate_gradient(const ImageType& shape_image_stack, const Matrix* shape_vector, const Matrix* displacement, const FluidOptimizationParams& params, Vector* gradient, const IndexType icalc)
{
   Matrix* shape_vector_copy = new Matrix[ params.ns ];
   Matrix* displacement_copy = new Matrix[ params.ns ];

   // Copy all the other shape vectors
   for (IndexType s = 0; s < params.ns; ++s)
   {
      shape_vector_copy[s] = shape_vector[s];
      displacement_copy[s] = displacement[s];
   }
   
   Matrix Covariance(params.ns, params.ns);
   Matrix eigenvector(params.ns, params.ns);
   Vector eigenvalue(params.ns);
   Vector eigenvalue_gradient(params.ns-1);

   MT deltas[] = { -params.Delta, params.Delta };
   
   const IndexType n = displacement[0].rows();
   
   // Calculate the mean shape vector and its areas
   Matrix mean_shape_vector;
   calculate_mean_shape_vector(shape_vector, params.ns, mean_shape_vector);

   Vector int_area(n);
   MT total_area = calculate_int_triangle_areas(mean_shape_vector, params.mx, params.my, int_area);
   
   // Calculate the covariance matrix for the current shape set
   calculate_covariance(shape_vector, params, Covariance);
   calculate_eigensystem(Covariance, eigenvalue, eigenvector);
   calculate_L(eigenvalue, eigenvalue_gradient, params);

   MT dSbydP[3];
   Matrix dCovbydP(params.ns, params.ns);

   IndexType* is;
   IndexType  ni;

   if (icalc != std::numeric_limits<IndexType>::max())
   {
      ni = 1;
      is = new IndexType[ni];
      is[0] = icalc;
   }
   else
   {
      ni = params.ns;
      is = new IndexType[ni];
      for (IndexType i = 0; i < params.ns; ++i)
         is[i] = i;
   }

   for (IndexType ii = 0; ii < ni; ++ii)
   {
      const IndexType i = is[ii];

      // The gradient is always zero for the reference shape
      if (i == params.ref)
      {
         gradient[i].fill(0);
         continue;
      }

      for (IndexType vv = 0; vv < displacement_copy[i].rows()*displacement_copy[i].cols(); ++vv)
      {
         const IndexType v = vv % n;
         const Coordinate coord = index_to_xy(v, params.mx, params.my);

         MT gx = coord.x * params.h;
         MT gy = coord.y * params.h;

         MT vtxFast[2][3];

         for (IndexType d = 0; d < 2; ++d)
         {
            MT perturb[] = { 
               gx - displacement_copy[i](v, 0), 
               gy - displacement_copy[i](v, 1) 
            };

            if (vv < n)
               perturb[0] -= deltas[d]; // This is an x-displacement[i]
            else
               perturb[1] -= deltas[d]; // This is an y-displacement[i]
         
            bilinear_interpolation<3>(shape_image_stack.get_shared_channel(i), params.h, perturb, vtxFast[d]);
         }

         for (IndexType ax = 0; ax < 3; ++ax)
            dSbydP[ax] = (vtxFast[1][ax] - vtxFast[0][ax]) / (deltas[1] - deltas[0]);

         for (IndexType j = 0; j < params.ns; ++j)
         {
            for (IndexType k = 0; k < params.ns; ++k)
            {
               dCovbydP(j, k) = 0;

               for (IndexType ax = 0; ax < 3; ++ax)
               {
                  MT component = 
                     + (params.ns * kronecker(i, j) - 1) * (shape_vector[k](v, ax) - mean_shape_vector(v, ax))
                     + (params.ns * kronecker(i, k) - 1) * (shape_vector[j](v, ax) - mean_shape_vector(v, ax))
                     ;

                  dCovbydP(j, k) += component * dSbydP[ax];
               }
            }
         }

         dCovbydP /= params.ns * total_area / int_area(v);

         // Put these together to calculate the gradient[i]
         gradient[i][vv] = 0;
         for (IndexType a = 0; a < params.ns-1; ++a)
         {
            MT d_LamA_by_d_pv = 0;
            for (IndexType j = 0; j < params.ns; ++j)
            {
               for (IndexType k = 0; k < params.ns; ++k)
               {
                  MT dLamA_by_d_Cov_jk = eigenvector(j, a) * eigenvector(k, a);

                  d_LamA_by_d_pv += dLamA_by_d_Cov_jk * dCovbydP(j, k);
               }
            }

            gradient[i][vv] += eigenvalue_gradient(a) * d_LamA_by_d_pv;
         }
      }
   }

   delete [] shape_vector_copy;
   delete [] displacement_copy;
   delete [] is;
}

void DaviesBook::optimise_fluid(ImageType& shape_image_stack, const CImg<bool>& free_node_image, const FluidOptimizationParams& params)
{
   signal(SIGINT, handleSignal);

   // START OF INITIALIZATION

   // 1. Let ref be the index of the reference shape
   //    - This is being passed as a parameter

   // 2. Initialize the regrid counter to zero for all shapes
   //    - I'm not tracking this, I'm just storing the existing cumulative reparametrization
   
   // 3. Make a copy of the original shape images
   const ImageType Original_Shape_Image_Stack = shape_image_stack;

   // 4. Concatenate the coordinates of the mxm regular grid into a 2n-dimensional vector (hence n = params.mx*params.my)
   const IndexType n = params.mx*params.my;
   Matrix grid_points(n, 2);

   for (IndexType j = 0; j < params.my; ++j)
   {
      for (IndexType i = 0; i < params.mx; ++i)
      {
         grid_points(xy_to_index(i, j, params.mx, params.my), 0) = i * params.h;
         grid_points(xy_to_index(i, j, params.mx, params.my), 1) = j * params.h;
      }
   }

   // I am also going to create the cumulative reparametrization here...
   Matrix* cumulative_reparametrization = new Matrix[ params.ns ];
   for (IndexType i = 0; i < params.ns; ++i)
   {
      // Copy the grid points into the reparametrization
      cumulative_reparametrization[i] = grid_points;
   }

   // 5. Let displacement_i be a 2n-dimensional vector representing the displacemnt field of the i-th
   //    shape - the vector is arranged so that each component of displacement corresponds
   //    to a component of grid_points; each element of each vector is initialized to zero
   // 6. Create a 3n-dimensional shape vector for each training example

   //    - Let's do 5 and 6 together

   Matrix* displacement = new Matrix[ params.ns ];
   Matrix* shape_vector = new Matrix[ params.ns ];

   for (IndexType i = 0; i < params.ns; ++i)
   {
      displacement[i].set_size(n, 2);
      displacement[i].fill(0);
   
      shape_vector[i].set_size(n, 3);
      
      // Call the function which calculates this...
      // (of course, this is just reforming the grid, since displacement == 0)
      populate_shape_vector(shape_image_stack.get_shared_channel(i), params.h, displacement[i], shape_vector[i]);
   }

      
   // 7. Let free_node be a n-dimensional boolean valued vector...
   //    - Well, I am passing this as a parameter, but as an image.  It's not
   //      really necessary, but let's convert this into an array
   bool* free_node = new bool[ n ];
   for (IndexType j = 0; j < params.my; ++j)
   {
      for (IndexType i = 0; i < params.mx; ++i)
      {
         IndexType k = xy_to_index(i, j, params.mx, params.my);
         free_node[ k ] = free_node_image(i, j);
      }
   }

   // END OF INITIALIZATION

   DEBUGLINE;
   //write_shape_vectors(shape_vector, params.ns, n);

   // START OF OPTIMIZATION
   std::cerr << "##### START OF OPTIMIZATION" << std::endl;
   std::cerr << "! Not doing rigid alignment!" << std::endl;

   // Define these so that we don't have to do them each iteration
   Vector* gradient = new Vector[params.ns];
   for (IndexType i = 0; i < params.ns; ++i)
      gradient[i].set_size(2*n);

   Matrix eigenvector(params.ns, params.ns);
   Vector eigenvalue(params.ns);

   SparseMatrix D(2*n, 2*n);
   SparseLU* lu_decomp = 0;

   std::ofstream lfs("Ls_data.m");
   lfs << "Ls = [" << std::endl;

   Vector int_area(n);

   const IndexType NFIXED = 1;
   IndexType fixed_nodes_for_build[ NFIXED ];

   for (IndexType i = 0; i < NFIXED; ++i)
      fixed_nodes_for_build[i] = 0;

   MT lastL = std::numeric_limits<MT>::infinity();

   bool* has_converged = new bool[ params.ns ];
   for (IndexType i = 0; i < params.ns; ++i)
      has_converged[i] = false;
   has_converged[ params.ref ] = true;

   AWT::OneTimeMessage skippingRegrid("Skipping Regrid Check!");
   AWT::OneTimeMessage usingUnregularized("Using unregularized gradient!");
   AWT::OneTimeMessage usingRegularized("Using regularized gradient!");

   IndexType* order = new IndexType[ params.ns-1 ];
   IndexType oi = 0;
   for (IndexType i = 0; i < params.ns; ++i)
      if (i != params.ref)
         order[oi++] = i;

   // 1 - For each iteration...
   for (IndexType it = 0; it <= params.niterations && keepIterating; ++it)
   {
      BLANKLINE;
      BLANKLINE;
      DEBUGMACRO("========== " << it << " ==========");
      BLANKLINE;

      if ((it % (params.ns-1)) == 0)
         randomlyPermuteArray(order, params.ns-1);

      // Stage I8 is taken from the initialization, but given that it depends on {shape_vector}, this has
      // to be calculated each time

      // I8. Precompute the 2n x 2n (sparse) matrix, D, which holds the finite difference operations
      //    required to construct the set of second derivatives
      
      // I8.3 Create a 3n-dimensional vector to store the shape function and set it to the mean shape
      //     - Instead of mean_shape_vector, I'm going to call this mean_shape_vector, it seems a little 
      //       more sensible, a vector of this name is used for the same purpose later too
      //std::cerr << "  Calculating the mean shape vector" << std::endl;
      
      //std::cerr << "  Done." << std::endl;

      const bool rebuild_d = params.regularize && (it == 0); //it % params.iterations_between_D_rebuild == 0;

      if (rebuild_d)
      {
         if (lu_decomp != 0)
            delete lu_decomp;

         Matrix mean_shape_vector(n, 3);
         calculate_mean_shape_vector(shape_vector, params.ns, mean_shape_vector);

         // call to calculate the matrix of derivatives
         std::cerr << "  Calculating the derivative matrix" << std::endl;
         calculate_D(D, mean_shape_vector, free_node, params);
         std::cerr << "  Done." << std::endl;

         // 8.6 - Perform an LU decomposition of D to get a lower triangular matrix L and an upper
         //       triangular matrix U
         //       - Let VNL handle the LU decomposition - doesn't explicitly do the decomp until first solve
         lu_decomp = new SparseLU(D);
      }

      // Calculate what L is right now
      const MT currentL = calculate_L(shape_vector, params, true);
      PRINTVBL(currentL);
      //PAUSE;

      { 
         Matrix Covariance;
         Matrix eigenvector;
         Vector eigenvalue;
         Vector dL_by_d_lam_a;

         calculate_covariance(shape_vector, params, Covariance);

         calculate_eigensystem(Covariance, eigenvalue, eigenvector);

         lfs << it << "\t" << currentL;
      
         for (IndexType a = 0; a < params.ns-1; ++a)
            lfs << "\t" << eigenvalue(a);

         lfs << std::endl;
      }

      // Calculate the (unregularized) gradient of the cost function for all shapes
      // calculate_gradient(shape_image_stack, shape_vector, displacement, params, gradient);
      
      //std::cerr << "  Calculating deformation updates" << std::endl;
      // 2 - For each non-reference shape
      //for (IndexType i = 0; i < params.ns; ++i)
      {
         const IndexType i = order[it%(params.ns-1)];
         DEBUGMACRO("Only updating shape " << i);
         
         if (i != params.ref)
         {
            // Calculate the (unregularized) gradient of the cost function for all shapes
            calculate_gradient(shape_image_stack, shape_vector, displacement, params, gradient, i);
            has_converged[i] = false;

            Matrix R(n, 2);
            MT max_norm_sq = 0;

            if (params.regularize)
            {
               usingRegularized.report();
               //std::cerr << "    Solving for velocity for shape " << i << std::endl;
               // 2.1 - Solve the linear PDE
               // -gradient_i = D velocity
               
               // Do this using the LU decomposition
               Vector velocity = lu_decomp->solve(gradient[i]);
               
               //std::cerr << "    Done." << std::endl;
               
               // 2.2 - Calculate the n-dimensional vectors corresponding tothe gradient of the displacement
               //       field of the i-th shape, displacement, along the x and y axes:
               // 2.3 - Calculate the n x 2 regularization matrix R

               MT duxdx, duxdy, duydx, duydy;

               //std::cerr << "    Calculating regularized update for shape " << i << std::endl;
               
               for (IndexType j = 0; j < n; ++j)
               {
                  if (free_node[j])
                  {
                     IndexType nbs[9];
                     neighbours(j, params.mx, params.my, nbs);

                     duxdy = (displacement[i](nbs[NB_E], 0) - displacement[i](nbs[NB_W], 0)) / (2*params.h);
                     duxdx = (displacement[i](nbs[NB_S], 0) - displacement[i](nbs[NB_N], 0)) / (2*params.h);

                     duydy = (displacement[i](nbs[NB_E], 1) - displacement[i](nbs[NB_W], 1)) / (2*params.h);
                     duydx = (displacement[i](nbs[NB_S], 1) - displacement[i](nbs[NB_N], 1)) / (2*params.h);
                  }
                  else
                  {
                     duxdx = 0;
                     duxdy = 0;
                     duydx = 0;
                     duydy = 0;
                  }
               
                  // Calculate the velocity-regularized update
                  R(j, 0) = velocity(j + 0*n) - (velocity(j + 0*n) * duxdx + velocity(j + 1*n) * duxdy);
                  R(j, 1) = velocity(j + 1*n) - (velocity(j + 0*n) * duydx + velocity(j + 1*n) * duydy);

                  MT this_norm_sq = R(j, 0)*R(j, 0) + R(j, 1)*R(j, 1);

                  if (this_norm_sq > max_norm_sq)
                     max_norm_sq = this_norm_sq;
               }
            }
            else
            {
               usingUnregularized.report();
               for (IndexType j = 0; j < n; ++j)
               {
                  R(j, 0) = -gradient[i](j + 0*n);
                  R(j, 1) = -gradient[i](j + 1*n);

                  MT this_norm_sq = R(j, 0)*R(j, 0) + R(j, 1)*R(j, 1);

                  if (this_norm_sq > max_norm_sq)
                     max_norm_sq = this_norm_sq;
               }
            }
            //std::cerr << "    Done." << std::endl;
            
            // 2.4 - Calculate the timestep as Delta / max(norm(R))
            MT dt = params.timestep_mult / sqrt(max_norm_sq);

            if (true)
            {
               // Let's see if we can optimize the time step
               const Matrix current_displacement = displacement[i];

               // Do a Nelder-Mead line search to find our optimum dt

               // Tuning parameters
               const MT alpha = 1;
               const MT gamma = 2;
               const MT rho   = static_cast<MT>(1) / 2;
               const MT sigma = static_cast<MT>(1) / 2;

               // The initial simplex
               MT x[] = { 0, dt };
               MT fx[2];

               for (IndexType xi = 0; xi < 2; ++xi)
               {
                  displacement[i] = current_displacement + x[xi] * R;
                  populate_shape_vector(shape_image_stack.get_shared_channel(i), params.h, displacement[i], shape_vector[i]);
                  fx[xi] = calculate_L(shape_vector, params);
               }

               DEBUGMACRO("Starting Line Search");
               for (IndexType nelderIters = 0; true; ++nelderIters)
               {
                  // Sort the simplex into ascending order - simple, it's just two things
                  if (fx[0] > fx[1])
                  {
                     swap(x[0], x[1]);
                     swap(fx[0], fx[1]);
                  }

                  if (nelderIters > 100)
                     break;

                  // Terminate when the simplex is too small
                  if (abs(x[1] - x[0]) < 1e-16 && (x[0] != 0))
                     break;
                  
                  // Compute a reflection
                  // The centre of gravity excluding the worst point is just the 0-th point
                  const MT x0 = x[0];

                  // Calculate f(xr)
                  const MT xr = x0 + alpha * (x0 - x[1]);
                  populate_shape_vector(shape_image_stack.get_shared_channel(i), params.h, current_displacement + xr * R, shape_vector[i]);
                  const MT fxr = calculate_L(shape_vector, params);

                  if (fxr >= fx[0] && fxr < fx[1])
                  {
                     // If the f(xr) is between our current simplex values, accept this as the "second best" point
                     x[1]  = xr;
                     fx[1] = fxr;
                  }
                  else if (fxr < fx[0])
                  {
                     // xr is the best point so far, so expand
                     // Calculate f(xe)
                     const MT xe = x0 + gamma * (x0 - x[1]);
                     populate_shape_vector(shape_image_stack.get_shared_channel(i), params.h, current_displacement + xe * R, shape_vector[i]);
                     const MT fxe = calculate_L(shape_vector, params);

                     // Check to see if the expansion move is better again...
                     if (fxe < fxr)
                     {
                        // Replace the worst point with xe
                        x[1]  = xe;
                        fx[1] = fxe;
                     }
                     else
                     {
                        // Replace the worst point with xr
                        x[1]  = xr;
                        fx[1] = fxr;
                     }
                  }
                  else
                  {
                     // Contraction
                     const MT xc = x[1] + rho * (x0 - x[1]);

                     // Calculate f(xc)
                     populate_shape_vector(shape_image_stack.get_shared_channel(i), params.h, current_displacement + xc * R, shape_vector[i]);
                     const MT fxc = calculate_L(shape_vector, params);

                     if (fxc < fx[1])
                     {
                        // Replace the worst point with xr
                        x[1]  = xc;
                        fx[1] = fxc;
                     }
                     else
                     {
                        // Shrink the simplex towards the best point
                        x[1] = x[0] + sigma * (x[1] - x[0]);

                        // Calculate f(x[1])
                        populate_shape_vector(shape_image_stack.get_shared_channel(i), params.h, current_displacement + x[1] * R, shape_vector[i]);
                        fx[1] = calculate_L(shape_vector, params);
                     }
                  }
               }
               DEBUGMACRO("Finished Line Search");

               // Make sure displacement is put back to original
               displacement[i] = current_displacement;

               dt = x[0];
            }

            if (dt == 0)
            {
               DEBUGLINE;
               has_converged[i] = true;

               DEBUGMACRO("===== Updating displacement with dt = 0");
               //std::cerr << "    Updating displacement field for shape " << i << std::endl;
               // Accept the displacement
               displacement[i] += dt * R;

               // 3 - Update the i-th shape vector
               //std::cerr << "    Updating shape vector " << i << std::endl;
               populate_shape_vector(shape_image_stack.get_shared_channel(i), params.h, displacement[i], shape_vector[i]);
               //std::cerr << "    Done." << std::endl;

               DEBUGMACRO("Saving reparametrization for shape " << i);
               save_reparametrization(cumulative_reparametrization[i], grid_points, displacement[i], 
                  shape_image_stack.get_shared_channel(i), Original_Shape_Image_Stack.get_shared_channel(i), params);
            }
            else
            {
               // std::cerr << "    Time step = " << dt << " for shape " << i << std::endl;

               // 2.5 - Calculate the proposed displacement
               //       - Let's call this proposed_reparametrization instead, since that's what it is
               Matrix proposed_reparametrization = grid_points - (displacement[i] + dt*R);

               // Calculate the Jacobian of m . proposed_reparametrization
               IndexType nbs[9];

               bool need_to_regrid = false && rebuild_d;
               IndexType min_m = (params.mx < params.my) ? params.mx : params.my;

               //std::cerr << "    Checking if need to regrid shape " << i << std::endl;
               for (IndexType j = 0; j < n && !need_to_regrid; ++j)
               {
                  skippingRegrid.report();
                  break;

                  // Get the neighbouring indices
                  neighbours(j, params.mx, params.my, nbs);

                  // Take off the current grid point
                  // Add on the centre grid point
                  // Subtract/Add on one grid square

                  const MT off_x = grid_points(nbs[NB_C], 0);
                  MT n_x = proposed_reparametrization(nbs[NB_N], 0) - grid_points(nbs[NB_N], 0) + off_x;
                  MT s_x = proposed_reparametrization(nbs[NB_S], 0) - grid_points(nbs[NB_S], 0) + off_x;
                  MT w_x = proposed_reparametrization(nbs[NB_W], 0) - grid_points(nbs[NB_W], 0) + off_x - params.h;
                  MT e_x = proposed_reparametrization(nbs[NB_E], 0) - grid_points(nbs[NB_E], 0) + off_x + params.h;

                  const MT off_y = grid_points(nbs[NB_C], 1);
                  MT n_y = proposed_reparametrization(nbs[NB_N], 1) - grid_points(nbs[NB_N], 1) + off_y - params.h;
                  MT s_y = proposed_reparametrization(nbs[NB_S], 1) - grid_points(nbs[NB_S], 1) + off_y + params.h;
                  MT w_y = proposed_reparametrization(nbs[NB_W], 1) - grid_points(nbs[NB_W], 1) + off_y;
                  MT e_y = proposed_reparametrization(nbs[NB_E], 1) - grid_points(nbs[NB_E], 1) + off_y;

                  // J = det([ dPx/dx dPx/dy; dPy/dx dPy/dy ])
                  MT dPx_x = (e_x - w_x) / (2*params.h);
                  MT dPx_y = (s_x - n_x) / (2*params.h);

                  MT dPy_x = (e_y - w_y) / (2*params.h);
                  MT dPy_y = (s_y - n_y) / (2*params.h);

                  MT det_J = dPx_x * dPy_y - dPx_y * dPy_x;

                  // We are only looking to see if m . det_J < 0.5 at any point, so we can fast break if it is
                  need_to_regrid = (min_m * det_J) < 0.5;
               }

               // 2.6 - If the Jacobian of m . proposed_displacement is less than 0.5 at any point
               if (need_to_regrid)
               {
                  std::cerr << "    Regridding shape " << i << std::endl;
                  //PAUSE;

                  save_reparametrization(cumulative_reparametrization[i], grid_points, displacement[i], 
                     shape_image_stack.get_shared_channel(i), Original_Shape_Image_Stack.get_shared_channel(i), params);
                  
                  std::cerr << "    Done." << std::endl;
               }
               else
               {
                  DEBUGMACRO("===== Updating displacement");
                  //std::cerr << "    Updating displacement field for shape " << i << std::endl;
                  // Accept the displacement
                  displacement[i] += dt * R;
               }

               // 3 - Update the i-th shape vector
               //std::cerr << "    Updating shape vector " << i << std::endl;
               populate_shape_vector(shape_image_stack.get_shared_channel(i), params.h, displacement[i], shape_vector[i]);
               //std::cerr << "    Done." << std::endl;

               DEBUGMACRO("Saving reparametrization for shape " << i);
               save_reparametrization(cumulative_reparametrization[i], grid_points, displacement[i], 
                  shape_image_stack.get_shared_channel(i), Original_Shape_Image_Stack.get_shared_channel(i), params);

            }
         }
         else
         {
            has_converged[i] = true;
         }
      }

      {
         const MT currentL = calculate_L(shape_vector, params, true);
         PRINTVBL(currentL);
         PAUSE;
      }

      bool all_converged = true;

      for (IndexType i = 0; i < params.ns && all_converged; ++i)
         if (!has_converged[i])
            all_converged = false;

      if (all_converged)
         break;
   }
   lfs << "];" << std::endl;
   lfs.close();

   std::cerr << "##### END OF OPTIMIZATION" << std::endl;

   DEBUGLINEANDEXIT(1);

   // END OF OPTIMIZATION

   std::cerr << "##### START OF POSTPROCESSING" << std::endl;
   // POST PROCESSING
   for (IndexType i = 0; i < params.ns; ++i)
   {
      // 1 - Save the final reparametrization
      // 2 - Calculate the accumulated reparametrization
      // 3 - Let Reparametrization[i] = cumulative_reparametrization[i]

      std::cerr << "    Calculating final reparametrization for shape " << i << std::endl;

      save_reparametrization(cumulative_reparametrization[i], grid_points, displacement[i], 
                  shape_image_stack.get_shared_channel(i), Original_Shape_Image_Stack.get_shared_channel(i), params);
      
      // I am passing back the reparametrization using the shape images
      std::cerr << "    Done." << std::endl;
   }

   std::cerr << "##### END OF POSTPROCESSING" << std::endl;

   write_shape_vectors(shape_vector, params.ns, n, false);

   // Tidy up
   delete [] cumulative_reparametrization;
   delete [] gradient;
   delete [] free_node;
   delete [] shape_vector;
   delete [] displacement;
   delete [] has_converged;

   if (lu_decomp != 0)
      delete lu_decomp;
}

void DaviesBook::write_shape_vectors(const DaviesBook::Matrix* shape_vector, const DaviesBook::IndexType ns, const DaviesBook::IndexType n, const bool pause)
{
   std::ofstream fs("shapes_script.m");
   fs << "clear; clc" << std::endl;

   for (IndexType i = 0; i < ns; ++i)
   {
      fs << "shapes{" << (i+1) << "} = [";

      for (IndexType j = 0; j < n; ++j)
      {
         fs << shape_vector[i](j, 0) << " " << shape_vector[i](j, 1) << " " << shape_vector[i](j, 2) << std::endl;
      }

      fs << "]';" << std::endl;
   }

   fs << "clf; hold on;" << std::endl;

   for (IndexType i = 0; i < ns; ++i)
   {
      //fs << "scatter3x(shapes{" << (i+1) << "}); axis equal tight" << std::endl;
      fs << "plot(1:1024,shapes{" << (i+1) << "}(1,:), 1:1024,shapes{" << (i+1) << "}(2,:), 1:1024,shapes{" << (i+1) << "}(3,:))" << std::endl;
   }
   fs << "disp(sqrt(sum(sum((shapes{1}-shapes{2}).^2)) / " << n << "));" << std::endl;

   fs.close();
   DEBUGMACRO("Written to shapes_script.m");

   if (pause) PAUSE;
}

#undef NB_NW
#undef NB_N
#undef NB_NE

#undef NB_W
#undef NB_C
#undef NB_E

#undef NB_SW
#undef NB_S 
#undef NB_SE
