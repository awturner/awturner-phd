#ifndef __DRAWABLEQUIVER_H__
#define __DRAWABLEQUIVER_H__

#include "Drawable.h"

#include "DrawMaterial.h"

#include "Mesh/Tuples.h"

namespace AWT
{
   template <class T>
   class DrawableQuiver : public virtual Drawable
   {
   public:
      typedef ManagedAutoPointer<DrawableQuiver<T>> P;

      virtual typename Tuples<T>::P getData( ) = 0;
      virtual void setData( typename Tuples<T>::P points ) = 0;

      virtual typename Tuples<T>::P getData2( ) = 0;
      virtual void setData2( typename Tuples<T>::P points ) = 0;

      virtual bool isRelative( ) const = 0;
      virtual void setRelative( const bool v ) = 0;

      virtual float getPointSize( ) = 0;
      virtual void setPointSize( const float s ) = 0;

      virtual float getLineWidth( ) = 0;
      virtual void setLineWidth( const float s ) = 0;

      virtual DrawMaterial::P getMaterial( ) = 0;
      virtual void setMaterial( DrawMaterial::P mat ) = 0;
   };
}

#endif // __DRAWABLEQUIVER_H__