#include "OpenGLDrawableWorld.h"

#include "Useful/Mutex.h"

#include "DrawableFramework/DrawableBackground.h"
#include "DrawableFramework/Light.h"



#include "OpenGLHeaders.h"

#include "Useful/PrintMacros.h"
#include "Useful/PrintMacros.h"

#include "OpenGLDrawableFactory.h"

#include <limits>

struct AWT::OpenGLDrawableWorld::D
{
   void drawDefaultBackground( )
   {
      glDisable( GL_DEPTH_TEST );

      glMatrixMode( GL_PROJECTION );
      glPushMatrix( );
      glLoadIdentity( );

      glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

      glMatrixMode( GL_MODELVIEW );
      glPushMatrix( );
      glLoadIdentity( );

      glDisable( GL_LIGHTING );
      glEnable( GL_COLOR_MATERIAL );

      glBegin( GL_TRIANGLE_STRIP );

      if ( m_BackgroundMode == DrawableWorld::BACKGROUND_WHITE )
         glColor3f( 1.f, 1.f, 1.f );
      else
         glColor3f( 0.f, 0.f, 0.f );
      
      glVertex3f( -1.f, -1.f, 0.f );
      glVertex3f( -1.f,  1.f, 0.f );
      glVertex3f(  1.f, -1.f, 0.f );
      glVertex3f(  1.f,  1.f, 0.f );

      glEnd( );

      glMatrixMode( GL_PROJECTION );
      glPopMatrix( );

      glMatrixMode( GL_MODELVIEW );
      glPopMatrix( );

      glEnable( GL_DEPTH_TEST );
   }

   Drawable::P                      m_Background;
   Light::P                         m_Light;
   
   OpenGLDrawableFactory<double>::P m_Factory;

   //bool                             m_Visible;

   BackgroundMode                   m_BackgroundMode;
};

AWT::OpenGLDrawableWorld::OpenGLDrawableWorld( AWT::DrawContext::P ctx )
{
   m_D = new D;

   m_D->m_BackgroundMode = BACKGROUND_BLACK;

   m_D->m_Factory = OpenGLDrawableFactory<double>::getInstance( ctx );

   //m_D->m_Visible = true;
}

AWT::OpenGLDrawableWorld::~OpenGLDrawableWorld( )
{
   clear( );

   delete m_D;
}

AWT::OpenGLDrawableWorld::P AWT::OpenGLDrawableWorld::getInstance( AWT::DrawContext::P ctx )
{
   AUTOGETINSTANCE( OpenGLDrawableWorld, ( ctx ) );
}

GETNAMEMACRO( AWT::OpenGLDrawableWorld );

void AWT::OpenGLDrawableWorld::draw( AWT::DrawContext::P context, const bool transparentPass )
{
   glDisable( GL_FOG );

   if ( !transparentPass )
   {
      switch ( m_D->m_BackgroundMode )
      {
      case BACKGROUND_USER:
         if ( *m_D->m_Background != 0 )
         {
            m_D->m_Background->draw( context, transparentPass );
            break;
         }
      default:
         m_D->drawDefaultBackground( );
      }
   }

   if ( !transparentPass )
   {
      GLfloat density = 0.3f;
      GLfloat fogColor[] = { 0.f, 0.f, 0.f, 1.f };

      //glEnable( GL_FOG );
      glFogi( GL_FOG_MODE, GL_EXP2 );
      glFogfv( GL_FOG_COLOR, fogColor );
      glFogf( GL_FOG_DENSITY, density );
      glHint( GL_FOG_HINT, GL_NICEST );
   }

   AWT::DrawableAssembly::draw( context, transparentPass );
   /*
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
   */
}

/*
void AWT::OpenGLDrawableWorld::getBounds( double* out_Bounds )
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

void AWT::OpenGLDrawableWorld::setVisible( const bool v )
{
   this->m_D->m_Visible = v;
   modified( );
}

bool AWT::OpenGLDrawableWorld::isVisible( ) const
{
   return this->m_D->m_Visible;
}
*/

void AWT::OpenGLDrawableWorld::setLight( AWT::Light::P light )
{
   m_D->m_Light = light;
   modified( );
}

AWT::Light::P AWT::OpenGLDrawableWorld::getLight( )
{
   return m_D->m_Light;
}

void AWT::OpenGLDrawableWorld::setBackground( AWT::Drawable::P background )
{
   m_D->m_Background = background;
   setBackgroundMode( BACKGROUND_USER );
   modified( );
}

AWT::Drawable::P AWT::OpenGLDrawableWorld::getBackground( )
{
   return m_D->m_Background;
}

void AWT::OpenGLDrawableWorld::doLighting( )
{
   if ( *m_D->m_Light != 0 )
   {
      float ambient[4];
      float diffuse[4];
      float specular[4];
      float position[4];

      m_D->m_Light->getAmbient( ambient );
      m_D->m_Light->getDiffuse( diffuse );
      m_D->m_Light->getSpecular( specular );
      m_D->m_Light->getPosition( position );

      glEnable(GL_LIGHTING);
	   glEnable(GL_LIGHT0);
	   
	   // Assign created components to GL_LIGHT0
	   glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	   glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	   glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	   glLightfv(GL_LIGHT0, GL_POSITION, position);

      const GLfloat doTwoSide = 1;

      glLightModelfv( GL_LIGHT_MODEL_AMBIENT, ambient );
      glLightModelfv( GL_LIGHT_MODEL_TWO_SIDE, &doTwoSide );
   }  
}

AWT::DrawableFactory<double>* AWT::OpenGLDrawableWorld::getFactory( )
{
   return *m_D->m_Factory;
}

AWT::DrawableWorld::BackgroundMode AWT::OpenGLDrawableWorld::getBackgroundMode( ) const
{
   return m_D->m_BackgroundMode;
}

void AWT::OpenGLDrawableWorld::setBackgroundMode( const AWT::DrawableWorld::BackgroundMode mode )
{
   m_D->m_BackgroundMode = mode;
}