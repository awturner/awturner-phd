#ifndef __RECTANGULARLASSOO_H__
#define __RECTANGULARLASSOO_H__

#include "DrawingTool.h"

#include <vector>

namespace AWT
{
   template <class T, class ScanT, unsigned int V>
   class RectangularLassoo : public DrawingTool<T,ScanT,V>
   {
   public:
      RectangularLassoo( CommandManager* m );

      virtual char* getName( ) const;

      virtual bool onMouseButton( const int x, const int y, const int z, const int button );
      virtual bool onMouseMove( const int x, const int y, const int z, const int button );

   protected:
      int xfrom, yfrom;
      int xto,   yto;

      int stipple;
   };
}

using namespace AWT;
using namespace cimg_library;

template <class T, class ScanT, unsigned int V>
AWT::RectangularLassoo<T,ScanT,V>::RectangularLassoo( AWT::CommandManager* m )
: AWT::DrawingTool<T,ScanT,V>::DrawingTool( m )
{
   stipple = 0x55555555;
}

template <class T, class ScanT, unsigned int V>
char* AWT::RectangularLassoo<T,ScanT,V>::getName( ) const
{
   return "Rect Lassoo";
}

#include <CImg.h>

template <class T, class ScanT, unsigned int V>
bool AWT::RectangularLassoo<T,ScanT,V>::onMouseButton( const int mx, const int my, const int mz, const int button )
{
   if ( m_CurrentImage != 0 )
   {
      if ( button )
      {
         // Make sure the loop is clear
         xfrom = xto = mx;
         yfrom = yto = my;
      }
      else
      {
         CImg<T> tmp( m_CurrentImage->dimx( ), m_CurrentImage->dimy( ) );
         tmp.fill( 0 );
         T col[] = { 255 };

         tmp.draw_line( xfrom, yfrom, xto, yfrom, col );
         tmp.draw_line( xfrom, yto, xto, yto, col );
         tmp.draw_line( xfrom, yfrom, xfrom, yto, col );
         tmp.draw_line( xto, yfrom, xto, yto, col );

         cimg_forXY( tmp, x, y )
         {
            if ( tmp( x, y ) != col[0] )
            {
               col[0] = 2;
               tmp.draw_fill( x, y, col );

               // Now we have 3 values of pixel:
               // 1 is the boundary we have painted
               // 0 is on one side of the boundary, 2 is on the other
               // Need to determine which of these is the one which has been
               // lassooed.  The boundary is always closed by construction.

               {
                  cimg_for_borderXY( tmp, x, y, 1 )
                  {
                     if ( tmp( x, y ) != 1 )
                     {
                        col[0] = tmp(x, y);
                        x = tmp.dimx( );
                        y = tmp.dimy( );
                        break;
                     }
                  }
               }
               
               // Get out of the loop
               x = tmp.dimx( );
               y = tmp.dimy( );
               break;
            }
         }

         int x, y, v;
         cimg_forXY( tmp, x, y )
         {
            if ( tmp( x, y ) != col[0] )
            {
               cimg_forV( tmp, v )
               {
                  if ( (*m_CurrentImage)(x,y,mz,v) == m_Background[v] )
                  {
                     // If it isn't, then paint it
                     cimg_forV( tmp, v )
                        (*m_CurrentImage)(x,y,mz,v) = m_Foreground[v];
                     break;
                  }
               }
            }
         }

         if ( m_OverlayImage )
            m_OverlayImage->fill( 0 );

         return true;
      }
   }
   
   return false;
}

template <class T, class ScanT, unsigned int V>
bool AWT::RectangularLassoo<T,ScanT,V>::onMouseMove(const int x, const int y, const int z, const int button)
{
   if ( button )
   {
      if ( m_OverlayImage != 0 )
      {
         int x, y;

         int mx = ( xfrom < xto ) ? xfrom : xto; int nx = xfrom + xto - mx;
         int my = ( yfrom < yto ) ? yfrom : yto; int ny = yfrom + yto - my;

         for ( x = mx; x <= nx; ++x )
            (*m_OverlayImage)( x, yfrom ) = (*m_OverlayImage)( x, yto ) = 0;

         for ( y = my; y <= ny; ++y )
            (*m_OverlayImage)( xfrom, y ) = (*m_OverlayImage)( xto, y ) = 0;

      }

      xto = x;
      yto = y;

      if ( m_OverlayImage != 0 )
      {
         unsigned char white[] = { 255 };

         m_OverlayImage->draw_line( xfrom, yfrom, xto,   yfrom, white, 1.f, stipple );
         m_OverlayImage->draw_line( xfrom, yto,   xto,   yto,   white, 1.f, stipple );
         m_OverlayImage->draw_line( xfrom, yfrom, xfrom, yto,   white, 1.f, stipple );
         m_OverlayImage->draw_line( xto,   yfrom, xto,   yto,   white, 1.f, stipple );

         return true;
      }
   }

   return false;
}

#endif // __RECTANGULARLASSOO_H__