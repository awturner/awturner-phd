#ifndef __OPENGLTEXTURE_H__
#define __OPENGLTEXTURE_H__

#include "DrawableFramework/DrawMaterial.h"

namespace AWT
{
   class OpenGLTexture : public DrawMaterial
   {
   public:
      typedef ManagedAutoPointer<OpenGLTexture> P;

   protected:
      OpenGLTexture( const unsigned char* data, unsigned int w, unsigned int h, bool lighting = true );

      virtual ~OpenGLTexture( );

   public:
      static P getInstance( const unsigned char* data, unsigned int w, unsigned int h, bool lighting = true );

      virtual std::string getClassName( ) const;

      virtual bool isOpaque( ) const;

      virtual void prepare( );

      virtual void unprepare( );

      virtual void tweak( const TweakType tw, const MeshIndex v );

      bool isLighting( ) const;

      void setLighting( const bool v );

   protected:
      struct D;
      D* m_D;
   };
}

#endif // __OPENGLTEXTURE_H__