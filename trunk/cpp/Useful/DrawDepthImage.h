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