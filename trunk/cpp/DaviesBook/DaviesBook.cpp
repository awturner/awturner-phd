#include "Chapter7.h"

#include <iostream>
#include <fstream>
#include "Useful/PrintMacros.h"
#include "Useful/Noise.h"

using namespace cimg_library;
using namespace DaviesBook;
using namespace AWT;

const MT deg_to_rad( const MT deg )
{
   return deg * ( 4.0 * atan( 1.0 ) ) / 180.0;
}

int main( int argc, char** argv )
{
   Noise<MT>::timeSeed( );

   const IndexType divs = 32;

   ImageType shape_image( divs, divs, 3, 3 );

   const MT R = 40;
   const MT r = 10;

   const MT noise_scale = 0;

   const MT ang_step = 2 * ( 4 * atan( 1.0 ) ) / divs;

   // Create two torii, which are offset by an amount in one angular direction
   MT pos0[3];
   
   for ( IndexType k = 0; k < shape_image.dimv(); ++k )
   {
      MT u_off = AWT::Noise<MT>::randu( -0.5, 0.5 ) * ang_step;

      for ( IndexType j = 0; j < divs; ++j )
      {
         for ( IndexType i = 0; i < divs; ++i )
         {
            MT u = i * ang_step + u_off;
            MT v = j * ang_step;

            pos0[0] = ( R + r * cos( v ) ) * cos( u );
            pos0[1] = ( R + r * cos( v ) ) * sin( u );
            pos0[2] = r * sin( v );

            shape_image( i, j, 0, k ) = pos0[0]; // + noise_scale*Noise<MT>::randu( -1, 1 );
            shape_image( i, j, 1, k ) = pos0[1]; // + noise_scale*Noise<MT>::randu( -1, 1 );
            shape_image( i, j, 2, k ) = pos0[2]; // + noise_scale*Noise<MT>::randu( -1, 1 );
         }
      }
   }

   // All nodes are free to move!
   CImg<bool> free_node_image( divs, divs );
   free_node_image.fill( true );

   FluidOptimizationParams params;

   params.h = 1;      // Grid spacing

   params.visc_mu     = 1; // Viscosity Parameter
   params.visc_lambda = 1; // Viscosity Parameter

   params.eps = 1e-8;        // Avoid singularity in the reciprocal eigenvalues
   params.Delta = 1e-6;      // How much to move by in the numerical estimation of gradient
   params.timestep_mult = 1; // Time step size

   params.lambda_cut = 0.1; // Thodberg's cut-off lambda

   params.niterations = 4000; // How many iterations of optimization

   params.ref = 0; // Which is the reference shape image

   params.mx = shape_image.dimx();
   params.my = shape_image.dimy();

   params.ns = shape_image.dimv();

   params.iterations_between_D_rebuild = 100;

   params.regularize = true;

   try
   {
      optimise_fluid( shape_image, free_node_image, params );
   }
   catch ( std::exception ex )
   {
      std::cerr << "EXCEPTION CAUGHT" << std::endl;
      std::cerr << ex.what( ) << std::endl;
   }
}