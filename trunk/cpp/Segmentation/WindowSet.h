#ifndef __WINDOWSET_H__
#define __WINDOWSET_H__

#include "CImg.h"
#include <vector>

using namespace cimg_library;

namespace AWT
{
   class WindowSet
   {
   public:
      void add( CImgDisplay* display );

      bool isShown( ) const;

      void show( );
      void hide( );
   protected:
      std::vector<CImgDisplay*> m_Displays;
   };
}

bool AWT::WindowSet::isShown( ) const
{
   for ( unsigned int i = 0; i < m_Displays.size( ); ++i )
      if ( !m_Displays[i]->is_closed )
         return true;

   return false;
}

void AWT::WindowSet::add(cimg_library::CImgDisplay *display)
{
   m_Displays.push_back( display );
}

void AWT::WindowSet::show( )
{
   for ( unsigned int i = 0; i < m_Displays.size( ); ++i )
   {
      if ( m_Displays[i]->is_closed )
         m_Displays[i]->show( );
   }
}

void AWT::WindowSet::hide( )
{
   for ( unsigned int i = 0; i < m_Displays.size( ); ++i )
   {
      if ( ! m_Displays[i]->is_closed )
         m_Displays[i]->close( );
   }
}

#endif // __WINDOWSET_H__