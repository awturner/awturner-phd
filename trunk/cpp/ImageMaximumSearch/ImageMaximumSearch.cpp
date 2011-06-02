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
#include "ImageMaximumTree.h"
#include "MaximumTreeSearch.h"
#include "WeightFunction.h"

#include "CImg.h"

#include <iostream>

using namespace AWT;
using namespace cimg_library;

double dot( double* a, double* b, unsigned int n )
{
   double ret = 0;
   for ( int i = 0; i < n; ++i )
      ret += a[i]*b[i];

   return ret;
}

double norm2( double* a, unsigned int n )
{
   return sqrt( dot( a, a, n ) );
}

double norm1( double* a, unsigned int n )
{
   double ret = 0;

   for ( int i = 0; i < n; ++i )
      ret += abs( a[i] );

   return ret;
}

double normInf( double* a, unsigned int n )
{
   double ret = 0;

   for ( int i = 0; i < n; ++i )
      if ( abs( a[i] ) > ret )
         ret = abs( a[i] );

   return ret;
}

void cross( double* a, double* b, double* c )
{
   c[0] = a[1]*b[2] - a[2]*b[1];
   c[1] = a[2]*b[0] - a[0]*b[2];
   c[2] = a[0]*b[1] - a[1]*b[0];
}

double* cross( double* a, double* b )
{
   double* ret = new double[3];
   cross( a, b, ret );
   return ret;
}

typedef float ImType;

CImg<ImType> getImage( char* filename )
{
   CImg<ImType> im( filename );

   im.blur( 2.0, 2.0, 2.0 );

   CImg<ImType> ret( im.dimx( ), im.dimy( ), im.dimz( ), 1 );

   cimg_forXYZ( ret, x, y, z )
   {
      // Calculate the local bending energy
      ImType tmp;
      ret(x,y,z) = 0;

      if ( x > 0 && x+1 < ret.dimx( ) )
      {
         tmp = im(x+1,y,z)-2*im(x,y,z)+im(x-1,y,z);
         ret(x,y,z) += tmp*tmp;
      }

      if ( y > 0 && y+1 < ret.dimy( ) )
      {
         tmp = im(x,y+1,z)-2*im(x,y,z)+im(x,y-1,z);
         ret(x,y,z) += tmp*tmp;
      }

      if ( z > 0 && z+1 < ret.dimz( ) )
      {
         tmp = im(x,y,z+1)-2*im(x,y,z)+im(x,y-1,z-1);
         ret(x,y,z) += tmp*tmp;
      }

      if ( x > 0 && x+1 < ret.dimx( ) && y > 0 && y+1 < ret.dimy( ) )
      {
         tmp = ( im(x+1,y+1,z)-im(x-1,y+1,z)-im(x+1,y-1,z)+im(x-1,y-1,z) ) / 4;
         ret(x,y,z) += 2*tmp*tmp;
      }

      if ( x > 0 && x+1 < ret.dimx( ) && z > 0 && z+1 < ret.dimz( ) )
      {
         tmp = ( im(x+1,y,z+1)-im(x-1,y,z+1)-im(x+1,y,z-1)+im(x-1,y,z-1) ) / 4;
         ret(x,y,z) += 2*tmp*tmp;
      }

      if ( y > 0 && y+1 < ret.dimy( ) && z > 0 && z+1 < ret.dimz( ) )
      {
         tmp = ( im(x,y+1,z+1)-im(x,y-1,z+1)-im(x,y+1,z-1)+im(x,y-1,z-1) ) / 4;
         ret(x,y,z) += 2*tmp*tmp;
      }
   }

   /*
   CImgList<ImType> gradient = im.get_gradientXYZ( );

   ret.fill( 0 );

   for ( int i = 0; i < 3; ++i )
   {
      cimg_forXYZ( gradient[i], x, y, z )
         ret(x,y,z) += gradient[i](x,y,z)*gradient[i](x,y,z);
   }
   */

   ret.sqrt( );
   

   ImType minn = ret.min( );
   ImType maxx = ret.max( );

   cimg_forXYZV( ret, x, y, z, w )
      ret( x, y, z, w ) = 255 * ( ret(x,y,z,w) - minn ) / (maxx - minn);

   cimg_forXYZV( ret, x, y, z, w )
      ret( x, y, z, w ) = ( ret( x, y, z, w ) < 10 ) ? 0 : ret( x, y, z, w );
   
   return ret;
}

int main( int argc, char** argv)
{  
   CImg<ImType> allIm = getImage( argv[1] );
   CImg<ImType> im( allIm.dimx( ), allIm.dimy( ), allIm.dimz( ), 1 );

   im = allIm.get_shared_plane( 0 );

   ImageMaximumTree<ImType>* tree = ImageMaximumTree<ImType>::buildTree( im );

   double pointOnLine[]   = { im.dimx( )/2, im.dimy( )/2, 0 };
   double lineDirection[] = { 0, 0, 0 };
   double point[3];

   CImg<ImType> outputImage( im.dimx( ), im.dimy( ), im.dimz( ), 3 );
   outputImage.fill( 0 );

   CImgDisplay disp( outputImage );

   int lastX = disp.mouse_x + 1;
   int lastY = disp.mouse_y + 1;

   double threshold = 0;

   WeightFunction<double>* weightFunc = new AnisotropicNormalWeightFunction<double>( 50, 1 );

   while ( !disp.is_closed )
   {
      if ( lastX != disp.mouse_x || lastY != disp.mouse_y )
      {
         bool doSearch = true;
         bool overHandle = ( abs( disp.mouse_x - pointOnLine[0] ) < 2 &&
               abs( disp.mouse_y - pointOnLine[1] ) < 2 );

         overHandle = true;

         if ( disp.button != 0 )
         {
            pointOnLine[0] = disp.mouse_x;
            pointOnLine[1] = disp.mouse_y;
            doSearch = false;
         }

         if ( doSearch )
         {
            lineDirection[0] = (lastX = disp.mouse_x) - pointOnLine[0];
            lineDirection[1] = (lastY = disp.mouse_y) - pointOnLine[1];

            double norm = sqrt( lineDirection[0]*lineDirection[0] + lineDirection[1]*lineDirection[1] );

            lineDirection[0] /= norm;
            lineDirection[1] /= norm;
         }

         outputImage.fill( 0 );
         outputImage.get_shared_plane( 0 ) = im;
         outputImage.get_shared_plane( 1 ) = im;
         outputImage.get_shared_plane( 2 ) = im;
         
         //MaximumTreeSearch<ImType>::setDebugImage( &outputImage );

         for ( int i = 0; i < 3; ++i )
            point[i] = pointOnLine[i];

         MaximumTreeSearch<ImType>::findMaximum( im, pointOnLine, lineDirection, weightFunc, threshold, point );
         //MaximumTreeSearch<ImType>::findMaximum( im, pointOnLine, lineDirection, weightFunc, threshold, tree, point );
         
         int pointInt[] = {0,0,0};
         for ( int i = 0; i < 3; ++i )
         {
            //std::cerr << point[i] << " ";
            pointInt[i] = static_cast<int>( 0.5 + point[i] );
         }
         //std::cerr << std::endl;

         allIm.get_shared_plane( 0 ) = im;

         allIm = outputImage;
         
         allIm( pointInt[0], pointInt[1], pointInt[2], 0 ) = 0;
         allIm( pointInt[0], pointInt[1], pointInt[2], 1 ) = 255;
         allIm( pointInt[0], pointInt[1], pointInt[2], 2 ) = 0;
         
         float green[] = {0,255,0};
         allIm.draw_rectangle( pointInt[0]-1, pointInt[1]-1, pointInt[0]+1, pointInt[1]+1, green );

         float blue[] = {0,0,255};
         allIm.draw_rectangle( pointOnLine[0]-2, pointOnLine[1]-2, pointOnLine[0]+2, pointOnLine[1]+2, blue );

         allIm.display( disp );
      }

      cimg::wait( 20 );
   }

   weightFunc->finishedWith( );
}