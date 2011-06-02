#ifndef __PENTOOL_H__
#define __PENTOOL_H__

#include "DrawingTool.h"

namespace AWT
{
   template <class T, class ScanT, unsigned int V>
   class PenTool : public DrawingTool<T,ScanT,V>
   {
   public:
      PenTool( CommandManager* manager );

      virtual char* getName( ) const;

      virtual bool onMouseButton( const int x, const int y, const int z, const int button );
      virtual bool onMouseMove( const int x, const int y, const int z, const int button );

   protected:
      int mouseAtX;
      int mouseAtY;
      int mouseAtZ;
   };
}

using namespace AWT;
using namespace cimg_library;

template <class T, class ScanT, unsigned int V>
AWT::PenTool<T,ScanT,V>::PenTool( AWT::CommandManager* m )
: DrawingTool<T,ScanT,V>( m )
{

}

template <class T, class ScanT, unsigned int V>
char* AWT::PenTool<T,ScanT,V>::getName( ) const
{
   return "Pen";
}

template <class T, class ScanT, unsigned int V>
bool AWT::PenTool<T,ScanT,V>::onMouseButton( const int x, const int y, const int z, const int button )
{
   if ( button )
   {
      return onMouseMove( mouseAtX = x, mouseAtY = y, mouseAtZ = z, button );
   }

   return false;
}

#include <iostream>

template <class T, class ScanT, unsigned int V>
bool AWT::PenTool<T,ScanT,V>::onMouseMove(const int x, const int y, const int z, const int button)
{
   if ( button == 1 )
   {
      CImg<T>* im = &( getCurrentImage( )->get_shared_plane( z ) );
      if ( im == 0 )
         return false;

      im->draw_line( mouseAtX, mouseAtY, x, y, m_Foreground );

      if ( m_OverlayImage != 0 )
      {
         T black[V];
         cimg_forV( (*m_OverlayImage), v )
            black[v] = 127;

         m_OverlayImage->draw_line( mouseAtX, mouseAtY, x, y, black );

      }
      
      mouseAtX = x;
      mouseAtY = y;

      return true;
   }

   if ( button == 0 && m_CurrentImage != 0 && m_OverlayImage != 0 )
   {
      CImg<T> im = m_CurrentImage->get_shared_plane( z );
      
      T curr[V];
      cimg_forV( im, v )
         curr[v] = im( x, y, 0, v );

      T fg[V];
      getForegroundColour( fg );

      m_OverlayImage->fill( 0 );

      im.draw_fill( x, y, 0, fg, *m_OverlayImage );

      cimg_forXY( (*m_OverlayImage), xx, yy )
         if ( (*m_OverlayImage)(xx,yy) )
         {
            (*m_OverlayImage)(xx,yy) = 255;
            cimg_forV( im, v )
               im(xx,yy,0,v) = curr[v];
         }
      
      return true;
   }

   return false;
}


#endif // __PENTOOL_H__