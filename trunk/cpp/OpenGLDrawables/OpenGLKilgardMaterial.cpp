#include "OpenGLKilgardMaterial.h"

#include "DrawableFramework/DrawMaterial.h"
#include "OpenGLHeaders.h"



struct AWT::OpenGLKilgardMaterial::D
{
   static float data[];

   enum OpenGLKilgardMaterial
   {
      emerald = 1,
      jade = 2,
      obsidian = 3,
      pearl = 4,
      ruby = 5,
      turquoise = 6,
      brass = 7,
      bronze = 8,
      chrome = 9,
      copper = 10,
      gold = 11,
      silver = 12,
      black_plastic = 13,
      cyan_plastic = 14,
      green_plastic = 15,
      red_plastic = 16,
      white_plastic = 17,
      yellow_plastic = 18,
      black_rubber = 19,
      cyan_rubber = 20,
      green_rubber = 21,
      red_rubber = 22,
      white_rubber = 23,
      yellow_rubber = 24,
      bone = 25,
      red = 26,
      yellow = 27,
   };

   OpenGLKilgardMaterial m_Mat;
   float m_Opacity;
};

float AWT::OpenGLKilgardMaterial::D::data[] = {
   0.0215f,   0.1745f,   0.0215f,   0.07568f,  0.61424f,    0.07568f,    0.633f,      0.727811f,   0.633f,      0.6f,        
   0.135f,    0.2225f,   0.1575f,   0.54f,     0.89f,       0.63f,       0.316228f,   0.316228f,   0.316228f,   0.1f,        
   0.05375f,  0.05f,     0.06625f,  0.18275f,  0.17f,       0.22525f,    0.332741f,   0.328634f,   0.346435f,   0.3f,        
   0.25f,     0.20725f,  0.20725f,  1.f,       0.829f,      0.829f,      0.296648f,   0.296648f,   0.296648f,   0.088f,      
   0.1745f,   0.01175f,  0.01175f,  0.61424f,  0.04136f,    0.04136f,    0.727811f,   0.626959f,   0.626959f,   0.6f,        
   0.1f,      0.18725f,  0.1745f,   0.396f,    0.74151f,    0.69102f,    0.297254f,   0.30829f,    0.306678f,   0.1f,        
   0.329412f, 0.223529f, 0.027451f, 0.780392f, 0.568627f,   0.113725f,   0.992157f,   0.941176f,   0.807843f,   0.21794872f, 
   0.2125f,   0.1275f,   0.054f,    0.714f,    0.4284f,     0.18144f,    0.393548f,   0.271906f,   0.166721f,   0.2f,        
   0.25f,     0.25f,     0.25f,     0.4f,      0.4f,        0.4f,        0.774597f,   0.774597f,   0.774597f,   0.6f,        
   0.19125f,  0.0735f,   0.0225f,   0.7038f,   0.27048f,    0.0828f,     0.256777f,   0.137622f,   0.086014f,   0.1f,        
   0.24725f,  0.1995f,   0.0745f,   0.75164f,  0.60648f,    0.22648f,    0.628281f,   0.555802f,   0.366065f,   0.4f,        
   0.19225f,  0.19225f,  0.19225f,  0.50754f,  0.50754f,    0.50754f,    0.508273f,   0.508273f,   0.508273f,   0.4f,        
   0.f,       0.f,       0.f,       0.01f,     0.01f,       0.01f,       0.5f,        0.5f,        0.5f,        0.25f,       
   0.f,       0.1f,      0.06f,     0.f,       0.50980392f, 0.50980392f, 0.50196078f, 0.50196078f, 0.50196078f, 0.25f,       
   0.f,       0.f,       0.f,       0.1f,      0.35f,       0.1f,        0.45f,       0.55f,       0.45f,       0.25f,       
   0.f,       0.f,       0.f,       0.5f,      0.f,         0.f,         0.7f,        0.6f,        0.6f,        0.25f,       
   0.f,       0.f,       0.f,       0.55f,     0.55f,       0.55f,       0.7f,        0.7f,        0.7f,        0.25f,       
   0.f,       0.f,       0.f,       0.5f,      0.5f,        0.f,         0.6f,        0.6f,        0.5f,        0.25f,       
   0.02f,     0.02f,     0.02f,     0.01f,     0.01f,       0.01f,       0.4f,        0.4f,        0.4f,        0.078125f,   
   0.f,       0.05f,     0.05f,     0.4f,      0.5f,        0.5f,        0.04f,       0.7f,        0.7f,        0.078125f,   
   0.f,       0.05f,     0.f,       0.4f,      0.5f,        0.4f,        0.04f,       0.7f,        0.04f,       0.078125f,   
   0.05f,     0.f,       0.f,       0.5f,      0.4f,        0.4f,        0.7f,        0.04f,       0.04f,       0.078125f,   
   0.05f,     0.05f,     0.05f,     0.5f,      0.5f,        0.5f,        0.7f,        0.7f,        0.7f,        0.078125f,   
   0.05f,     0.05f,     0.f,       0.5f,      0.5f,        0.4f,        0.7f,        0.7f,        0.04f,       0.078125f,   

   // Bone
   0.f, 0.f, 0.f, 1.f, 1.f, 0.8f, 0.f, 0.f, 0.f, 1.0f, // Bone
   
   0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 1.f, 1.f, .6f, // Red
   0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 1.f, .6f, // Yellow
};

AWT::OpenGLKilgardMaterial::OpenGLKilgardMaterial( const std::string& name, const float opacity )
{
   m_D = new D;

   m_D->m_Opacity = std::max<float>( 0, std::min<float>( 1, opacity ) );

   if ( !strcmp( name.c_str( ),"kilgard::emerald" ) )
      m_D->m_Mat = D::emerald;
   else if  ( !strcmp( name.c_str( ), "kilgard::jade" ) )
      m_D->m_Mat = D::jade;
   else if  ( !strcmp( name.c_str( ), "kilgard::obsidian" ) )
      m_D->m_Mat = D::obsidian;
   else if  ( !strcmp( name.c_str( ), "kilgard::pearl" ) )
      m_D->m_Mat = D::pearl;
   else if  ( !strcmp( name.c_str( ), "kilgard::ruby" ) )
      m_D->m_Mat = D::ruby;
   else if  ( !strcmp( name.c_str( ), "kilgard::turquoise" ) )
      m_D->m_Mat = D::turquoise;
   else if  ( !strcmp( name.c_str( ), "kilgard::brass" ) )
      m_D->m_Mat = D::brass;
   else if  ( !strcmp( name.c_str( ), "kilgard::bronze" ) )
      m_D->m_Mat = D::bronze;
   else if  ( !strcmp( name.c_str( ), "kilgard::chrome" ) )
      m_D->m_Mat = D::chrome;
   else if  ( !strcmp( name.c_str( ), "kilgard::copper" ) )
      m_D->m_Mat = D::copper;
   else if  ( !strcmp( name.c_str( ), "kilgard::gold" ) )
      m_D->m_Mat = D::gold;
   else if  ( !strcmp( name.c_str( ), "kilgard::silver" ) )
      m_D->m_Mat = D::silver;
   else if  ( !strcmp( name.c_str( ), "kilgard::black_plastic" ) )
      m_D->m_Mat = D::black_plastic;
   else if  ( !strcmp( name.c_str( ), "kilgard::cyan_plastic" ) )
      m_D->m_Mat = D::cyan_plastic;
   else if  ( !strcmp( name.c_str( ), "kilgard::green_plastic" ) )
      m_D->m_Mat = D::green_plastic;
   else if  ( !strcmp( name.c_str( ), "kilgard::red_plastic" ) )
      m_D->m_Mat = D::red_plastic;
   else if  ( !strcmp( name.c_str( ), "kilgard::white_plastic" ) )
      m_D->m_Mat = D::white_plastic;
   else if  ( !strcmp( name.c_str( ), "kilgard::yellow_plastic" ) )
      m_D->m_Mat = D::yellow_plastic;
   else if  ( !strcmp( name.c_str( ), "kilgard::black_rubber" ) )
      m_D->m_Mat = D::black_rubber;
   else if  ( !strcmp( name.c_str( ), "kilgard::cyan_rubber" ) )
      m_D->m_Mat = D::cyan_rubber;
   else if  ( !strcmp( name.c_str( ), "kilgard::green_rubber" ) )
      m_D->m_Mat = D::green_rubber;
   else if  ( !strcmp( name.c_str( ), "kilgard::red_rubber" ) )
      m_D->m_Mat = D::red_rubber;
   else if  ( !strcmp( name.c_str( ), "kilgard::white_rubber" ) )
      m_D->m_Mat = D::white_rubber;
   else if  ( !strcmp( name.c_str( ), "kilgard::yellow_rubber" ) )
      m_D->m_Mat = D::yellow_rubber;
   else if  ( !strcmp( name.c_str( ), "kilgard::bone" ) )
      m_D->m_Mat = D::bone;
   else if  ( !strcmp( name.c_str( ), "kilgard::red" ) )
      m_D->m_Mat = D::red;
   else if  ( !strcmp( name.c_str( ), "kilgard::yellow" ) )
      m_D->m_Mat = D::yellow;
}

AWT::OpenGLKilgardMaterial::~OpenGLKilgardMaterial( )
{
   delete m_D;
}

AWT::OpenGLKilgardMaterial::P AWT::OpenGLKilgardMaterial::getInstance( const std::string& name, const float opacity )
{
   AUTOGETINSTANCE( OpenGLKilgardMaterial, ( name, opacity ) );
}

bool AWT::OpenGLKilgardMaterial::isOpaque( ) const
{
   return m_D->m_Opacity == 1.f;
}

void AWT::OpenGLKilgardMaterial::prepare( )
{
   glEnable( GL_LIGHTING );

   unsigned int offset = (m_D->m_Mat - 1)*10;

   glDisable( GL_COLOR_MATERIAL );

   float mat[4];

   mat[0] = D::data[offset+0];
   mat[1] = D::data[offset+1];
   mat[2] = D::data[offset+2];
   mat[3] = m_D->m_Opacity;
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat);

   mat[0] = D::data[offset+3];
   mat[1] = D::data[offset+4];
   mat[2] = D::data[offset+5];
   glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat);

   mat[0] = D::data[offset+6];
   mat[1] = D::data[offset+7];
   mat[2] = D::data[offset+8];
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat);

   mat[0] = D::data[offset+9];
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mat[0] * 128.f);

   glShadeModel(GL_SMOOTH);
}

void AWT::OpenGLKilgardMaterial::unprepare( )
{
}

void AWT::OpenGLKilgardMaterial::tweak( const TweakType /*tw*/, const AWT::MeshIndex /*v*/ )
{
}

GETNAMEMACRO( AWT::OpenGLKilgardMaterial );
