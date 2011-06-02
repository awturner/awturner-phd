#if 0
#include "OpenGLDrawableAssembly.h"

#include <vector>
#include <limits>

struct AWT::OpenGLDrawableAssembly::D
{
   typedef std::pair< std::string, Drawable::P > DrawPair;
   std::vector< DrawPair > m_Objects;

   DrawableAssembly* m_Parent;

   bool m_Visible;
};

AWT::OpenGLDrawableAssembly::OpenGLDrawableAssembly( )
{
   m_D = new D;

   m_D->m_Visible = true;
   m_D->m_Parent = 0;
}

AWT::OpenGLDrawableAssembly::~OpenGLDrawableAssembly( )
{
   delete m_D;
}

AWT::OpenGLDrawableAssembly::P AWT::OpenGLDrawableAssembly::getInstance( )
{
   AUTOGETINSTANCE( AWT::OpenGLDrawableAssembly, () );
}

GETNAMEMACRO( AWT::OpenGLDrawableAssembly );

void AWT::OpenGLDrawableAssembly::draw( AWT::DrawContext::P context, const bool transparentPass )
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

void AWT::OpenGLDrawableAssembly::add( const std::string& name, Drawable::P drawable )
{
   m_D->m_Objects.push_back( std::pair< std::string, Drawable::P >( name, drawable ) );

   drawable->setParent( this );
}

void AWT::OpenGLDrawableAssembly::remove( Drawable::P drawable )
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

void AWT::OpenGLDrawableAssembly::clear( )
{
   while ( !m_D->m_Objects.empty( ) )
   {
      D::DrawPair back = m_D->m_Objects.back( );
      m_D->m_Objects.pop_back( );

      back.second->setParent( 0 );
   }
}

unsigned int AWT::OpenGLDrawableAssembly::getNumberOfChildren( ) const
{
   return static_cast< unsigned int >( m_D->m_Objects.size( ) );
}

AWT::Drawable::P AWT::OpenGLDrawableAssembly::getChild( const unsigned int i )
{
   return m_D->m_Objects[i].second;
}

std::string AWT::OpenGLDrawableAssembly::getChildName( const unsigned int i )
{
   return m_D->m_Objects[i].first;
}

void AWT::OpenGLDrawableAssembly::setParent( AWT::DrawableAssembly* p )
{
   m_D->m_Parent = p;
   modified( );
}

AWT::DrawableAssembly* AWT::OpenGLDrawableAssembly::getParent( )
{
   return m_D->m_Parent;
}

void AWT::OpenGLDrawableAssembly::setVisible( const bool v )
{
   if ( m_D->m_Visible = v )
   {
      m_D->m_Visible = v;
      modified( );
   }
}

bool AWT::OpenGLDrawableAssembly::isVisible( ) const
{
   return m_D->m_Visible;
}

void AWT::OpenGLDrawableAssembly::getBounds( double* out_Bounds )
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
#endif 