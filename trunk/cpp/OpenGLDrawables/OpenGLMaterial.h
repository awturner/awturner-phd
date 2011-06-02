#ifndef __OPENGLMATERIAL_H__
#define __OPENGLMATERIAL_H__

#include "DrawableFramework/DrawMaterial.h"

#include "OpenGLHeaders.h"

namespace AWT
{
   class OpenGLMaterial : public DrawMaterial
   {
   public:
      typedef ManagedAutoPointer<OpenGLMaterial> P;

   protected:
      OpenGLMaterial( const float* ambient, const float* diffuse, const float* specular, const float shininess, const float opacity );
      virtual ~OpenGLMaterial( );

   public:
      static P getInstance( const float* ambient, const float* diffuse, const float* specular, const float shininess, const float opacity = 1.f );

      virtual bool isOpaque( ) const;

      void prepare( );

      void unprepare( );

      void tweak( const TweakType tw, const MeshIndex v );

      virtual std::string getClassName( ) const;

   protected:
      struct D;
      D* m_D;
   };
}

#endif // __OPENGLDRAWMATERIAL_H__