#ifndef __DRAWABLEMESHNORMALS_H__
#define __DRAWABLEMESHNORMALS_H__

#include "Drawable.h"
#include "DrawMaterial.h"

#include "Mesh/Mesh.h"

namespace AWT
{
   enum DrawNormalsType { NORMALS_VERTEX, NORMALS_FACE };

   template <class T>
   class DrawableMeshNormals : public virtual Drawable
   {
   public:
      typedef ManagedAutoPointer<DrawableMeshNormals<T>> P;

   public:
      virtual typename Mesh<T>::P getMesh( ) = 0;
      virtual void setMesh( typename Mesh<T>::P mesh ) = 0;

      virtual DrawNormalsType getDrawNormalsType( ) const = 0;
      virtual void setDrawNormalsType( const DrawNormalsType da ) = 0;

      virtual T getScale( ) const = 0;
      virtual void setScale( const T scale ) = 0;

      virtual DrawMaterial::P getMaterial( ) = 0;
      virtual void setMaterial( DrawMaterial::P mat ) = 0;

   protected:
      struct D;
      D* m_D;

   };
}

#endif // __DRAWABLEMESHNORMALS_H__