#ifndef __ALLSLICEAllSliceHoleFillTool_H__
#define __ALLSLICEAllSliceHoleFillTool_H__

#include "DrawingTool.h"

#include <vector>

namespace AWT
{
   template <class T, class ScanT, unsigned int V>
   class AllSliceHoleFillTool : public DrawingTool<T,ScanT,V>
   {
   public:
      AllSliceHoleFillTool( CommandManager* m );

      virtual char* getName( ) const;

      virtual bool onMouseButton( const int x, const int y, const int z, const int button );
   protected:
   };
}

using namespace AWT;
using namespace cimg_library;

template <class T, class ScanT, unsigned int V>
AWT::AllSliceHoleFillTool<T,ScanT,V>::AllSliceHoleFillTool( AWT::CommandManager* m )
: AWT::DrawingTool<T,ScanT,V>::DrawingTool( m )
{
}

template <class T, class ScanT, unsigned int V>
char* AWT::AllSliceHoleFillTool<T,ScanT,V>::getName( ) const
{
   return "All Slice Hole Fill";
}

template <class T, class ScanT, unsigned int V>
bool AWT::AllSliceHoleFillTool<T,ScanT,V>::onMouseButton( const int mx, const int my, const int mz, const int button )
{
   //DEBUGMACRO( "onMouseButton" );
   if ( m_CurrentImage != 0 && !button )
   {
      cimg_forZ( *m_CurrentImage, zz )
      {
         CImg<T> im = m_CurrentImage->get_shared_plane( zz );

         CImg<T> tmp   ( im.dimx( ), im.dimy( ), im.dimz( ) );
         CImg<T> region( im.dimx( ), im.dimy( ), im.dimz( ) );

         tmp.fill( 0 );

         T fg[V], bg[V];
         getForegroundColour( fg );
         getBackgroundColour( bg );

         bool isSame;
         cimg_forXY( tmp, x, y )
         {
            isSame = true;

            cimg_forV( im, v )
            {
               if ( im( x, y, 0, v ) != bg[v] )
               {
                  isSame = false;
                  break;
               }
            }

            if ( isSame )
               tmp( x, y ) = 1;
         }

         T fill[] = { 0 };
         tmp.draw_fill( mx, my, 0, fill, region );

         cimg_forXY( tmp, x, y )
         {
            if ( tmp( x, y ) )
               cimg_forV( im, v )
                  im( x, y, 0, v ) = fg[v];
         }
      }

      return true;
   }
   
   return false;
}

#endif // __ALLSLICEAllSliceHoleFillTool_H__