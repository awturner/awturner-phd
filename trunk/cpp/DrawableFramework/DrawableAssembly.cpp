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
#include "DrawableAssembly.h"

#include <vector>
#include <limits>

struct AWT::DrawableAssembly::D
{
   typedef std::pair< std::string, Drawable::P > DrawPair;
   std::vector< DrawPair > m_Objects;

   DrawableAssembly* m_Parent;

   bool m_Visible;
};

AWT::DrawableAssembly::DrawableAssembly( )
{
   m_D = new D;

   m_D->m_Visible = true;
   m_D->m_Parent = 0;
}

AWT::DrawableAssembly::~DrawableAssembly( )
{
   delete m_D;
}

AWT::DrawableAssembly::P AWT::DrawableAssembly::getInstance( )
{
   AUTOGETINSTANCE( AWT::DrawableAssembly, () );
}

GETNAMEMACRO( AWT::DrawableAssembly );

void AWT::DrawableAssembly::draw( AWT::DrawContext::P context, const bool transparentPass )
{
   if ( !m_D->m_Visible )
      return;

   const unsigned int nobjs = getNumberOfChildren( );
   for ( unsigned int i = 0; i < nobjs; ++i )
   {
      Drawable::P child = getChild( i );

      if ( child->isVisible( ) )
      {
         child->draw( context, transparentPass );
      }
   }
}

void AWT::DrawableAssembly::add( const std::string& name, Drawable::P drawable )
{
   m_D->m_Objects.push_back( std::pair< std::string, Drawable::P >( name, drawable ) );

   drawable->setParent( this );
}

void AWT::DrawableAssembly::remove( Drawable::P drawable )
{
   std::vector<D::DrawPair>::iterator it = m_D->m_Objects.begin( );
   std::vector<D::DrawPair>::iterator en = m_D->m_Objects.end( );

   while ( it != en )
   {
      if ( it->second == drawable )
      {
         m_D->m_Objects.erase( it, it+1 );
         drawable->setParent( 0 );
         break;
      }

      ++it;
   }
}

void AWT::DrawableAssembly::clear( )
{
   while ( !m_D->m_Objects.empty( ) )
   {
      D::DrawPair back = m_D->m_Objects.back( );
      m_D->m_Objects.pop_back( );

      back.second->setParent( 0 );
   }
}

unsigned int AWT::DrawableAssembly::getNumberOfChildren( ) const
{
   return static_cast< unsigned int >( m_D->m_Objects.size( ) );
}

AWT::Drawable::P AWT::DrawableAssembly::getChild( const unsigned int i )
{
   return m_D->m_Objects[i].second;
}

std::string AWT::DrawableAssembly::getChildName( const unsigned int i )
{
   return m_D->m_Objects[i].first;
}

void AWT::DrawableAssembly::setParent( AWT::DrawableAssembly* p )
{
   m_D->m_Parent = p;
   modified( );
}

AWT::DrawableAssembly* AWT::DrawableAssembly::getParent( )
{
   return m_D->m_Parent;
}

void AWT::DrawableAssembly::setVisible( const bool v )
{
   if ( m_D->m_Visible = v )
   {
      m_D->m_Visible = v;
      modified( );
   }
}

bool AWT::DrawableAssembly::isVisible( ) const
{
   return m_D->m_Visible;
}

void AWT::DrawableAssembly::getBounds( double* out_Bounds )
{
   double tmpBounds[6];

   bool firstDone = false;

   const unsigned int nobjs = getNumberOfChildren( );
   for ( unsigned int i = 0; i < nobjs; ++i )
   {
      Drawable::P child = getChild( i );

      if ( child->isVisible( ) )
      {
         child->getBounds( tmpBounds );

         if ( tmpBounds[0] != tmpBounds[0] ) continue;
         if ( tmpBounds[1] != tmpBounds[1] ) continue;
         if ( tmpBounds[2] != tmpBounds[2] ) continue;
         if ( tmpBounds[3] != tmpBounds[3] ) continue;
         if ( tmpBounds[4] != tmpBounds[4] ) continue;
         if ( tmpBounds[5] != tmpBounds[5] ) continue;

         if ( !firstDone )
         {
            for ( int i = 0; i < 6; ++i )
               out_Bounds[i] = tmpBounds[i];

            firstDone = true;
         }
         else
         {
            for ( int i = 0; i < 3; ++i )
            {
               out_Bounds[2*i+0] = std::min<double>( out_Bounds[2*i+0], tmpBounds[2*i+0] );
               out_Bounds[2*i+1] = std::max<double>( out_Bounds[2*i+1], tmpBounds[2*i+1] );
            }
         }
      }
   }

   if ( !firstDone )
   {
      for ( int i = 0; i < 6; ++i )
         out_Bounds[i] = std::numeric_limits<double>::quiet_NaN();
   }
}