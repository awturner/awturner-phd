#ifndef __DRAWABLELIGHT_H__
#define __DRAWABLELIGHT_H__

#include "Drawable.h"

#include "Light.h"

namespace AWT
{
   class DrawableLight : public virtual Drawable
   {
   public:
      typedef ManagedAutoPointer<DrawableLight> P;

   public:
      virtual Light::P getLight( ) = 0;

      virtual void setLight( Light::P light ) = 0;
   };
}

#endif // __DRAWABLELIGHT_H__