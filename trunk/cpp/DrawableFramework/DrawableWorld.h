#ifndef __DRAWABLEWORLD_H__
#define __DRAWABLEWORLD_H__

#include "DrawableAssembly.h"
#include "Light.h"

namespace AWT
{
   class Light;
   
   template <class T>
   class DrawableFactory;

   class DrawableWorld : public virtual DrawableAssembly
   {
   public:
      typedef ManagedAutoPointer<DrawableWorld> P;

   public:
      enum BackgroundMode { BACKGROUND_USER, BACKGROUND_WHITE, BACKGROUND_BLACK };

   public:
      virtual void setLight( Light::P light ) = 0;
      virtual Light::P getLight( ) = 0;

      virtual void setBackground( Drawable::P background ) = 0;
      virtual Drawable::P getBackground( ) = 0;
   
      virtual BackgroundMode getBackgroundMode( ) const = 0;
      virtual void setBackgroundMode( const BackgroundMode mode ) = 0;

      virtual DrawableFactory<double>* getFactory( ) = 0;

      virtual void doLighting( ) = 0;
   };
}

#endif // __DRAWABLEWORLD_H__