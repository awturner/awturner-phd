#ifndef __DRAWAXIS_H__
#define __DRAWAXIS_H__

#include "Drawable.h"

namespace AWT
{
   class DrawAxis : public Drawable
   {
   public:
      typedef ManagedAutoPointer<DrawAxis> P;

   public:
      virtual void draw( DrawContext::P context, const bool transparentPass ) = 0;

      virtual void setVisible( const bool v ) = 0;

      virtual bool isVisible( ) const = 0;

      virtual void getBounds( double* out_Bounds ) = 0;

      virtual void setParent( DrawableAssembly* p ) = 0;
      virtual DrawableAssembly* getParent( ) = 0;
   };
}

#endif // __DRAWAXIS_H__