#include "OpenGLMaterial.h"

struct AWT::OpenGLMaterial::D
{
   float m_Ambient[4];
   float m_Diffuse[4];
   float m_Specular[4];
   float m_Shininess;
   float m_Opacity;
};

AWT::OpenGLMaterial::OpenGLMaterial( const float* ambient, const float* diffuse, const float* specular, const float shininess, const float opacity )
{
   m_D = new D( );

   m_D->m_Shininess = 128.f * shininess;
   m_D->m_Opacity   = opacity;

   for ( unsigned int i = 0; i < 3; ++i )
   {
      m_D->m_Ambient[i]  = ambient[i];
      m_D->m_Diffuse[i]  = diffuse[i];
      m_D->m_Specular[i] = specular[i];
   }

   // Keep the opacity too
   m_D->m_Ambient[3]  = opacity;
   m_D->m_Diffuse[3]  = opacity;
   m_D->m_Specular[3] = opacity;
}

AWT::OpenGLMaterial::~OpenGLMaterial( )
{
   delete m_D;
}

AWT::OpenGLMaterial::P AWT::OpenGLMaterial::getInstance( const float* ambient, const float* diffuse, const float* specular, const float shininess, const float opacity )
{
   AUTOGETINSTANCE( AWT::OpenGLMaterial, ( ambient, diffuse, specular, shininess, opacity ) );
}

GETNAMEMACRO( AWT::OpenGLMaterial );

bool AWT::OpenGLMaterial::isOpaque( ) const
{
   return m_D->m_Opacity == 1.f;
}

void AWT::OpenGLMaterial::prepare( )
{
   glEnable( GL_LIGHTING );

   glDisable( GL_COLOR_MATERIAL );

   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, m_D->m_Ambient);
   glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, m_D->m_Diffuse);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, m_D->m_Specular);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, m_D->m_Shininess);

   glShadeModel(GL_SMOOTH);
}

void AWT::OpenGLMaterial::unprepare( )
{
}

void AWT::OpenGLMaterial::tweak( const TweakType tw, const MeshIndex v )
{
}
