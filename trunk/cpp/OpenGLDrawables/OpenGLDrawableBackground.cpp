#include "OpenGLDrawableBackground.h"

#include "OpenGLHeaders.h"

#include <limits>


#include "DrawableFramework/DrawMaterial.h"

struct AWT::OpenGLDrawableBackground::D
{
   DrawMaterial::P m_Material;
   float m_BottomCol[3];
   float m_TopCol[3];
   double m_Bounds[6];
};

AWT::OpenGLDrawableBackground::OpenGLDrawableBackground( )
{
   m_D = new D;

   for ( int i = 0; i < 3; ++i )
   {
      m_D->m_BottomCol[i] = m_D->m_TopCol[i] = 0;
   }
}

AWT::OpenGLDrawableBackground::~OpenGLDrawableBackground( )
{
   delete m_D;
}

AWT::OpenGLDrawableBackground::P AWT::OpenGLDrawableBackground::getInstance( )
{
   AUTOGETINSTANCE( AWT::OpenGLDrawableBackground, ( ) );
}

GETNAMEMACRO( AWT::OpenGLDrawableBackground );

void AWT::OpenGLDrawableBackground::buildList( AWT::DrawContext::P context )
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

   glColor3fv( m_D->m_BottomCol );
   glVertex3f( -1.f, -1.f, 0.f );

   glColor3fv( m_D->m_TopCol );
   glVertex3f( -1.f,  1.f, 0.f );

   glColor3fv( m_D->m_BottomCol );
   glVertex3f(  1.f, -1.f, 0.f );

   glColor3fv( m_D->m_TopCol );
   glVertex3f(  1.f,  1.f, 0.f );

   glEnd( );

   glMatrixMode( GL_PROJECTION );
   glPopMatrix( );

   glMatrixMode( GL_MODELVIEW );
   glPopMatrix( );

   glEnable( GL_DEPTH_TEST );
}

AWT::ModifiedTime AWT::OpenGLDrawableBackground::getChildModifiedTime( )
{
   return getTimeObjectModified( );
}

void AWT::OpenGLDrawableBackground::updateBounds( )
{
   for ( int ax = 0; ax < 3; ++ax )
   {
      m_D->m_Bounds[2*ax+0] = std::numeric_limits<double>::quiet_NaN( );
      m_D->m_Bounds[2*ax+1] = std::numeric_limits<double>::quiet_NaN( );
   }
}

double AWT::OpenGLDrawableBackground::getBoundImpl( unsigned int bound )
{
   return std::numeric_limits<double>::quiet_NaN( );
}

void AWT::OpenGLDrawableBackground::getTopColor( float* col ) const
{
   col[0] = m_D->m_TopCol[0];
   col[1] = m_D->m_TopCol[1];
   col[2] = m_D->m_TopCol[2];
}

void AWT::OpenGLDrawableBackground::setTopColor( const float* col )
{
   m_D->m_TopCol[0] = col[0];
   m_D->m_TopCol[1] = col[1];
   m_D->m_TopCol[2] = col[2];
}

void AWT::OpenGLDrawableBackground::getBottomColor( float* col ) const
{
   col[0] = m_D->m_BottomCol[0];
   col[1] = m_D->m_BottomCol[1];
   col[2] = m_D->m_BottomCol[2];
}

void AWT::OpenGLDrawableBackground::setBottomColor( const float* col )
{
   m_D->m_BottomCol[0] = col[0];
   m_D->m_BottomCol[1] = col[1];
   m_D->m_BottomCol[2] = col[2];
}

void AWT::OpenGLDrawableBackground::setMaterial(AWT::DrawMaterial::P mat)
{
   m_D->m_Material = mat;
}

AWT::DrawMaterial::P AWT::OpenGLDrawableBackground::getMaterial( )
{
   return m_D->m_Material;
}