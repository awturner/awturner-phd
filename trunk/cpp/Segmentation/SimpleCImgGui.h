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
#ifndef __SIMPLECIMGGUI_H__
#define __SIMPLECIMGGUI_H__

#include "CImg.h"

using namespace cimg_library;

namespace AWT
{
   class SimpleCImgGui
   {
   public:
      SimpleCImgGui( const char* title );
      virtual ~SimpleCImgGui( );
      virtual bool refresh( ) = 0;

      virtual void show( );

      virtual CImgDisplay* getDisplay( ) const;

   protected:
      void init( unsigned int w, unsigned int h );
      void createImage( unsigned int w, unsigned int h );

      CImgDisplay*         m_Disp;
      CImg<unsigned char>* m_Image;
      const char*          m_Title;
   };
}

AWT::SimpleCImgGui::SimpleCImgGui( const char* title )
: m_Title( title )
{
   m_Image = 0;
   m_Disp = 0;
}

AWT::SimpleCImgGui::~SimpleCImgGui( )
{
   if ( m_Disp )  delete m_Disp;
   if ( m_Image ) delete m_Image;
}

CImgDisplay* AWT::SimpleCImgGui::getDisplay( ) const
{
   return m_Disp;
}

void AWT::SimpleCImgGui::show( )
{
   if ( !m_Disp->is_closed )
   {
      m_Disp->show( );
   }
}

void AWT::SimpleCImgGui::init(unsigned int w, unsigned int h)
{
   createImage( w, h );
   m_Disp  = new CImgDisplay( *m_Image, m_Title );
}

void AWT::SimpleCImgGui::createImage( unsigned int w, unsigned int h )
{
   if ( m_Image != 0 )
      delete m_Image;

   m_Image = new CImg<unsigned char>( w, h, 1, 3 );
}

#endif //__SIMPLECIMGGUI_H__