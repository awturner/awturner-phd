#ifndef __INTENSITYWINDOWGUI_H__
#define __INTENSITYWINDOWGUI_H__

#include "SimpleCImgGui.h"

namespace AWT
{
   class IntensityWindowGui : public SimpleCImgGui
   {
   public:
      IntensityWindowGui( const char* title, float* centre, float* width );

      ~IntensityWindowGui( );

      template <class T>
      void setImage( CImg<T>& im );

      bool refresh( );

   protected:
      void drawGraph( CImg<long>& data, CImg<unsigned char>& ret )
      {
         ret.fill( 255 );

         unsigned char col[] = { 255, 0, 0 };

         long minn = 0;
         long maxx = data.max( );

         long delta = maxx - minn;
         minn -= delta / 10;
         maxx += delta / 10;

         int h = ret.dimy( );

         cimg_forY( data, y )
         {
            for ( int x = 1; x < data.dimx( ); ++x )
            {
               int y0 = h - static_cast<int>( h * ( data(x-1,y) - minn + 0.0 ) / ( maxx - minn + 0.0 ) + 0.5 );
               int y1 = h - static_cast<int>( h * ( data(x  ,y) - minn + 0.0 ) / ( maxx - minn + 0.0 ) + 0.5 );

               ret.draw_line( x-1, y0, x, y1, col );
            }
         }
      }

      float calculateIntensity( int x );
      int calculatePosition( float i );

      float       m_ImageMin, m_ImageMax;

      CImg<long>* m_Histogram;
      float*      m_Centre;
      float*      m_Width;

      float       m_MouseDownAt;

      bool        m_MouseWasDown;
   };
}

AWT::IntensityWindowGui::IntensityWindowGui( const char* title, float* centre, float* width )
: AWT::SimpleCImgGui( title )
{
   m_Histogram = 0;
   m_MouseWasDown = false;

   m_Centre = centre;
   m_Width  = width;

   init( 256, 256 );
}

AWT::IntensityWindowGui::~IntensityWindowGui( )
{
   if ( m_Histogram )
      delete m_Histogram;
}

template <class T>
void  AWT::IntensityWindowGui::setImage(cimg_library::CImg<T> &im)
{
   if ( m_Histogram )
      delete m_Histogram;

   CImg<long> hist = im.get_histogram( );
   m_Histogram = new CImg<long>( hist );

   m_ImageMin = im.min( );
   m_ImageMax = im.max( );

   m_MouseWasDown = false;
}

float AWT::IntensityWindowGui::calculateIntensity( int x )
{
   return ( x / ( m_Image->dimx( ) + 0.f ) ) * ( m_ImageMax - m_ImageMin ) + m_ImageMin;
}

int AWT::IntensityWindowGui::calculatePosition( float intensity )
{
   return static_cast<int>( ( intensity - m_ImageMin ) / ( m_ImageMax - m_ImageMin ) * m_Image->dimx( ) );
}

bool AWT::IntensityWindowGui::refresh( )
{
   bool ret = false;

   if ( !m_MouseWasDown && m_Disp->button != 0 )
   {
      m_MouseWasDown = true;

      m_MouseDownAt = calculateIntensity( m_Disp->mouse_x );

      *m_Centre = m_MouseDownAt;
      *m_Width  = 0;

      ret = true;
   }
   else if ( m_MouseWasDown && m_Disp->button == 0 )
   {
      m_MouseWasDown = false;

      ret = true;
   }
   else if ( m_MouseWasDown && m_Disp->button != 0 )
   {
      float intensity = calculateIntensity( m_Disp->mouse_x );

      *m_Centre =    ( intensity + m_MouseDownAt ) / 2.f;
      *m_Width  = abs( intensity - m_MouseDownAt );

      ret = true;
   }

   drawGraph( *m_Histogram, *m_Image );

   unsigned char blue[] = { 0, 0, 255 };

   int x = calculatePosition( *m_Centre );
   m_Image->draw_line( x, 0, x, m_Image->dimy( ), blue, 1.f, 0xFFFFFFFF );

   x = calculatePosition( *m_Centre - *m_Width/2.f );
   m_Image->draw_line( x, 0, x, m_Image->dimy( ), blue, 1.f, 0xFFFF0000 );

   x = calculatePosition( *m_Centre + *m_Width/2.f );
   m_Image->draw_line( x, 0, x, m_Image->dimy( ), blue, 1.f, 0xFFFF0000 );

   m_Disp->display( *m_Image );

   return ret;
}

#endif // __INTENSITYWINDOWGUI_H__
