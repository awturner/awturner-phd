/*
* ################################################################################
* ### MIT License
* ################################################################################
* 
* Copyright (c) 2006-2011 Andy Turner
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/
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