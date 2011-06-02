#ifndef __OPENGLDRAWMATERIAL_H__
#define __OPENGLDRAWMATERIAL_H__

#include "DrawableFramework/DrawMaterial.h"

#include "OpenGLHeaders.h"

namespace AWT
{
   class OpenGLKilgardMaterial : public DrawMaterial
   {
   public:
      typedef ManagedAutoPointer<OpenGLKilgardMaterial> P;

   protected:
      OpenGLKilgardMaterial( const std::string& name, const float opacity );
      virtual ~OpenGLKilgardMaterial( );

   public:
      static P getInstance( const std::string& name, const float opacity = 1.f );

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