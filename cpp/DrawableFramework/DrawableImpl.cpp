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

#include "DrawableImpl.h"

#include "DrawableFramework/DrawMaterial.h"


struct AWT::DrawableImpl::D
{
   bool              m_Visible;

   DrawableAssembly* m_Parent;
};

AWT::DrawableImpl::DrawableImpl( )
{
   m_D = new D;
   m_D->m_Visible = true;
   m_D->m_Parent = 0;
}

AWT::DrawableImpl::~DrawableImpl( )
{
   delete m_D;
}

void AWT::DrawableImpl::draw( AWT::DrawContext::P context, const bool transparentPass )
{
   if ( !m_D->m_Visible ) return;

   DrawMaterial::P mat = getMaterial( );

   if ( *mat != 0 )
   {
      if ( mat->isOpaque( ) ^ transparentPass )
      {
         mat->prepare( );
         drawImpl( context );
         mat->unprepare( );
      }
   }
   else
   {
      // Only draw if we aren't transparent
      if ( !transparentPass )
         drawImpl( context );
   }
}

bool AWT::DrawableImpl::isVisible( ) const
{
   return m_D->m_Visible;
}

void AWT::DrawableImpl::setVisible( const bool vis )
{
   this->m_D->m_Visible = vis;
   modified( );
}

void AWT::DrawableImpl::setParent( AWT::DrawableAssembly* p )
{
   m_D->m_Parent = p;
   modified( );
}

AWT::DrawableAssembly* AWT::DrawableImpl::getParent( )
{
   return m_D->m_Parent;
}
