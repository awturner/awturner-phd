#ifndef __FILL3DTOOL_H__
#define __FILL3DTOOL_H__

#include "DrawingTool.h"

#include <ostream>

namespace AWT
{
   template <class T, class ScanT, unsigned int V>
   class Fill3DTool : public DrawingTool<T,ScanT,V>
   {
   public:
      Fill3DTool( CommandManager* m );

      virtual char* getName( ) const;

      virtual bool onMouseButton( const int x, const int y, const int z, const int button );
      
      virtual void printState( std::ostream& os ) const
      {
         switch ( errorState )
         {
         case FILL_AREATOOLARGE:
            os << "Fill area too big!";
            break;
         default:
            os << "OK";
         }
      }

   protected:
      enum ErrorState { FILL_NOERROR, FILL_AREATOOLARGE, FILL_ALREADYCOLOUR };

      int mouseAtX;
      int mouseAtY;

      ErrorState errorState;
   };
}

using namespace AWT;
using namespace cimg_library;

template <class T, class ScanT, unsigned int V>
AWT::Fill3DTool<T,ScanT,V>::Fill3DTool( AWT::CommandManager* m )
: AWT::DrawingTool<T,ScanT,V>::DrawingTool( m )
{
   errorState = FILL_NOERROR;
}

template <class T, class ScanT, unsigned int V>
char* AWT::Fill3DTool<T,ScanT,V>::getName( ) const
{
   return "Fill 3D";
}

template <class T, class ScanT, unsigned int V>
bool AWT::Fill3DTool<T,ScanT,V>::onMouseButton( const int mx, const int my, const int mz, const int button )
{
   if ( m_CurrentImage != 0 && button == 1 )
   {
      const int minz = 0;
      const int maxz = m_CurrentImage->dimz( )-1;
      const int zz   = mz - minz;

      CImg<T> im = m_CurrentImage->get_shared_planes( minz, maxz );

      int x, y, z, v;
      bool flag = true;

      T fg[V];
      getForegroundColour( fg );

      cimg_forV( im, v )
      {
         if ( im(mx,my,zz,v) != fg[v] )
         {
            flag = false;
            break;
         }
      }

      if ( flag )
      {
         errorState = FILL_ALREADYCOLOUR;
         return true;
      }
      
      CImg<T> region( im.dimx( ), im.dimy( ), im.dimz( ), V );

      im.draw_fill( mx, my, zz, fg, region, 0, 0 );

      unsigned long count = 0;
      const unsigned long maxcount = region.size( ) / 2;
      cimg_forXYZ( region, x, y, z )
      {
         if ( region( x, y, z ) )
            if ( ++count > maxcount )
            {
               flag = true;
               x = region.dimx( );
               y = region.dimy( );
               z = region.dimz( );
               break;
            }
      }

      if ( !flag )
      {
         cimg_forXYZ( region, x, y, z )
         {
            if ( region(x,y,z) )
            {
               cimg_forV( im, v )
               {
                  im(x,y,z,v) = fg[v];
               }
            }
         }

         errorState = FILL_NOERROR;
         return true;
      }
      else
      {
         errorState = FILL_AREATOOLARGE;
         return true;
      }
   }

   errorState = FILL_NOERROR;
   return false;
}

#endif // __FILL3DTOOL_H__