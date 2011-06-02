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
#include "OpenGLDrawAxis.h"

#include "OpenGLHeaders.h"

struct AWT::OpenGLDrawAxis::D
{
   DrawableAssembly* m_Parent;
   double m_Scale;
   bool m_Visible;
};

AWT::OpenGLDrawAxis::OpenGLDrawAxis( double scale )
{
   m_D = new D;

   m_D->m_Scale = scale;
   m_D->m_Visible = true;
}

AWT::OpenGLDrawAxis::~OpenGLDrawAxis( )
{
   delete m_D;
}

AWT::OpenGLDrawAxis::P AWT::OpenGLDrawAxis::getInstance( double scale )
{
   AUTOGETINSTANCE( AWT::OpenGLDrawAxis, ( scale ) );
}

GETNAMEMACRO( AWT::OpenGLDrawAxis );

void AWT::OpenGLDrawAxis::draw( AWT::DrawContext::P context, const bool transparentPass )
{
   glDisable( GL_LIGHTING );
   glLineWidth( 2.f );
   glEnable( GL_COLOR_MATERIAL );

   glBegin( GL_LINES );

   glColor3f( 1, 0, 0 );
   glVertex3f( 0, 0, 0 );
   glVertex3f( m_D->m_Scale, 0, 0 );

   glColor3f( 0, 1, 0 );
   glVertex3f( 0, 0, 0 );
   glVertex3f( 0, m_D->m_Scale, 0 );

   glColor3f( 0, 0, 1 );
   glVertex3f( 0, 0, 0 );
   glVertex3f( 0, 0, m_D->m_Scale );

   glEnd( );

   glColor3f( 1, 0, 0 );
   context->drawText( 11*m_D->m_Scale/10, 0, 0, "X" );

   glColor3f( 0, 1, 0 );
   context->drawText( 0, 11*m_D->m_Scale/10, 0, "Y" );

   glColor3f( 0, 0, 1 );
   context->drawText( 0, 0, 11*m_D->m_Scale/10, "Z" );
}

void AWT::OpenGLDrawAxis::setVisible( const bool v )
{
   if ( v != m_D->m_Visible )
   {
      m_D->m_Visible = v;
      modified( );
   }
}

bool AWT::OpenGLDrawAxis::isVisible( ) const
{
   return m_D->m_Visible;
}

void AWT::OpenGLDrawAxis::getBounds( double* out_Bounds )
{
   out_Bounds[0] = out_Bounds[2] = out_Bounds[4] = 0;
   out_Bounds[1] = out_Bounds[3] = out_Bounds[5] = 0;
}

void AWT::OpenGLDrawAxis::setParent( DrawableAssembly* p )
{
   if ( p != m_D->m_Parent )
   {
      m_D->m_Parent = p;
      modified( );
   }
}

AWT::DrawableAssembly* AWT::OpenGLDrawAxis::getParent( )
{
   return m_D->m_Parent;
}
