#include "OpenGLSimpleColour.h"

#include "OpenGLHeaders.h"

struct AWT::OpenGLSimpleColour::D
{
   float m_Col[4];
   bool  m_Lighting;
};

AWT::OpenGLSimpleColour::OpenGLSimpleColour( float r, float g, float b, float a, bool lighting )
{
   m_D = new D;

   setColour( r, g, b, a );
   setLighting( lighting );
}

AWT::OpenGLSimpleColour::~OpenGLSimpleColour( )
{
   delete m_D;
}

AWT::OpenGLSimpleColour::P AWT::OpenGLSimpleColour::getInstance( float r, float g, float b, float a, bool lighting  )
{
   AUTOGETINSTANCE( OpenGLSimpleColour, ( r, g, b, a, lighting ) );
}

GETNAMEMACRO( AWT::OpenGLSimpleColour );

void AWT::OpenGLSimpleColour::prepare( )
{
   if ( m_D->m_Lighting )
      glEnable( GL_LIGHTING );
   else
      glDisable( GL_LIGHTING );

   glEnable( GL_COLOR_MATERIAL );

   GLfloat ambient[]   = { 0.2f, 0.2f, 0.2f, 1.0f };
   GLfloat diffuse[]   = { 0.8f, 0.8f, 0.8f, 1.0f };
   GLfloat specular[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
   GLfloat shininess[] = { 0.0f };

   glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE,   ambient );
   glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE,   diffuse );
   glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR,  specular );
   glMaterialfv( GL_FRONT_AND_BACK, GL_SHININESS, shininess );

   glColor4fv( m_D->m_Col );
}

void AWT::OpenGLSimpleColour::unprepare( )
{
}

void AWT::OpenGLSimpleColour::tweak( const TweakType /*tw*/, AWT::MeshIndex /*v*/ )
{
}

void AWT::OpenGLSimpleColour::setColour( const float* col )
{
   for ( int i = 0; i < 4; ++i )
      m_D->m_Col[i] = col[i];
}

void AWT::OpenGLSimpleColour::setColour( const float r, const float g, const float b, const float a )
{
   m_D->m_Col[0] = r;
   m_D->m_Col[1] = g;
   m_D->m_Col[2] = b;
   m_D->m_Col[3] = a;
}

void AWT::OpenGLSimpleColour::getColour( float* col ) const
{
   for ( int i = 0; i < 4; ++i )
      col[i] = m_D->m_Col[i];
}

bool AWT::OpenGLSimpleColour::isLighting( ) const
{
   return m_D->m_Lighting;
}

void AWT::OpenGLSimpleColour::setLighting( const bool v )
{
   m_D->m_Lighting = v;
}

bool AWT::OpenGLSimpleColour::isOpaque() const
{
   return m_D->m_Col[3] == 1.f;
}