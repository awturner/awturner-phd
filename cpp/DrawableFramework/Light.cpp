#include "Light.h"

AWT::Light::P AWT::Light::getInstance( )
{
   AUTOGETINSTANCE( Light, ( ) );
}

AWT::Light::Light( )
{
}

AWT::Light::~Light( )
{
}

GETNAMEMACRO( AWT::Light );

void AWT::Light::setPosition( const float* position )
{
   for ( int i = 0; i < 4; ++i )
      m_Position[i] = position[i];
}

void AWT::Light::getPosition( float* out_Position ) const
{
   for ( int i = 0; i < 4; ++i )
      out_Position[i] = m_Position[i];
}

void AWT::Light::setAmbient( const float* ambient )
{
   for ( int i = 0; i < 4; ++i )
      m_Ambient[i] = ambient[i];
}

void AWT::Light::getAmbient( float* out_Ambient ) const
{
   for ( int i = 0; i < 4; ++i )
      out_Ambient[i] = m_Ambient[i];
}

void AWT::Light::setDiffuse( const float* diffuse )
{
   for ( int i = 0; i < 4; ++i )
      m_Diffuse[i] = diffuse[i];
}

void AWT::Light::getDiffuse( float* out_Diffuse ) const
{
   for ( int i = 0; i < 4; ++i )
      out_Diffuse[i] = m_Diffuse[i];
}

void AWT::Light::setSpecular( const float* specular )
{
   for ( int i = 0; i < 4; ++i )
      m_Specular[i] = specular[i];
}

void AWT::Light::getSpecular( float* out_Specular ) const
{
   for ( int i = 0; i < 4; ++i )
      out_Specular[i] = m_Specular[i];
}