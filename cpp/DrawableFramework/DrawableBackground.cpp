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
#include "DrawableBackground.h"

#include <limits>

struct AWT::DrawableBackground::D
{
   bool  m_Visible;
   float m_TopCol[3];
   float m_BottomCol[3];
};

AWT::DrawableBackground::DrawableBackground( )
{
   m_D = new D;

   m_D->m_TopCol[0]    = m_D->m_TopCol[1]    = m_D->m_TopCol[2]    = 0;
   m_D->m_BottomCol[0] = m_D->m_BottomCol[1] = m_D->m_BottomCol[2] = 0;
}

AWT::DrawableBackground::~DrawableBackground( )
{
   delete m_D;
}

GETNAMEMACRO( AWT::DrawableBackground );

bool AWT::DrawableBackground::isVisible( ) const
{
   return m_D->m_Visible;
}


void AWT::DrawableBackground::setVisible( const bool vis )
{
   this->m_D->m_Visible = vis;
   modified( );
}

void AWT::DrawableBackground::getTopColor( float* col ) const
{
   col[0] = m_D->m_TopCol[0];
   col[1] = m_D->m_TopCol[1];
   col[2] = m_D->m_TopCol[2];
}

void AWT::DrawableBackground::setTopColor( const float* col )
{
   m_D->m_TopCol[0] = col[0];
   m_D->m_TopCol[1] = col[1];
   m_D->m_TopCol[2] = col[2];
}

void AWT::DrawableBackground::getBottomColor( float* col ) const
{
   col[0] = m_D->m_BottomCol[0];
   col[1] = m_D->m_BottomCol[1];
   col[2] = m_D->m_BottomCol[2];
}

void AWT::DrawableBackground::setBottomColor( const float* col )
{
   m_D->m_BottomCol[0] = col[0];
   m_D->m_BottomCol[1] = col[1];
   m_D->m_BottomCol[2] = col[2];
}

void AWT::DrawableBackground::getBounds( double* out_Bounds )
{
   for ( int i = 0; i < 6; ++i )
      out_Bounds[i] = std::numeric_limits<double>::quiet_NaN( );
}
