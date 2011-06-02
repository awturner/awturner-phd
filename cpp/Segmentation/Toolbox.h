#ifndef __TOOLBOX_H__
#define __TOOLBOX_H__

#include "DrawingTool.h"
#include "Useful/Exception.h"
#include <vector>

namespace AWT
{
   template <class T, class ScanT, unsigned int V>
   class Toolbox
   {
   public:
      Toolbox( );
      void addTool( DrawingTool<T,ScanT,V>* in_Tool );

      unsigned int getNumberOfTools( ) const;
      DrawingTool<T,ScanT,V>* getTool( unsigned int idx ) const;

      DrawingTool<T,ScanT,V>* getCurrentTool( ) const;

      unsigned int getCurrentToolIndex( ) const;
      void setCurrentToolIndex( unsigned int idx );

   protected:
      std::vector<DrawingTool<T,ScanT,V>*> m_Tools;
      unsigned int                         m_CurToolIdx;
   };
}

template <class T, class ScanT, unsigned int V>
AWT::Toolbox<T,ScanT,V>::Toolbox( )
{
   m_CurToolIdx = 0;
}

template <class T, class ScanT, unsigned int V>
void AWT::Toolbox<T,ScanT,V>::addTool(AWT::DrawingTool<T,ScanT,V> *in_Tool)
{
   m_Tools.push_back( in_Tool );
}

template <class T, class ScanT, unsigned int V>
unsigned int AWT::Toolbox<T,ScanT,V>::getNumberOfTools( ) const
{
   return m_Tools.size( );
}

template <class T, class ScanT, unsigned int V>
AWT::DrawingTool<T,ScanT,V>* AWT::Toolbox<T,ScanT,V>::getTool( unsigned int idx ) const
{
   return m_Tools[idx];
}

template <class T, class ScanT, unsigned int V>
AWT::DrawingTool<T,ScanT,V>* AWT::Toolbox<T,ScanT,V>::getCurrentTool( ) const
{
   return m_Tools[m_CurToolIdx];
}

template <class T, class ScanT, unsigned int V>
unsigned int AWT::Toolbox<T,ScanT,V>::getCurrentToolIndex( ) const
{
   return m_CurToolIdx;
}

template <class T, class ScanT, unsigned int V>
void AWT::Toolbox<T,ScanT,V>::setCurrentToolIndex( unsigned int idx )
{
   if ( m_Tools.size( ) == 0 )
      AWTEXCEPTIONTHROW("No tools to select!");

   m_CurToolIdx = idx % m_Tools.size( );
}

#endif // __TOOLBOX_H__