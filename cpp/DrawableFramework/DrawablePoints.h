#ifndef __DRAWABLEPOINTS_H__
#define __DRAWABLEPOINTS_H__

#include "Drawable.h"

#include "DrawMaterial.h"

#include "Mesh/Tuples.h"

namespace AWT
{
   template <class T>
   class DrawablePoints : public virtual Drawable
   {  
   public:
      typedef ManagedAutoPointer<DrawablePoints<T>> P;

      virtual typename Tuples<T>::P getData( ) = 0;
      virtual void setData( typename Tuples<T>::P points ) = 0;

      virtual typename Tuples<T>::P getNormals( ) = 0;
      virtual void setNormals( typename Tuples<T>::P points ) = 0;

      virtual float getPointSize( ) = 0;
      virtual void setPointSize( const float s ) = 0;

      virtual float getLineWidth( ) = 0;
      virtual void setLineWidth( const float s ) = 0;

      virtual bool isContiguous( ) const = 0;
      virtual void setContiguous( const bool v ) = 0;

      virtual bool isClosed( ) const = 0;
      virtual void setClosed( const bool v ) = 0;

      virtual DrawMaterial::P getMaterial( ) = 0;
      virtual void setMaterial( DrawMaterial::P mat ) = 0;
   };
}

#endif // __DRAWABLEPOINTS_H__