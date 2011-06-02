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
#ifndef __DRAWDEPTHIMAGE_H__
#define __DRAWDEPTHIMAGE_H__

#include "CImg.h"

namespace AWT
{
   template <class Ty, class Ot>
   CImg<Ot> drawSimpleDepthImage( CImg<Ty>& img, int direction )
   {
      CImg<Ot> out( img.dimx( ), img.dimy( ) );
      drawSimpleDepthImage( img, direction, out );
      return out;
   }

   template <class Ty, class Ot>
   void drawSimpleDepthImage( CImg<Ty>& img, int direction, CImg<Ot>& out )
   {
      out.fill( 0 );

      int xend, yend;
      int xstart, ystart;
      int xd, yd;

      int maxx[] = { img.dimx( ), img.dimy( ), img.dimz( ) };
      int maxy[] = { img.dimy( ), img.dimz( ), img.dimx( ) };

      for ( int y = 0; y < out.dimy( ); ++y )
      {
         if ( y >= maxy[direction / 2] )
            break;

         for ( int x = 0; x < out.dimx( ); ++x )
         {
            if ( x >= maxx[direction / 2] )
               break;

            switch ( direction )
            {
            case 0:
               for ( int z = img.dimz( )-1; z >= 0; --z )
               {
                  if ( img( x, y, z ) )
                  {
                     out( x, y ) = z;
                     break;
                  }
               }
               break;
            case 1:
               for ( int z = 0; z < img.dimz( ); ++z )
               {
                  if ( img( x, y, z ) )
                  {
                     out( out.dimx( )-1-x, y ) = img.dimz( ) - 1 - z;
                     break;
                  }
               }
               break;
            case 2:
               for ( int z = 0; z < img.dimx( ); ++z )
               {
                  if ( img( z, x, y ) )
                  {
                     out( x, y ) = z;
                     break;
                  }
               }
               break;
            case 3:
               for ( int z = 0; z < img.dimx( ); ++z )
               {
                  if ( img( z, x, y ) )
                  {
                     out( out.dimx( )-1-x, y ) = img.dimx( ) - 1 - z;
                     break;
                  }
               }
               break;
            case 4:
               for ( int z = 0; z < img.dimy( ); ++z )
               {
                  if ( img( y, z, x ) )
                  {
                     out( x, y ) = z;
                     break;
                  }
               }
               break;
            case 5:
               for ( int z = 0; z < img.dimy( ); ++z )
               {
                  if ( img( y, z, x ) )
                  {
                     out( out.dimx( )-1-x, y ) = img.dimy( ) - 1 - z;
                     break;
                  }
               }
               break;
            }
         }
      }

      return;

      switch ( direction )
      {
      case 0:
         for ( int y = 0; y <= yend; ++y )
            for ( int x = 0; x <= xend; ++x )
            {
               for ( int z = img.dimz( )-1; z >= 0; --z )
               {
                  if ( img( x, y, z ) )
                  {
                     out( x, y ) = z;
                     break;
                  }
               }
            }
         break;

      case 1:
         for ( int y = 0; y <= yend; ++y )
            for ( int x = xend; x >= 0; --x )
            {
               for ( int z = 0; z < img.dimz( ); ++z )
               {
                  if ( img( x, y, z ) )
                  {
                     out( x, y ) = img.dimz( ) - 1 - z;
                     break;
                  }
               }
            }
         break;

      }
   }
}

#endif // __DRAWDEPTHIMAGE_H__