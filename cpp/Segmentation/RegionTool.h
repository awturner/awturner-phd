#ifndef __REGIONTOOL_H__
#define __REGIONTOOL_H__

#include "DrawingTool.h"

#include <ostream>
#include <vector>

namespace AWT
{
   template <class T, class ScanT, unsigned int V>
   class RegionTool : public DrawingTool<T,ScanT,V>
   {
   public:
      RegionTool( CommandManager* m );
      ~RegionTool( );

      virtual char* getName( ) const;

      virtual void setCurrentImage( cimg_library::CImg<T>* im );

      virtual bool onMouseButton( const int x, const int y, const int z, const int button );
      virtual bool onMouseMove( const int x, const int y, const int z, const int button );

   protected:
      CImg<T>* m_RegionImage;
      bool     m_FirstTime;
      T        m_CurrentRegion;
   };
}

#include "ConnectedRegions.h"

using namespace AWT;
using namespace cimg_library;

template <class T, class ScanT, unsigned int V>
AWT::RegionTool<T,ScanT,V>::RegionTool( AWT::CommandManager* m )
: AWT::DrawingTool<T,ScanT,V>::DrawingTool( m )
{
   m_RegionImage = 0;
   m_FirstTime = true;
}

template <class T, class ScanT, unsigned int V>
AWT::RegionTool<T,ScanT,V>::~RegionTool( )
{
   if ( m_RegionImage != 0 )
      delete m_RegionImage;
}

template <class T, class ScanT, unsigned int V>
char* AWT::RegionTool<T,ScanT,V>::getName( ) const
{
   return "Region Highlighter";
}

template <class T, class ScanT, unsigned int V>
void AWT::RegionTool<T,ScanT,V>::setCurrentImage( CImg<T>* im )
{
   // Do the usual stuff
   AWT::DrawingTool<T,ScanT,V>::setCurrentImage( im );

   if ( m_RegionImage != 0 )
   {
      if ( m_RegionImage->dimx( ) != im->dimx( ) || m_RegionImage->dimy( ) != im->dimy( ) )
      {
         delete m_RegionImage;
         m_RegionImage = 0;      
      }
   }

   if ( m_RegionImage == 0 )
      m_RegionImage = new CImg<T>( im->dimx( ), im->dimy( ), 1, 1 );
   
   m_FirstTime = true;
   m_CurrentRegion = 0;
}

template <class T, class ScanT, unsigned int V>
bool AWT::RegionTool<T,ScanT,V>::onMouseButton( const int mx, const int my, const int mz, const int button )
{
   return onMouseMove( mx, my, mz, button );
}

template <class T, class ScanT, unsigned int V>
bool AWT::RegionTool<T,ScanT,V>::onMouseMove( const int mx, const int my, const int mz, const int button )
{
   if ( m_CurrentImage && m_OverlayImage )
   {
      if ( button )
      {
         if ( m_FirstTime )
         {
            findConnectedRegions( m_CurrentImage->get_shared_plane( mz ), *m_RegionImage );
            m_FirstTime = false;
         }

         if ( m_CurrentRegion == (*m_RegionImage)(mx, my) )
            return false;

         m_CurrentRegion = (*m_RegionImage)(mx, my);

         cimg_forXY( *m_OverlayImage, x, y )
         {
            if ( (*m_RegionImage)(x,y) != m_CurrentRegion )
               (*m_OverlayImage)(x,y) = 255;
            else
               (*m_OverlayImage)(x,y) =   0;
         }
      }
      else
      {
         m_CurrentRegion = 0;
         m_OverlayImage->fill( 0 );
      }
      
      return true;
   }
   
   return false;
}

#endif // __REGIONTOOL_H__