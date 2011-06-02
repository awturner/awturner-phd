#ifndef __DRAWABLEIMPL_H__
#define __DRAWABLEIMPL_H__

#include "DrawableFramework/Drawable.h"

#include "DrawMaterial.h"

namespace AWT
{
   class DrawableAssembly;

   class DrawableImpl : public virtual Drawable
   {
   public:
      typedef ManagedAutoPointer<DrawableImpl> P;

   protected:
      DrawableImpl( );

      virtual ~DrawableImpl( );

   public:
      void draw( DrawContext::P context, const bool transparentPass );

      virtual bool isVisible( ) const;

      virtual void setVisible( const bool vis );

      virtual DrawMaterial::P getMaterial( ) = 0;

      virtual void setMaterial( DrawMaterial::P material ) = 0;

      virtual void setParent( DrawableAssembly* p );

      virtual DrawableAssembly* getParent( );

   protected:
      virtual void drawImpl( DrawContext::P context ) = 0;

      struct D;
      D* m_D;
   };
}

#endif // __DRAWABLEIMPL_H__