#ifndef __TOOLBOXGUI_H__
#define __TOOLBOXGUI_H__

#include "Toolbox.h"
#include "CImg.h"

#include "SimpleCImgGui.h"

using namespace cimg_library;

namespace AWT
{
   template <class T, class ScanT, unsigned int V>
   class ToolboxGui : public SimpleCImgGui
   {
   public:
      ToolboxGui( const char* title, Toolbox<T,ScanT,V>* in_Toolbox );
      
      virtual bool refresh( );

   protected:
      Toolbox<T,ScanT,V>*  m_Toolbox;
      unsigned int         m_NumberOfTools;

      CImgList<unsigned char> m_Font;
   };
}

template <class T, class ScanT, unsigned int V>
AWT::ToolboxGui<T,ScanT,V>::ToolboxGui(const char* title, AWT::Toolbox<T,ScanT,V> *in_Toolbox)
: AWT::SimpleCImgGui( title )
{
   if ( in_Toolbox == 0 )
      AWTEXCEPTIONTHROW("Toolbox must not be null!");

   m_Toolbox = in_Toolbox;

   m_NumberOfTools = m_Toolbox->getNumberOfTools( );

   init( 100, 16*m_NumberOfTools );

   m_Font = CImgList<unsigned char>::get_font( 13 );
}

template <class T, class ScanT, unsigned int V>
bool AWT::ToolboxGui<T,ScanT,V>::refresh( )
{
   // Called in the polling loop

   bool imageResized = false;

   if ( m_NumberOfTools != m_Toolbox->getNumberOfTools( ) )
   {
      createImage( 100, 16*m_NumberOfTools );
      imageResized = true;
   }

   if ( m_Disp->is_resized || imageResized )
   {
      m_Disp->resize( *m_Image );
   }

   int mouseHover = (m_Disp->mouse_y / 16);

   bool ret = false;

   if ( m_Disp->button == 1 )
   {
      m_Toolbox->setCurrentToolIndex( mouseHover );
      ret = true;
      m_Disp->button = 0;
   }

   unsigned char white[]  = { 255, 255, 255 };
   unsigned char black[]  = {   0,   0,   0 };
   unsigned char active[] = { 255, 127,   0 };
   unsigned char gray[]   = { 127, 127, 127 };

   m_Image->fill( 0 );

   int yfrom = 16 * mouseHover;
   m_Image->draw_rectangle( 0, yfrom, m_Image->dimx( ), yfrom+16, gray );

   DrawingTool<T,ScanT,V>* tool;
   unsigned char* fg;
   unsigned char* bg;
   for ( unsigned int t = 0; t < m_NumberOfTools; ++t )
   {
      tool = m_Toolbox->getTool( t );

      fg = ( t == m_Toolbox->getCurrentToolIndex( ) ) ? &active[0] : &white[0];
      bg = ( t == mouseHover                       ) ? &gray[0] : &black[0];

      m_Image->draw_text( tool->getName( ), 0, 16*(t) + 2, fg, bg, m_Font );
      
   }

   m_Disp->display( *m_Image );

   return ret;
}

#endif // __TOOLBOXGUI_H__
