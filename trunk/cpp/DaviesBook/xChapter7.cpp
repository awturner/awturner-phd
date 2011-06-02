#include "Chapter7.h"

#include "Useful/PrintMacros.h"
#include <iostream>
#include <fstream>

const char* DaviesBook::dir_name( const DaviesBook::MoveDirection dir )
{
   switch ( dir )
   {
   case N: return "N";
   case NE: return "NE";
   case NW: return "NW";
   case W: return "W";
   case E: return "E";
   case S: return "S";
   case SE: return "SE";
   case SW: return "SW";
   default:
      return "Unknown direction";
   }
}

const DaviesBook::IndexType DaviesBook::castToIndex( const MT x )
{
   return static_cast<const IndexType>( x );
}

DaviesBook::MT DaviesBook::dot( const MT* a, const MT* b, const IndexType K )
{
   MT ret = 0;

   for ( IndexType i = 0; i < K; ++i )
      ret += a[i]*b[i];

   return ret;
}

const DaviesBook::MT DaviesBook::wrap( const DaviesBook::MT x, const DaviesBook::MT lower, const DaviesBook::MT upper )
{
   return x - floor( ( x - lower ) / ( upper - lower ) ) * ( upper - lower );
}

const DaviesBook::IndexType DaviesBook::index_move( const DaviesBook::IndexType k, const DaviesBook::IndexType mx, const DaviesBook::IndexType my, const DaviesBook::MoveDirection dir )
{
   // Calculate the current grid x and y
   const Coordinate coord = index_to_xy( k, mx, my );

   IndexType xnew = coord.x, ynew = coord.y;

   switch ( dir )
   {
   case W:
   case NW:
   case SW:
      xnew = ( ( mx + coord.x ) - 1 ) % mx;
      break;
   case E:
   case NE:
   case SE:
      xnew = ( coord.x + 1      ) % mx;
      break;
   }

   switch ( dir )
   {
   case N:
   case NE:
   case NW:
      ynew = ( ( my + coord.y ) - 1 ) % my;
      break;
   case S:
   case SE:
   case SW:
      ynew = ( coord.y + 1      ) % my;
      break;
   }

   if ( xnew >= mx )
   {
      throw std::exception( "xnew >= mx" );
   }

   if ( ynew >= my )
   {
      throw std::exception( "ynew >= my" );
   }
   
   return xy_to_index( xnew, ynew, mx, my );
}

#define NB_1_NW 0
#define NB_1_N  1
#define NB_1_NE 2

#define NB_1_W  3
#define NB_1_C  4
#define NB_1_E  5

#define NB_1_SW 6
#define NB_1_S  7
#define NB_1_SE 8

#define NB_2_NW 0][0
#define NB_2_N  0][1
#define NB_2_NE 0][2

#define NB_2_W  1][0
#define NB_2_C  1][1
#define NB_2_E  1][2

#define NB_2_SW 2][0
#define NB_2_S  2][1
#define NB_2_SE 2][2

void DaviesBook::neighbours( const DaviesBook::IndexType index, const DaviesBook::IndexType mx, 
                                  const DaviesBook::IndexType my, DaviesBook::IndexType neighbours[3][3] )
{
   neighbours[NB_2_NW] = index_move( index, mx, my, NW );
   neighbours[NB_2_N ] = index_move( index, mx, my, N  );
   neighbours[NB_2_NE] = index_move( index, mx, my, NE );

   neighbours[NB_2_W ] = index_move( index, mx, my, W  );
   neighbours[NB_2_C ] = index;
   neighbours[NB_2_E ] = index_move( index, mx, my, E  );

   neighbours[NB_2_SW] = index_move( index, mx, my, SW );
   neighbours[NB_2_S ] = index_move( index, mx, my, S  );
   neighbours[NB_2_SE] = index_move( index, mx, my, SE );
}

void DaviesBook::neighbours( const DaviesBook::IndexType index, const DaviesBook::IndexType mx, 
                                  const DaviesBook::IndexType my, DaviesBook::IndexType neighbours[9] )
{
   neighbours[ 0] = index_move( index, mx, my, NW );
   neighbours[ 1] = index_move( index, mx, my, N  );
   neighbours[ 2] = index_move( index, mx, my, NE );

   neighbours[ 3] = index_move( index, mx, my, W  );
   neighbours[ 4] = index;
   neighbours[ 5] = index_move( index, mx, my, E  );

   neighbours[ 6] = index_move( index, mx, my, SW );
   neighbours[ 7] = index_move( index, mx, my, S  );
   neighbours[ 8] = index_move( index, mx, my, SE );
}

void DaviesBook::bilinear_interpolation( const ImageType& shape_image, const MT h, const MT a[2], MT b[3] )
{
   // Scale into the image's coordinate frame and wrap around - this is explicitly assuming closed 1-toroid topology
   MT ah[] = { wrap( a[0]/h, 0, shape_image.dimx() ), wrap( a[1]/h, 0, shape_image.dimy() ) };

   IndexType x0 = static_cast<IndexType>( ah[0] );
   IndexType y0 = static_cast<IndexType>( ah[1] );

   // Wrap around if necessary
   IndexType x1 = ( x0 + 1 ) % shape_image.dimx();
   IndexType y1 = ( y0 + 1 ) % shape_image.dimy();

   // Calculate the fractional parts
   MT dx = ah[0] - x0;
   MT dy = ah[1] - y0;

   /*
   PRINTVEC( a, 2 );
   PRINTVEC( ah, 2 );

   PRINTVBL( x0 );
   PRINTVBL( y0 );
   PRINTVBL( x1 );
   PRINTVBL( y1 );

   PRINTVBL( dx );
   PRINTVBL( dy );

   PAUSE;
   */

   MT v00, v01, v10, v11;

   // Do the bilinear interpolation
   for ( IndexType ax = 0; ax < 3; ++ax )
   {
      v00 = shape_image( x0, y0, ax );
      v01 = shape_image( x1, y0, ax );

      v10 = shape_image( x0, y1, ax );
      v11 = shape_image( x1, y1, ax );

      b[ax] = ( 1 - dy ) * ( ( 1 - dx ) * v00 + dx * v01 ) + dy * ( ( 1 - dx ) * v10 + dx * v11 );
   }

   //BLANKLINE;
}

void DaviesBook::populate_shape_vector( const ImageType& shape_image, const MT h, const Matrix& displacement, Matrix& shape_vector )
{
   const IndexType n = shape_image.dimx() * shape_image.dimy();

   // Do a little bit of size checking
   if ( displacement.rows( ) != n )
      throw std::exception( "Displacement vector is the wrong size" );

   if ( shape_vector.rows() != n )
      throw std::exception( "Shape vector is the wrong size" );

   const IndexType mx = shape_image.dimx();
   const IndexType my = shape_image.dimy();

   /*
   std::ofstream fs( "shapei.m" );
   fs << "clear; close all; clc; " << std::endl;
   
   fs << "shpv = [" << std::endl;
   */

   // Now get down to work
   MT sample_point[2];
   MT vtx[3];
   for ( IndexType j = 0; j < n; ++j )
   {
      // Assume that the points go in column major order, i.e. down columns first
      const Coordinate grid_pos = index_to_xy( j, mx, my );

      sample_point[0] = grid_pos.x * h + displacement( j, 0 );
      sample_point[1] = grid_pos.y * h + displacement( j, 1 );

      bilinear_interpolation( shape_image, h, sample_point, vtx );

      shape_vector( j, 0 ) = vtx[0];
      shape_vector( j, 1 ) = vtx[1];
      shape_vector( j, 2 ) = vtx[2];

      //fs << vtx[0] << " " << vtx[1] << " " << vtx[2] << std::endl;
   }

   //fs << "]'; scatter3x(shpv); axis equal; hold on" << std::endl;

   //fs << "shp = [" << std::endl;
   for ( IndexType k = 0; k < n; ++k )
   {
      Coordinate coord = index_to_xy( k, mx, my );

      //fs << shape_image( coord.x, coord.y, 0 ) << " " << shape_image( coord.x, coord.y, 1 ) << " " << shape_image( coord.x, coord.y, 2 ) << std::endl;
      //fs << shape_vector[i]( k, 0 ) << " " << shape_vector[i]( k, 1 ) << " " << shape_vector[i]( k, 2 ) << std::endl;
   }
   //fs << "]'; scatter3x(shp); axis equal; hold on" << std::endl;
   //fs.close( );

   //PAUSE;
}

void DaviesBook::calculate_mean_shape_vector( const DaviesBook::Matrix* shape_vector, const IndexType ns, DaviesBook::Matrix& mean_shape_vector )
{
   mean_shape_vector.fill( 0 );

   for ( IndexType i = 0; i < ns; ++i )
      mean_shape_vector += shape_vector[i];

   mean_shape_vector /= ns;
}

const DaviesBook::Coordinate DaviesBook::index_to_xy( const DaviesBook::IndexType index, const DaviesBook::IndexType mx, const DaviesBook::IndexType my )
{
   Coordinate ret;
   ret.x = index / my;
   ret.y = index % my;
   return ret;
}

const DaviesBook::IndexType DaviesBook::xy_to_index( const DaviesBook::IndexType x, const DaviesBook::IndexType y, const DaviesBook::IndexType /*mx*/, const DaviesBook::IndexType my )
{
   return x * my + y;
}

const DaviesBook::MT DaviesBook::calculate_int_triangle_areas( const DaviesBook::Matrix& shape_vector, const DaviesBook::IndexType mx, const DaviesBook::IndexType my, DaviesBook::Vector& areas )
{
   // Number of points
   const IndexType n = mx * my;

   // Assume an implicit triangulation of the plane
   IndexType triangles[] = {
      NB_1_N , NB_1_NE, NB_1_C ,  // COUNT THIS IN TOTAL SURFACE AREA
      NB_1_NE, NB_1_E , NB_1_C ,  // COUNT THIS IN TOTAL SURFACE AREA
      NB_1_C , NB_1_E , NB_1_S ,
      NB_1_C , NB_1_S , NB_1_SW,
      NB_1_W , NB_1_C , NB_1_SW,
      NB_1_N , NB_1_C , NB_1_W 
   };

   MT p[3][3];

   MT ret = 0;

   for ( IndexType i = 0; i < n; ++i )
   {
      // Reset the area
      areas( i ) = 0;

      // Get the adjacent indices
      IndexType movedIndexes[9];
      neighbours( i, mx, my, movedIndexes );

      //PRINTVBL( i );

      //std::cerr << movedIndexes[0] << "\t" << movedIndexes[1] << "\t" << movedIndexes[2] << "\t" << std::endl;
      //std::cerr << movedIndexes[3] << "\t" << movedIndexes[4] << "\t" << movedIndexes[5] << "\t" << std::endl;
      //std::cerr << movedIndexes[6] << "\t" << movedIndexes[7] << "\t" << movedIndexes[8] << "\t" << std::endl;
      
      // For each of the 6 surrounding triangles
      for ( IndexType t = 0; t < 6; ++t )
      {
         // Print out the triangle
         // std::cerr << "Triangle " << t << ": " << movedIndexes[ triangles[3*t + 0] ] << ", " << movedIndexes[ triangles[3*t + 1] ] << ", " << movedIndexes[ triangles[3*t + 2] ] << std::endl;

         // Get the coordinates of the vertices
         for ( IndexType v = 0; v < 3; ++v )
            for ( IndexType ax = 0; ax < 3; ++ax )
            {
               p[v][ax] = shape_vector( movedIndexes[ triangles[3*t+v] ], ax );
            }

         // Relative to the 0-th vertex
         for ( IndexType ax = 0; ax < 3; ++ax )
         {
            p[1][ax] -= p[0][ax];
            p[2][ax] -= p[0][ax];
         }

         // Calculate the cross product
         p[0][0] = p[1][1]*p[2][2] - p[1][2]*p[2][1];
         p[0][1] = p[1][2]*p[2][0] - p[1][0]*p[2][2];
         p[0][2] = p[1][0]*p[2][1] - p[2][0]*p[1][1];

         // Calculate the magnitude of this
         MT this_area = sqrt( dot( p[0], p[0], 3 ) );

         // Add to the totals
         areas( i ) += this_area;

         if ( t < 2 )
            ret += this_area;
      }
   }

   return ret;
}

const DaviesBook::MT DaviesBook::kronecker( const DaviesBook::IndexType a, const DaviesBook::IndexType b )
{
   return ( a == b ) ? 1 : 0;
}

void calculate_D( DaviesBook::SparseMatrix& D, const DaviesBook::Matrix& mean_shape_vector, const bool* free_node, const DaviesBook::FluidOptimizationParams& params )
{
   using namespace DaviesBook;

   const IndexType n = mean_shape_vector.size( ) / 3;

   // 8.1 Initialize by setting all elements of D to zero
   //     - vnl's sparse matrix is by default all zero, so do nothing

   // 8.2 Let...
   const MT hh = params.h * params.h;

   const MT a = ( 2*params.visc_mu + params.visc_lambda ) /    hh;
   const MT b =     params.visc_mu                        /    hh;
   const MT c = (   params.visc_mu + params.visc_lambda ) / (4*hh);
   const MT d = (   params.visc_mu + params.visc_lambda ) /    hh;
   const MT e = ( 3*params.visc_mu + params.visc_lambda ) / (4*hh);

   IndexType k_nb[3][3];

   // 8.4 Populate the matrix using a finite difference scheme
   MT grad_i[3], grad_j[3], metric_times_h3[2][2], inverse_metric[2][2];
   for ( IndexType k = 0; k < n; ++k )
   {
      if ( free_node[k] )
      {
         // Calculate the neighbours
         neighbours( k, params.mx, params.my, k_nb );

         // Calculate the gradient of the shape function in the i and j directions and
         // store them in three-dimensional vectors
         for ( IndexType ax = 0; ax < 3; ++ax )
         {
            grad_i[ax] = ( mean_shape_vector[ k_nb[NB_2_E ], ax ] - mean_shape_vector[ k_nb[NB_2_W ], ax ] ) / 2;
            grad_j[ax] = ( mean_shape_vector[ k_nb[NB_2_N ], ax ] - mean_shape_vector[ k_nb[NB_2_S ], ax ] ) / 2;
         }

         // Calculate the 2x2 matrix Metric - corresponds to g in (8.32)
         // - I'm almost calculating this, just doing a slight optimization in not dividing by h^3
         metric_times_h3[0][0]                         = dot( grad_i, grad_i, 3 );
         metric_times_h3[0][1] = metric_times_h3[1][0] = dot( grad_i, grad_j, 3 );
         metric_times_h3[1][1]                         = dot( grad_j, grad_j, 3 );

         // Calculate the inverse of the metric_times_h3 matrix
         MT idet = ( params.h * params.h * params.h ) / ( metric_times_h3[0][0]*metric_times_h3[1][1] - metric_times_h3[0][1]*metric_times_h3[1][0] );

         inverse_metric[0][0] =  metric_times_h3[1][1] * idet;
         inverse_metric[0][1] = -metric_times_h3[0][1] * idet;
         inverse_metric[1][0] = -metric_times_h3[1][0] * idet;
         inverse_metric[1][1] =  metric_times_h3[0][0] * idet;

         // Fill in the finite difference operations for the i component
         {
            const MT aa = a * inverse_metric[0][0];
            const MT bb = b * inverse_metric[1][1];
            const MT cc = c * inverse_metric[0][0];
            const MT dd = d * inverse_metric[0][1];
            const MT ee = e * inverse_metric[0][1];

            D(     k,     k             ) = - 2*aa - 2*bb; // v_i(i,j)
            D(     k,     k_nb[NB_2_W ] ) = aa;            // v_i(i-1,j)
            D(     k,     k_nb[NB_2_E ] ) = aa;            // v_i(i+1,j)
            D(     k,     k_nb[NB_2_N ] ) = bb;            // v_i(i,j-1)
            D(     k,     k_nb[NB_2_S ] ) = bb;            // v_i(i,j+1)
            D(     k,     k_nb[NB_2_SE] ) = ee;            // v_i(i+1,j+1)
            D(     k,     k_nb[NB_2_SW] ) = -ee;           // v_i(i-1,j+1)
            D(     k,     k_nb[NB_2_NE] ) = -ee;           // v_i(i-1,j-1)
            D(     k,     k_nb[NB_2_NW] ) = ee;            // v_i(i+1,j-1)
            D(     k, n + k             ) = -2*dd;         // v_j(i,j)
            D(     k, n + k_nb[NB_2_S ] ) = dd;            // v_j(i,j+1)
            D(     k, n + k_nb[NB_2_N ] ) = dd;            // v_j(i,j-1)
            D(     k, n + k_nb[NB_2_SE] ) = cc;            // v_j(i+1,j+1)
            D(     k, n + k_nb[NB_2_SW] ) = -cc;           // v_j(i-1,j+1)
            D(     k, n + k_nb[NB_2_NE] ) = -cc;           // v_j(i+1,j-1)
            D(     k, n + k_nb[NB_2_NW] ) = cc;            // v_j(i-1,j-1)
         }

         // Now do the same for the j component
         {
            const MT aa = a * inverse_metric[1][1];
            const MT bb = b * inverse_metric[0][0];
            const MT cc = c * inverse_metric[1][1];
            const MT dd = d * inverse_metric[0][1];
            const MT ee = e * inverse_metric[0][1];

            D( n + k, n + k             ) = - 2*aa - 2*bb; // v_j(i,j)
            D( n + k, n + k_nb[NB_2_N ] ) = aa;            // v_j(i-1,j)
            D( n + k, n + k_nb[NB_2_S ] ) = aa;            // v_j(i+1,j)
            D( n + k, n + k_nb[NB_2_W ] ) = bb;            // v_j(i,j-1)
            D( n + k, n + k_nb[NB_2_E ] ) = bb;            // v_j(i,j+1)
            D( n + k, n + k_nb[NB_2_SE] ) = ee;            // v_j(i+1,j+1)
            D( n + k, n + k_nb[NB_2_SW] ) = -ee;           // v_j(i-1,j+1)
            D( n + k, n + k_nb[NB_2_NE] ) = -ee;           // v_j(i-1,j-1)
            D( n + k, n + k_nb[NB_2_NW] ) = ee;            // v_j(i+1,j-1)
            D( n + k,     k             ) = -2*dd;         // v_i(i,j)
            D( n + k,     k_nb[NB_2_S ] ) = dd;            // v_i(i,j+1)
            D( n + k,     k_nb[NB_2_N ] ) = dd;            // v_i(i,j-1)
            D( n + k,     k_nb[NB_2_SE] ) = cc;            // v_i(i+1,j+1)
            D( n + k,     k_nb[NB_2_SW] ) = -cc;           // v_i(i-1,j+1)
            D( n + k,     k_nb[NB_2_NE] ) = -cc;           // v_i(i+1,j-1)
            D( n + k,     k_nb[NB_2_NW] ) = cc;            // v_i(i-1,j-1)
         }
      }
      else
      {
         // 8.5 - Fill in the values for the fixed nodes
         D( k, k ) = D( k+n, k+n ) = 1;

         // Not going to do the inner loop here for 2 reasons:-
         // 1. D(k,...) = kronecker Delta already
         // 2. It doesn't matter whether D(...,k) is kronecker Delta,
         //    because the gradient at this point is zero, so there
         //    will be no effect from this point, no need to decouple explicitly
      }
   }
}

void DaviesBook::accumulate_reparameterization( DaviesBook::Matrix& cumulative_reparameterization, const DaviesBook::Matrix& proposed_reparameterization, const DaviesBook::IndexType mx, const DaviesBook::IndexType my, const DaviesBook::MT h )
{
   // Reform the cumulative reparameterization into an image
   ImageType cumulative_image( mx, my, 3, 1 );

   for ( IndexType y = 0; y < my; ++y )
      for ( IndexType x = 0; x < mx; ++x )
      {
         IndexType j = xy_to_index( x, y, mx, my );

         // Take off the grid position, so as to avoid wrapping issues
         cumulative_image( x, y, 0, 0 ) = cumulative_reparameterization( j, 0 ) - x*h;
         cumulative_image( x, y, 1, 0 ) = cumulative_reparameterization( j, 1 ) - y*h;

         // The z-value is zero (we are really working in 2D, just leveraging the 3D function)
         cumulative_image( x, y, 2, 0 ) = 0;
      }

   // Now resample at each of the grid points
   MT a[2];
   MT b[3];

   for ( IndexType y = 0; y < my; ++y )
   {
      for ( IndexType x = 0; x < mx; ++x )
      {
         IndexType j = xy_to_index( x, y, mx, my );

         // Treat the grid spacing as unity
         a[0] = proposed_reparameterization( j, 0 );
         a[1] = proposed_reparameterization( j, 1 );

         bilinear_interpolation( cumulative_image, 1, a, b );

         // Update the cumulative_reparameterization matrix
         cumulative_reparameterization( j, 0 ) = b[0] + ( x + wrap( a[0] / h, 0, 1 ) ) * h;
         cumulative_reparameterization( j, 1 ) = b[1] + ( y + wrap( a[1] / h, 0, 1 ) ) * h;

         // b[2] is identically 0, and we don't need to do anything with it anyway
      }
   }
}

void DaviesBook::save_reparameterization( DaviesBook::Matrix& cumulative_reparameterization, 
                                          const DaviesBook::Matrix& grid_points, 
                                          DaviesBook::Matrix& displacement, ImageType& shape_image, 
                                          const ImageType& Original_Shape_Image_Stack, 
                                          const DaviesBook::FluidOptimizationParams& params )
{
   const IndexType n = params.mx * params.my;

   Matrix proposed_reparameterization( n, 2 );

   for ( IndexType j = 0; j < n; ++j )
   {
      proposed_reparameterization( j, 0 ) = grid_points( j, 0 ) - displacement( j, 0 );
      proposed_reparameterization( j, 1 ) = grid_points( j, 1 ) - displacement( j, 1 );
   }

   Matrix old_cum = cumulative_reparameterization;

   // 2.6.3 - Resample the shape image according to the accumulated reparameterization
   accumulate_reparameterization( cumulative_reparameterization, proposed_reparameterization, params.mx, params.my, params.h );

   /*
   // Check to see if the cumulative reparameterization differs from grid_points
   for ( IndexType j = 0; j < n; ++j )
   {
      std::cerr << proposed_reparameterization( j, 0 ) << "\t" << old_cum( j, 0 ) << "\t" << cumulative_reparameterization( j, 0 ) << "\t" << grid_points( j, 0 ) << std::endl;
   }
   
   PRINTVBL( ( cumulative_reparameterization - proposed_reparameterization ).absolute_value_max( ) );
   PAUSE;
   */

   // 2.6.3.2 - Resample the original shape image to create a new shape image, sampled 
   //           according to Cumulative_Reparameterization
   MT a[2], b[3];
   for ( IndexType j = 0; j < n; ++j )
   {
      // Get the position from the cumulative reparameterization
      a[0] = cumulative_reparameterization( j, 0 );
      a[1] = cumulative_reparameterization( j, 1 );

      bilinear_interpolation( Original_Shape_Image_Stack, params.h, a, b );

      // Calculate the x and y position
      const Coordinate coord = index_to_xy( j, params.mx, params.my );

      // Update the shape image
      shape_image( coord.x, coord.y, 0 ) = b[0];
      shape_image( coord.x, coord.y, 1 ) = b[1];
      shape_image( coord.x, coord.y, 2 ) = b[2];
   }

   // 2.6.4 - Set all the elements of the displacement vector of the i-th training example to
   //         zero
   displacement.fill( 0 );
}

void DaviesBook::calculate_eigensystem( const DaviesBook::Matrix& mat, DaviesBook::Vector& eigenvalues, DaviesBook::Matrix& eigenvectors )
{
   EigenSystem eig( mat );

   const IndexType ns = mat.rows();

   const bool flip = eig.get_eigenvalue( 0 ) < eig.get_eigenvalue( ns-1 );

   for ( IndexType a = 0; a < ns; ++a )
   {
      IndexType idx = ( flip ) ? ( (ns-1)-a ) : a;

      eigenvalues( a ) = eig.get_eigenvalue( idx );

      Vector eigv = eig.get_eigenvector( idx );

      for ( IndexType r = 0; r < ns; ++r )
         eigenvectors( r, a ) = eigv( r );
   }
}

void DaviesBook::allocate_vectors( DaviesBook::Vector*& vec, const DaviesBook::IndexType n, const DaviesBook::IndexType len )
{
   vec = new Vector[ n ];

   for ( IndexType i = 0; i < n; ++i )
      vec[i] = Vector( len );
}

void printf_matrix( const DaviesBook::Matrix& m )
{
   using namespace DaviesBook;

   for ( IndexType r = 0; r < m.rows(); ++r )
   {
      for ( IndexType c = 0; c < m.cols(); ++c )
      {
         printf( "%f\t", m(r,c) );
      }
      printf( "\n" );
   }
}

void DaviesBook::calculate_covariance( const DaviesBook::Matrix* shape_vector, const DaviesBook::FluidOptimizationParams params, DaviesBook::Matrix& Covariance )
{
   const IndexType n = shape_vector[0].rows( );

   // Calculate the mean shape
   Matrix mean_shape( n, 3 );
   calculate_mean_shape_vector( shape_vector, params.ns, mean_shape );

   // Calculate triangle areas on the mean shape
   Vector int_area( n );
   MT total_area = calculate_int_triangle_areas( mean_shape, params.mx, params.my, int_area );

   // Calculate the Covariance matrix
   for ( IndexType i = 0; i < params.ns; ++i )
   {
      for ( IndexType j = i; j < params.ns; ++j )
      {
         Covariance( i, j ) = 0;

         for ( IndexType k = 0; k < n; ++k )
         {
            MT increment = 0;

            for ( IndexType ax = 0; ax < 3; ++ax )
               increment += ( shape_vector[i]( k, ax ) - mean_shape( k, ax ) ) * ( shape_vector[j]( k, ax ) - mean_shape( k, ax ) );

            Covariance( i, j ) += int_area( k ) * increment;
         }

         Covariance( j, i ) = Covariance( i, j );
      }
   }

   Covariance /= total_area;
}

DaviesBook::MT DaviesBook::calculate_L( const DaviesBook::Vector& eigenvalues, DaviesBook::Vector& dL_by_de, const DaviesBook::FluidOptimizationParams& params )
{
   MT ret = 0;

   PRINTVBL( eigenvalues );
   for ( IndexType i = 0; i < eigenvalues.size( )-1; ++i )
   {
      if ( eigenvalues(i) < eigenvalues(i+1) )
         throw std::exception( "Eigenvalues are not in order!" );

      MT scaled_eig = eigenvalues( i ) / params.lambda_cut;

      // Thodberg's form
      if ( scaled_eig >= 1 )
      {
         ret += 1 + log( scaled_eig );
         dL_by_de( i ) = 1 / ( scaled_eig ); // No need for twiddle, it's never less than 1!
      }
      else
      {
         ret += scaled_eig;
         dL_by_de( i ) = params.lambda_cut;
      }

      // Basic form
      //ret += log( scaled_eig );
      //dL_by_de( i ) = 1 / ( scaled_eig + params.eps );
   }

   return ret;
}

void DaviesBook::optimise_fluid( ImageType& shape_image_stack, const CImg<bool>& free_node_image, const FluidOptimizationParams& params )
{
   // START OF INITIALIZATION

   // 1. Let ref be the index of the reference shape
   //    - This is being passed as a parameter

   // 2. Initialize the regrid counter to zero for all shapes
   //    - I'm not tracking this, I'm just storing the existing cumulative reparameterization
   
   // 3. Make a copy of the original shape images
   const ImageType Original_Shape_Image_Stack = shape_image_stack;

   // 4. Concatenate the coordinates of the mxm regular grid into a 2n-dimensional vector (hence n = params.mx*params.my )
   const IndexType n = params.mx*params.my;
   Matrix grid_points( n, 2 );

   for ( IndexType j = 0; j < params.my; ++j )
   {
      for ( IndexType i = 0; i < params.mx; ++i )
      {
         grid_points( xy_to_index( i, j, params.mx, params.my ), 0 ) = i * params.h;
         grid_points( xy_to_index( i, j, params.mx, params.my ), 1 ) = j * params.h;
      }
   }

   // I am also going to create the cumulative reparameterization here...
   Matrix* cumulative_reparameterization = new Matrix[ params.ns ];
   for ( IndexType i = 0; i < params.ns; ++i )
   {
      // Copy the grid points into the reparameterization
      cumulative_reparameterization[i] = Matrix( n, 2 );
      
      for ( IndexType j = 0; j < n; ++j )
      {
         cumulative_reparameterization[i]( j, 0 ) = grid_points( j, 0 );
         cumulative_reparameterization[i]( j, 1 ) = grid_points( j, 1 );
      }
   }

   // 5. Let displacement_i be a 2n-dimensional vector representing the displacemnt field of the i-th
   //    shape - the vector is arranged so that each component of displacement corresponds
   //    to a component of grid_points; each element of each vector is initialized to zero
   // 6. Create a 3n-dimensional shape vector for each training example

   //    - Let's do 5 and 6 together

   Matrix* displacement = new Matrix[ params.ns ];
   Matrix* shape_vector = new Matrix[ params.ns ];

   for ( IndexType i = 0; i < params.ns; ++i )
   {
      displacement[i] = Matrix( n, 2 );
      displacement[i].fill( 0 );
   
      shape_vector[i] = Matrix( n, 3 );
      
      // Call the function which calculates this...
      // (of course, this is just reforming the grid, since displacement == 0)
      populate_shape_vector( shape_image_stack.get_shared_channel( i ), params.h, displacement[i], shape_vector[i] );
   }

      
   // 7. Let free_node be a n-dimensional boolean valued vector...
   //    - Well, I am passing this as a parameter, but as an image.  It's not
   //      really necessary, but let's convert this into an array
   bool* free_node = new bool[ n ];
   bool  all_free  = true;
   for ( IndexType j = 0; j < params.my; ++j )
   {
      for ( IndexType i = 0; i < params.mx; ++i )
      {
         IndexType k = xy_to_index( i, j, params.mx, params.my );
         free_node[ k ] = free_node_image( i, j );

         if ( !free_node[k] )
            all_free = false;
      }
   }

   if ( all_free )
   {
      DEBUGMACRO( "All nodes are free; fixed node will be moved around!" );
   }

   // Right, if all of the nodes are free, then we have a problem, because D will be
   // rank deficient.  Instead, the free node will be moved around every time the
   // D matrix is rebuilt

   // END OF INITIALIZATION

   DEBUGLINE;
   //write_shape_vectors( shape_vector, params.ns, n );

   // START OF OPTIMIZATION
   std::cerr << "##### START OF OPTIMIZATION" << std::endl;
   std::cerr << "! Not doing rigid alignment!" << std::endl;

   // Define these so that we don't have to do them each iteration
   Vector* gradient = new Vector[params.ns];
   for ( IndexType i = 0; i < params.ns; ++i )
      gradient[i] = Vector( 2*n );

   Matrix* centred_shape_vector = new Matrix[params.ns];
   for ( IndexType i = 0; i < params.ns; ++i )
      centred_shape_vector[i] = Matrix( n, 3 );

   Matrix eigenvector( params.ns, params.ns );
   Vector eigenvalue( params.ns );

   Matrix mean_shape_vector( n, 3 );

   SparseMatrix D( 2*n, 2*n );
   SparseLU* lu_decomp = 0;

   IndexType fixed_node_for_build = 0;

   std::ofstream lfs( "Ls_data.m" );
   lfs << "Ls = [" << std::endl;

   MT total_t = 0;

   Vector int_area( n );

   bool skip_regrid_warning_issued = false;

   // 1 - For each iteration...
   for ( IndexType it = 0; it <= params.niterations; ++it )
   {
      std::cerr << "Starting iteration " << it << " (t=" << total_t << ")" << std::endl;

      // Stage I8 is taken from the initialization, but given that it depends on {shape_vector}, this has
      // to be calculated each time

      // I8. Precompute the 2n x 2n (sparse) matrix, D, which holds the finite difference operations
      //    required to construct the set of second derivatives
      
      // I8.3 Create a 3n-dimensional vector to store the shape function and set it to the mean shape
      //     - Instead of mean_shape_vector, I'm going to call this mean_shape_vector, it seems a little 
      //       more sensible, a vector of this name is used for the same purpose later too
      //std::cerr << "  Calculating the mean shape vector" << std::endl;
      calculate_mean_shape_vector( shape_vector, params.ns, mean_shape_vector );
      //std::cerr << "  Done." << std::endl;

      const bool rebuild_d = it % params.iterations_between_D_rebuild == 0;

      if ( rebuild_d )
      {
         if ( lu_decomp != 0 )
            delete lu_decomp;

         if ( all_free )
         {
            // Free the current fixed node
            free_node[ fixed_node_for_build ] = true;

            // Fix a different node
            free_node[ fixed_node_for_build = rand() % n ] = false;

            DEBUGMACRO( "Fixing node " << fixed_node_for_build );
         }

         // call to calculate the matrix of derivatives
         std::cerr << "  Calculating the derivative matrix" << std::endl;
         calculate_D( D, mean_shape_vector, free_node, params );
         std::cerr << "  Done." << std::endl;

         lu_decomp = new SparseLU( D );
      }

      // 8.6 - Perform an LU decomposition of D to get a lower triangular matrix L and an upper
      //       triangular matrix U
      //       - Let VNL handle the LU decomposition - doesn't explicitly do the decomp until first solve
      //SparseLU lu_decomp( D );

      // 1.1 - Select a shape number, i, at random using a uniform distribution over the training
      //       set with i != ref
      //       - I don't know why you are asked to do this, this i doesn't seem to be used again
      //         (or at least overwritten/redefined)
   
      // 1.2 - Let gradient_i be a 2n-dimensional vector that holds the value of the gradient of the
      //       objective function, L, w.r.t. the displacement of the sample points of the i-th shape
      //       - for convenience, we will hold the position of the sample points in a 2n-dimensional
      //         vector, p, where p_A[i] = grid_points(A) - displacement_i(A)
      //       - I allocated these earlier

      // * Set the value of the gradient of the reference shape to zero:
      // * Set the value at all fixed nodes to be zero
      //   - If we're going to be calculating gradient on each shape anyhow, why not just fill
      //     all the vectors with zeros?
      for ( IndexType i = 0; i < params.ns; ++i )
         gradient[i].fill( 0 );

      // 1.3 - The components of each other term are calculated as follows:

      // 1.3.1 - Calculate the mean shape vector
      //         - No need to do this again, did it to calculate D
      //calculate_mean_shape_vector( shape_vector, params.ns, mean_shape_vector );

      // 1.3.2 - Create a shape difference vector for each shape by subtracting the mean shape vector:
      //         - No, I'm going to use this implicitly whenever needed
      for ( IndexType i = 0; i < params.ns; ++i )
         centred_shape_vector[i] = shape_vector[i] - mean_shape_vector;

      // 1.3.3 - For each sample point: calculate the sum of the areas (on the mean shape) of
      //         all triangle connected to that sample point and store it in a n-dimensional
      //         vector int_area
      //std::cerr << "  Calculating triangle area integrals" << std::endl;
      MT total_area = calculate_int_triangle_areas( mean_shape_vector, params.mx, params.my, int_area );
      //PRINTVBL( total_area );
      //std::cerr << "  Done." << std::endl;

      //{
      //   std::ofstream fs( "shape_image.m" );
      //   fs << "clear; clc; close all;" << std::endl;

      //   for ( IndexType r = 0; r < params.my; ++r )
      //      for ( IndexType c = 0; c < params.mx; ++c )
      //      {
      //         IndexType j = xy_to_index( c, r, params.mx, params.my );

      //         fs << "mean_shape( " << (r+1) << ", " << (c+1) << ", 1) = " << mean_shape_vector( j, 0 ) << ";" << std::endl;
      //         fs << "mean_shape( " << (r+1) << ", " << (c+1) << ", 2) = " << mean_shape_vector( j, 1 ) << ";" << std::endl;
      //         fs << "mean_shape( " << (r+1) << ", " << (c+1) << ", 3) = " << mean_shape_vector( j, 2 ) << ";" << std::endl;

      //         fs << "int_area( " << (r+1) << ", " << (c+1) << " ) = " << int_area( j ) << ";" << std::endl;
      //      }

      //   fs.close( );
      //   PAUSE;
      //}

      // 1.3.4 - Calculate a params.ns x params.ns covariance matrix, Covariance, using the approximation
      //         described in Sect 7.3.1
      Matrix Covariance( params.ns, params.ns );
      //std::cerr << "  Calculating Covariance Matrix" << std::endl;
      calculate_covariance( shape_vector, params, Covariance );
      //std::cerr << "  Done." << std::endl;

      //printf_matrix( Covariance );

      // 1.3.5 - obtain the params.ns-1 set of eigenvectors {eigenvector_a} and the corresponding
      //         (ordered) eigenvalues {eigenvalue_a} of Covariance
      //         - This is handled by VNL
      //std::cerr << "  Calculating Eigendecomposition" << std::endl;
      calculate_eigensystem( Covariance, eigenvalue, eigenvector );
      //std::cerr << "  Done." << std::endl;

      Vector dL_by_d_lam_a( params.ns-1 );

      MT currentL = calculate_L( eigenvalue, dL_by_d_lam_a, params );
      
      lfs << it << " " << total_t << " " << currentL << " " << ( rebuild_d ? "1" : "0" );
      for ( IndexType i = 0; i < params.ns-1; ++i )
         lfs << " " << eigenvalue( i );
      lfs << std::endl;

      if ( it >= params.niterations )
         break;

      PRINTVBL( currentL );

      /*
      // This is part of the gradient calculation
      Vector dL_by_d_lam_a( params.ns-1 );

      for ( IndexType a = 0; a < params.ns-1; ++a )
      {
         // 1.3.6 - Use the eigenvalues to calculate dL_by_d_lam_a
         dL_by_d_lam_a(a) = 1 / ( eigenvalue(a) + params.eps );

         // 1.3.7 - Normalize all eigenvectors to have unit length
         //         - They're already normalized
      }
      */

      // Loop over each shape
      for ( IndexType i = 0; i < params.ns; ++i )
      {
         // No need to do anything for the reference shape
         if ( i != params.ref )
         {
            //std::cerr << "    Calculating gradient for shape " << i << std::endl;

            // Loop over which point is being moved
            for ( IndexType A = 0; A < n; ++A )
            {
               // I think that the sum over m is unnecessary if you are using the grid points
               // as the sample points: there is only one point in the shape function which is 
               // going to be affected by the change in p_A

               // Make sure the gradient is zero
               gradient[i]( A + 0*n ) = 0;
               gradient[i]( A + 1*n ) = 0;

               if ( free_node[A] )
               {

                  // I need to check whether this gradient is (approximately) correct
                  //DEBUGMACRO( "=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=" );
                  //#include "IterationDebug.h"

                  Coordinate coord = index_to_xy( A, params.mx, params.my );

                  // Now we need to evaluate the expression on the RHS

                  // 1.3.10 - Use a finite difference scheme to approximate numerically the x-coordinate
                  //          components of dS_i(x)/dp_A[i] for each free grid point

                  // This is the numerical integration.  A couple of things to note:-
                  // - The only grid point at which the shape is affected by p_A is where m=A
                  // - This doesn't depend upon a, j or k, and so it can be precomputed
                  MT perturb_xp[] = { grid_points( A, 0 ), grid_points( A, 1 ) };
                  MT perturb_xn[] = { grid_points( A, 0 ), grid_points( A, 1 ) };
                  MT perturb_yp[] = { grid_points( A, 0 ), grid_points( A, 1 ) };
                  MT perturb_yn[] = { grid_points( A, 0 ), grid_points( A, 1 ) };

                  /*
                  // Actually sample at the current position
                  perturb_xp[0] = grid_points( A, 0 ) - displacement[i]( A, 0 ) + params.Delta;
                  perturb_xp[1] = grid_points( A, 1 ) - displacement[i]( A, 1 );

                  perturb_xn[0] = grid_points( A, 0 ) - displacement[i]( A, 0 ) - params.Delta;
                  perturb_xn[1] = grid_points( A, 1 ) - displacement[i]( A, 1 );

                  perturb_yp[0] = grid_points( A, 0 ) - displacement[i]( A, 0 );
                  perturb_yp[1] = grid_points( A, 1 ) - displacement[i]( A, 1 ) + params.Delta;

                  perturb_yn[0] = grid_points( A, 0 ) - displacement[i]( A, 0 );
                  perturb_yn[1] = grid_points( A, 1 ) - displacement[i]( A, 1 ) - params.Delta;
                  */
                  
                  if ( displacement[i]( A, 0 ) < 0 )
                     perturb_xp[0] += params.Delta;
                  else
                     perturb_xn[0] -= params.Delta;

                  if ( displacement[i]( A, 1 ) < 0 )
                     perturb_yp[1] += params.Delta;
                  else
                     perturb_yn[1] -= params.Delta;
                  
                  /*
                  // Central differencing
                  perturb_xp[0] += params.Delta;
                  perturb_xn[0] -= params.Delta;

                  perturb_yp[1] += params.Delta;
                  perturb_yn[1] -= params.Delta;
                  */

                  MT perturb_xp_shape[3], perturb_yp_shape[3];
                  MT perturb_xn_shape[3], perturb_yn_shape[3];

                  bilinear_interpolation( shape_image_stack.get_shared_channel( i ), params.h, perturb_xp, perturb_xp_shape );
                  bilinear_interpolation( shape_image_stack.get_shared_channel( i ), params.h, perturb_xn, perturb_xn_shape );

                  bilinear_interpolation( shape_image_stack.get_shared_channel( i ), params.h, perturb_yp, perturb_yp_shape );
                  bilinear_interpolation( shape_image_stack.get_shared_channel( i ), params.h, perturb_yn, perturb_yn_shape );

                  MT d_Si_by_d_pA[2][3];

                  for ( IndexType ax = 0; ax < 3; ++ax )
                  {
                     // This denominator ensures that whatever selection of +/- params.Delta the perturbed positions are,
                     // the gradient approximation is correct
                     d_Si_by_d_pA[0][ax] = -( perturb_xp_shape[ax] - perturb_xn_shape[ax] ) / ( perturb_xp[0] - perturb_xn[0] );
                     d_Si_by_d_pA[1][ax] = -( perturb_yp_shape[ax] - perturb_yn_shape[ax] ) / ( perturb_yp[1] - perturb_yn[1] );
                  }

                  // For testing later
                  Vector d_lam_a_by_d_p_a_X( params.ns - 1 );
                  Vector d_lam_a_by_d_p_a_Y( params.ns - 1 );
                  d_lam_a_by_d_p_a_X.fill( 0 );
                  d_lam_a_by_d_p_a_Y.fill( 0 );

                  // For each of the eigenvalues
                  for ( IndexType a = 0; a < params.ns-1; ++a )
                  {
                     Matrix d_D_by_d_pA[2];
                     
                     d_D_by_d_pA[0] = Matrix( params.ns, params.ns );
                     d_D_by_d_pA[1] = Matrix( params.ns, params.ns );

                     // For each pair of grid points
                     for ( IndexType j = 0; j < params.ns; ++j )
                     {
                        for ( IndexType k = 0; k < params.ns; ++k )
                        {
                           // 1.3.8 - Use the eigenvectors to calculate
                           MT d_lam_a_by_d_D_jk = eigenvector( j, a ) * eigenvector( k, a );

                           // Wrap up the division by ns here...
                           MT k_mul = kronecker( i, j ) - static_cast<MT>( 1 ) / params.ns;
                           MT j_mul = kronecker( i, k ) - static_cast<MT>( 1 ) / params.ns;

                           // 1.3.9 - Calculate the components of ...
                           MT d_D_jk_by_d_S_i[3];
                           for ( IndexType ax = 0; ax < 3; ++ax )
                           {
                              d_D_jk_by_d_S_i[ax] = (   
                                                        j_mul * centred_shape_vector[j]( A, ax )
                                                      + k_mul * centred_shape_vector[k]( A, ax )
                                                    ) / total_area;
                           }

                           MT d_Djk_by_d_pA[] = {
                              dot( d_D_jk_by_d_S_i, d_Si_by_d_pA[0], 3 ) * int_area( A ),
                              dot( d_D_jk_by_d_S_i, d_Si_by_d_pA[1], 3 ) * int_area( A )
                           };

                           // SAVE FOR DEBUGGING
                           d_D_by_d_pA[0]( j, k ) = d_Djk_by_d_pA[0];
                           d_D_by_d_pA[1]( j, k ) = d_Djk_by_d_pA[1];
                           
                           // This is the increment to the gradient
                           MT dLa_by_d_pA[] = {
                              dL_by_d_lam_a(a) * d_lam_a_by_d_D_jk * d_Djk_by_d_pA[0],
                              dL_by_d_lam_a(a) * d_lam_a_by_d_D_jk * d_Djk_by_d_pA[1]
                           };

                           // Actually increment the gradient
                           gradient[i]( A + 0*n ) += dLa_by_d_pA[0];
                           gradient[i]( A + 1*n ) += dLa_by_d_pA[1];

                           // For testing later
                           d_lam_a_by_d_p_a_X( a ) += d_lam_a_by_d_D_jk * d_Djk_by_d_pA[0];
                           d_lam_a_by_d_p_a_Y( a ) += d_lam_a_by_d_D_jk * d_Djk_by_d_pA[1];
                        }
                     }
                  }
               }

            }

            //std::cerr << "    Done." << std::endl;
         }
      }

      /*
      std::ofstream fs( "grads.m" );
      fs << "close all; clear; clc;" << std::endl;
      for ( IndexType i = 0; i < params.ns; ++i )
      {
         fs << " shapes{" << (i+1) << "} = [" << std::endl;

         for ( IndexType j = 0; j < n; ++j )
         {
            fs << grid_points( j, 0 ) << " " << grid_points( j, 1 ) << " " << gradient[i]( 0*n + j ) << " " << gradient[i]( 1*n + j ) << std::endl;
         }

         fs << "]';" << std::endl;
      }
      fs.close( );
      DEBUGMACRO( "Written gradients to grads.m" );
      PAUSE;
      */

      //std::cerr << "  Calculating deformation updates" << std::endl;
      // 2 - For each non-reference shape
      for ( IndexType i = 0; i < params.ns; ++i )
      {
         if ( i != params.ref )
         {
            //std::cerr << "    Solving for velocity for shape " << i << std::endl;
            // 2.1 - Solve the linear PDE
            // -gradient_i = D velocity

            //PRINTVBL( gradient[i].magnitude( ) );
            Vector velocity = lu_decomp->solve( -gradient[i] );

            // Look at the reciprocal condition number
            //PRINTVBL( lu_decomp.rcond( ) );
            
            //std::cerr << "    Done." << std::endl;
            
            // 2.2 - Calculate the n-dimensional vectors corresponding tothe gradient of the displacement
            //       field of the i-th shape, displacement, along the x and y axes:
            // 2.3 - Calculate the n x 2 regularization matrix R

            Matrix R( n, 2 );
            MT max_norm_sq = 0;

            MT duxdx;
            MT duxdy;
            MT duydx;
            MT duydy;

            //std::cerr << "    Calculating regularized update for shape " << i << std::endl;
            
            for ( IndexType j = 0; j < n; ++j )
            {
               if ( free_node[j] )
               {
                  IndexType nbs[9];
                  neighbours( j, params.mx, params.my, nbs );

                  duxdx = ( displacement[i]( nbs[NB_1_S], 0 ) - displacement[i]( nbs[NB_1_N], 0 ) ) / ( 2*params.h );
                  duxdy = ( displacement[i]( nbs[NB_1_E], 0 ) - displacement[i]( nbs[NB_1_W], 0 ) ) / ( 2*params.h );

                  duydx = ( displacement[i]( nbs[NB_1_S], 1 ) - displacement[i]( nbs[NB_1_N], 1 ) ) / ( 2*params.h );
                  duydy = ( displacement[i]( nbs[NB_1_E], 1 ) - displacement[i]( nbs[NB_1_W], 1 ) ) / ( 2*params.h );
               }
               else
               {
                  duxdx = 0;
                  duxdy = 0;
                  duydx = 0;
                  duydy = 0;
               }
            
               R( j, 0 ) = velocity( j + 0*n ) - ( velocity( j + 0*n ) * duxdx + velocity( j + 1*n ) * duxdy );
               R( j, 1 ) = velocity( j + 1*n ) - ( velocity( j + 0*n ) * duydx + velocity( j + 1*n ) * duydy );

               MT this_norm_sq = R( j, 0 )*R( j, 0 ) + R( j, 1 )*R( j, 1 );

               if ( this_norm_sq > max_norm_sq )
                  max_norm_sq = this_norm_sq;
            }
            //std::cerr << "    Done." << std::endl;
            
            // 2.4 - Calculate the timestep as Delta / max( norm( R ) )
            MT dt = params.timestep_mult /*params.timestep_mult*/ / sqrt( max_norm_sq );

            // std::cerr << "    Time step = " << dt << " for shape " << i << std::endl;

            // 2.5 - Calculate the proposed displacement
            //       - Let's call this proposed_reparameterization instead, since that's what it is
            Matrix proposed_reparameterization( n, 2 );
            for ( IndexType j = 0; j < n; ++j )
            {
               proposed_reparameterization( j, 0 ) = grid_points( j, 0 ) - displacement[i]( j, 0 ) - dt * R( j, 0 );
               proposed_reparameterization( j, 1 ) = grid_points( j, 1 ) - displacement[i]( j, 1 ) - dt * R( j, 1 );
            }

            //{
            //   std::ofstream fs( "prop_repar.m" );
            //   fs << "clear; clf; clc; gr = [";
            //   for ( IndexType j = 0; j < n; ++j )
            //   {
            //      fs << grid_points( j, 0 ) << " " << grid_points( j, 1 ) << std::endl;
            //   }
            //   fs << "]'; scatter2x( gr, 'b+' ); hold on; axis equal; " << std::endl;
            //   fs << "pr = [" << proposed_reparameterization << "]'; quiver( gr(1,:), gr(2,:), pr(1,:)-gr(1,:), pr(2,:)-gr(2,:) );";
            //   fs.close( );
            //   PAUSE;
            //}
            
            // Calculate the Jacobian of m . proposed_reparameterization
            IndexType nbs[9];

            bool need_to_regrid = false && rebuild_d;
            IndexType min_m = ( params.mx < params.my ) ? params.mx : params.my;

            const MT wrap_lower = -0.5 * params.h * params.mx;
            const MT wrap_upper =  0.5 * params.h * params.mx;

            //std::cerr << "    Checking if need to regrid shape " << i << std::endl;
            for ( IndexType j = 0; j < n && !need_to_regrid; ++j )
            {
               if ( !skip_regrid_warning_issued )
               {
                  std::cerr << "      SKIPPING REGRIDDING CHECK" << std::endl;
                  skip_regrid_warning_issued = true;
               }
               break;

               // Get the neighbouring indices
               neighbours( j, params.mx, params.my, nbs );

               // Take off the current grid point
               // Add on the centre grid point
               // Subtract/Add on one grid square

               const MT off_x = grid_points( nbs[NB_1_C], 0 );
               MT n_x = proposed_reparameterization( nbs[NB_1_N], 0 ) - grid_points( nbs[NB_1_N], 0 ) + off_x;
               MT s_x = proposed_reparameterization( nbs[NB_1_S], 0 ) - grid_points( nbs[NB_1_S], 0 ) + off_x;
               MT w_x = proposed_reparameterization( nbs[NB_1_W], 0 ) - grid_points( nbs[NB_1_W], 0 ) + off_x - params.h;
               MT e_x = proposed_reparameterization( nbs[NB_1_E], 0 ) - grid_points( nbs[NB_1_E], 0 ) + off_x + params.h;

               const MT off_y = grid_points( nbs[NB_1_C], 1 );
               MT n_y = proposed_reparameterization( nbs[NB_1_N], 1 ) - grid_points( nbs[NB_1_N], 1 ) + off_y - params.h;
               MT s_y = proposed_reparameterization( nbs[NB_1_S], 1 ) - grid_points( nbs[NB_1_S], 1 ) + off_y + params.h;
               MT w_y = proposed_reparameterization( nbs[NB_1_W], 1 ) - grid_points( nbs[NB_1_W], 1 ) + off_y;
               MT e_y = proposed_reparameterization( nbs[NB_1_E], 1 ) - grid_points( nbs[NB_1_E], 1 ) + off_y;

               /*
               MT n_x = wrap( proposed_reparameterization( nbs[NB_1_N], 0 ), wrap_lower, wrap_upper );
               MT s_x = wrap( proposed_reparameterization( nbs[NB_1_S], 0 ), wrap_lower, wrap_upper );
               MT e_x = wrap( proposed_reparameterization( nbs[NB_1_E], 0 ), wrap_lower, wrap_upper );
               MT w_x = wrap( proposed_reparameterization( nbs[NB_1_W], 0 ), wrap_lower, wrap_upper );
               
               MT n_y = wrap( proposed_reparameterization( nbs[NB_1_N], 1 ), wrap_lower, wrap_upper );
               MT s_y = wrap( proposed_reparameterization( nbs[NB_1_S], 1 ), wrap_lower, wrap_upper );
               MT e_y = wrap( proposed_reparameterization( nbs[NB_1_E], 1 ), wrap_lower, wrap_upper );
               MT w_y = wrap( proposed_reparameterization( nbs[NB_1_W], 1 ), wrap_lower, wrap_upper );
               */

               // J = det( [ dPx/dx dPx/dy; dPy/dx dPy/dy ] )
               MT dPx_x = ( e_x - w_x ) / ( 2*params.h );
               MT dPx_y = ( s_x - n_x ) / ( 2*params.h );

               MT dPy_x = ( e_y - w_y ) / ( 2*params.h );
               MT dPy_y = ( s_y - n_y ) / ( 2*params.h );

               //DEBUGMACRO( dPx_x << "\t" << dPx_y << "\t" << dPy_x << "\t" << dPy_y );

               MT det_J = dPx_x * dPy_y - dPx_y * dPy_x;

               // We are only looking to see if m . det_J < 0.5 at any point, so we can fast break if it is
               need_to_regrid = ( min_m * det_J ) < 0.5;

               /*
               if ( need_to_regrid )
               {
                  PRINTVBL( proposed_reparameterization( nbs[NB_1_N], 0 ) );
                  PRINTVBL( proposed_reparameterization( nbs[NB_1_S], 0 ) );
                  PRINTVBL( proposed_reparameterization( nbs[NB_1_W], 0 ) );
                  PRINTVBL( proposed_reparameterization( nbs[NB_1_E], 0 ) );
                  BLANKLINE;

                  PRINTVBL( n_x );
                  PRINTVBL( s_x );
                  PRINTVBL( w_x );
                  PRINTVBL( e_x );
                  BLANKLINE;

                  PRINTVBL( proposed_reparameterization( nbs[NB_1_N], 1 ) );
                  PRINTVBL( proposed_reparameterization( nbs[NB_1_S], 1 ) );
                  PRINTVBL( proposed_reparameterization( nbs[NB_1_W], 1 ) );
                  PRINTVBL( proposed_reparameterization( nbs[NB_1_E], 1 ) );
                  BLANKLINE;

                  PRINTVBL( n_y );
                  PRINTVBL( s_y );
                  PRINTVBL( w_y );
                  PRINTVBL( e_y );
                  BLANKLINE;

                  PRINTVBL( dPx_x );
                  PRINTVBL( dPx_y );
                  PRINTVBL( dPy_x );
                  PRINTVBL( dPy_y );
                  BLANKLINE;

                  PRINTVBL( det_J );

                  PAUSE;
               }
               */
            }

            //PAUSE;

            //std::cerr << "    Done." << std::endl;

            // 2.6 - If the Jacobian of m . proposed_displacement is less than 0.5 at any point
            if ( need_to_regrid )
            {
               std::cerr << "    Regridding shape " << i << std::endl;
               //PAUSE;

               save_reparameterization( cumulative_reparameterization[i], grid_points, displacement[i], 
                  shape_image_stack.get_shared_channel( i ), Original_Shape_Image_Stack.get_shared_channel( i ), params );

               std::cerr << "    Done." << std::endl;
            }
            else
            {
               //std::cerr << "    Updating displacement field for shape " << i << std::endl;
               // Accept the displacement
               for ( IndexType j = 0; j < n; ++j )
               {
                  displacement[i]( j, 0 ) += dt * R( j, 0 );
                  displacement[i]( j, 1 ) += dt * R( j, 1 );
               }
               //std::cerr << "    Done." << std::endl;

               // Increment the total time
               total_t += dt;
            }

            // 3 - Update the i-th shape vector
            //std::cerr << "    Updating shape vector " << i << std::endl;
            populate_shape_vector( shape_image_stack.get_shared_channel( i ), params.h, displacement[i], shape_vector[i] );
            //std::cerr << "    Done." << std::endl;
         }
      }
      //std::cerr << "  Done." << std::endl;
      //std::cerr << std::endl;

      // Calculate the RMS between shape images
      //for ( IndexType i = 0; i < params.ns; ++i )
      //{
      //   MT rms = 0;

      //   for ( IndexType j = 0; j < n; ++j )
      //   {
      //      for ( IndexType ax = 0; ax < 3; ++ax )
      //         rms += ( shape_vector[i]( j, ax ) - shape_vector[params.ref]( j, ax ) ) * ( shape_vector[i]( j, ax ) - shape_vector[params.ref]( j, ax ) );
      //   }

      //   rms = sqrt( rms / n );
      //   DEBUGMACRO( "RMS(ref," << i << ") = " << rms );
      //}
      ////write_shape_vectors( shape_vector, params.ns, n );
   }
   lfs << "];" << std::endl;
   lfs.close( );

   std::cerr << "##### END OF OPTIMIZATION" << std::endl;

   // END OF OPTIMIZATION

   std::cerr << "##### START OF POSTPROCESSING" << std::endl;
   // POST PROCESSING
   for ( IndexType i = 0; i < params.ns; ++i )
   {
      // 1 - Save the final reparameterization
      // 2 - Calculate the accumulated reparameterization
      // 3 - Let Reparameterization[i] = cumulative_reparameterization[i]

      std::cerr << "    Calculating final reparameterization for shape " << i << std::endl;

      save_reparameterization( cumulative_reparameterization[i], grid_points, displacement[i], 
                  shape_image_stack.get_shared_channel( i ), Original_Shape_Image_Stack.get_shared_channel( i ), params );

      // I am passing back the reparameterization using the shape images
      std::cerr << "    Done." << std::endl;
   }

   std::cerr << "##### END OF POSTPROCESSING" << std::endl;

   write_shape_vectors( shape_vector, params.ns, n, false );

   // Tidy up
   delete [] cumulative_reparameterization;
   delete [] centred_shape_vector;
   delete [] gradient;
   delete [] free_node;
   delete [] shape_vector;
   delete [] displacement;

   if ( lu_decomp != 0 )
      delete lu_decomp;
}

void DaviesBook::write_shape_vectors( const DaviesBook::Matrix* shape_vector, const DaviesBook::IndexType ns, const DaviesBook::IndexType n, const bool pause )
{
   std::ofstream fs( "shapes_script.m" );
   fs << "clear; clc" << std::endl;

   for ( IndexType i = 0; i < ns; ++i )
   {
      fs << "shapes{" << (i+1) << "} = [";

      for ( IndexType j = 0; j < n; ++j )
      {
         fs << shape_vector[i]( j, 0 ) << " " << shape_vector[i]( j, 1 ) << " " << shape_vector[i]( j, 2 ) << std::endl;
      }

      fs << "]';" << std::endl;
   }

   fs << "clf; hold on;" << std::endl;

   for ( IndexType i = 0; i < ns; ++i )
   {
      //fs << "scatter3x( shapes{" << (i+1) << "} ); axis equal tight" << std::endl;
      fs << "plot(1:1024,shapes{" << (i+1) << "}(1,:), 1:1024,shapes{" << (i+1) << "}(2,:), 1:1024,shapes{" << (i+1) << "}(3,:))" << std::endl;
   }
   fs << "disp(sqrt( sum( sum( (shapes{1}-shapes{2}).^2 ) ) / " << n << " ));" << std::endl;

   fs.close( );

   if ( pause ) PAUSE;
}

#undef NB_1_NW
#undef NB_1_N
#undef NB_1_NE

#undef NB_1_W
#undef NB_1_C
#undef NB_1_E

#undef NB_1_SW
#undef NB_1_S 
#undef NB_1_SE

#undef NB_2_NW
#undef NB_2_N
#undef NB_2_NE

#undef NB_2_W
#undef NB_2_C
#undef NB_2_E

#undef NB_2_SW
#undef NB_2_S 
#undef NB_2_SE