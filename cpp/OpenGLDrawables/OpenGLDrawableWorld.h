#ifndef __OPENGLDRAWABLEWORLD_H__
#define __OPENGLDRAWABLEWORLD_H__
#pragma warning( disable: 4250 )

#include "DrawableFramework/DrawableAssembly.h"
#include "DrawableFramework/DrawableWorld.h"

namespace AWT
{
   class OpenGLDrawableWorld : public virtual DrawableWorld
   {
   public:
      typedef ManagedAutoPointer<OpenGLDrawableWorld> P;

   protected:
      OpenGLDrawableWorld( AWT::DrawContext::P ctx );

      ~OpenGLDrawableWorld( );

   public:
      static P getInstance( AWT::DrawContext::P ctx );

      virtual void draw( DrawContext::P context, const bool transparentPass );

      /*
      void getBounds( double* out_Bounds );

      void setVisible( const bool v );

      bool isVisible( ) const;
      */

      void setLight( Light::P light );
      Light::P getLight( );

      void setBackground( Drawable::P background );
      Drawable::P getBackground( );

      virtual BackgroundMode getBackgroundMode( ) const;
      virtual void setBackgroundMode( const BackgroundMode mode );

      void doLighting( );

      virtual std::string getClassName( ) const;

      virtual DrawableFactory<double>* getFactory( );

   protected:
      struct D;
      D* m_D;
   };
}

#endif // __OPENGLDRAWABLEWORLD_H__