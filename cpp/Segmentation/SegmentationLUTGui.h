#ifndef __SEGMENTATIONLUTGUI_H__
#define __SEGMENTATIONLUTGUI_H__

#include "SimpleCImgGui.h"
#include "SegmentationLUT.h"

namespace AWT
{
   class SegmentationLUTGui : public SimpleCImgGui
   {
   public:
      SegmentationLUTGui( const char* title, SegmentationLUT* lut, unsigned char* fg, unsigned char* bg );

      virtual bool refresh( );

   protected:
      SegmentationLUT* m_Lut;
      unsigned char*   m_Fg;
      unsigned char*   m_Bg;

      CImgList<unsigned char> m_Font;
   };
}

AWT::SegmentationLUTGui::SegmentationLUTGui(const char* title, AWT::SegmentationLUT *lut, unsigned char *fg, unsigned char *bg)
: AWT::SimpleCImgGui( title )
{
   m_Lut = lut;
   m_Fg = fg;
   m_Bg = bg;

   init( 64, lut->getNumberOfColours( ) * 32 );
   
   m_Font = CImgList<unsigned char>::get_font( 13 );

   m_Disp->title = "Colours";
}

bool AWT::SegmentationLUTGui::refresh( )
{
   unsigned char ncolours = m_Lut->getNumberOfColours( );

   unsigned char black[] = { 0, 0, 0 };
   unsigned char white[] = { 255, 255, 255 };

   unsigned char col[3];

   unsigned char* bg;

   for ( unsigned int i = 0; i < ncolours; ++i )
   {
      m_Lut->getColour( 1.f, i, col );

      m_Image->draw_rectangle( 0, 32*i, 64, 32*(i+1), col );

      float luminance = ( 0.3f*col[0] + 0.59f*col[1] + 0.11f*col[2] );

      if ( i == *m_Fg )
      {
         m_Image->draw_text( "F", 16, 32*i + 16, (luminance>127.f)?&black[0]:&white[0], col, m_Font );
      }
      else if ( i == *m_Bg )
      {
         m_Image->draw_text( "B", 48, 32*i + 16, (luminance>127.f)?&black[0]:&white[0], col, m_Font );
      }
   }

   unsigned int hoverColour = m_Disp->mouse_y / 32;
   m_Lut->getColour( 1.f, hoverColour, col );
   float luminance = ( 0.3f*col[0] + 0.59f*col[1] + 0.11f*col[2] );

   bool ret = false;

   if ( m_Disp->button )
   {
      if ( m_Disp->mouse_x < 32 )
         *m_Fg = hoverColour;
      else
         *m_Bg = hoverColour;

      m_Disp->button = 0;
      ret = true;
   }

   if ( m_Disp->mouse_x != -1 && m_Disp->mouse_y != -1 )
   {
      int x0 = 32*( m_Disp->mouse_x / 32 );
      int y0 = 32*( m_Disp->mouse_y / 32 );
      m_Image->draw_rectangle( x0, y0, x0+32, y0+32, (luminance>127.f)?&black[0]:&white[0], 1.f, 0xFFFFFFFF );
   }
   
   m_Disp->display( *m_Image );

   return ret;
}

#endif __SEGMENTATIONLUTGUI_H__