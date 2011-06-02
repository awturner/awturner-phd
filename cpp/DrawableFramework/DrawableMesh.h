#ifndef __DRAWABLEMESH_H__
#define __DRAWABLEMESH_H__

#include "Drawable.h"

#include "Mesh/Mesh.h"

#include "DrawMaterial.h"

namespace AWT
{
   enum DrawMeshType { DRAWAS_POINTS, DRAWAS_LINES, DRAWAS_SOLID };

   template <class T>
   class DrawableMesh : public virtual Drawable
   {
   public:
      typedef ManagedAutoPointer<DrawableMesh<T>> P;

   public:
      virtual typename Mesh<T>::P getMesh( ) = 0;
      virtual void setMesh( typename Mesh<T>::P mesh ) = 0;

      virtual DrawMeshType getDrawAs( ) const = 0;
      virtual void setDrawAs( const DrawMeshType da ) = 0;

      virtual float getLineWidth( ) const = 0;
      virtual void  setLineWidth( const float v ) = 0;

      virtual float getPointSize( ) const = 0;
      virtual void setPointSize( const float v ) = 0;

      virtual DrawMaterial::P getMaterial( ) = 0;
      virtual void setMaterial( DrawMaterial::P mat ) = 0;

      virtual bool isAutoNormals( ) const = 0;
      virtual void setAutoNormals( const bool v ) = 0;

      virtual bool isHideBackFaces( ) const = 0;
      virtual void setHideBackFaces( const bool v ) = 0;
   };
}

#endif // __DRAWABLEMESH_H__