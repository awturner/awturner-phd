#ifndef __OPENGLDRAWABLEMESHNORMALS_H__
#define __OPENGLDRAWABLEMESHNORMALS_H__
#pragma warning( disable: 4250 )

#include "OpenGLDrawDisplayList.h"

#include "DrawableFramework/DrawableMeshNormals.h"

#include "DrawableFramework/DrawMaterial.h"

#include "Mesh/Mesh.h"

namespace AWT
{
   template <class T>
   class OpenGLDrawableMeshNormals : public OpenGLDrawDisplayList, public virtual DrawableMeshNormals<T>
   {
   public:
      typedef ManagedAutoPointer<OpenGLDrawableMeshNormals<T>> P;

   protected:
      OpenGLDrawableMeshNormals( typename Mesh<T>::P mesh );
      virtual ~OpenGLDrawableMeshNormals( );

   public:
      static P getInstance( typename Mesh<T>::P mesh );

      ModifiedTime getChildModifiedTime( );

      virtual std::string getClassName( ) const;

      virtual typename Mesh<T>::P getMesh( );

      virtual void setMesh( typename Mesh<T>::P mesh );

      virtual DrawNormalsType getDrawNormalsType( ) const;
      virtual void setDrawNormalsType( const DrawNormalsType da );

      virtual T getScale( ) const;
      virtual void setScale( const T scale );

      virtual DrawMaterial::P getMaterial( );
      virtual void setMaterial( DrawMaterial::P mat );

   protected:
      void buildList( DrawContext::P context );

      void updateBounds( );

      double getBoundImpl( unsigned int i );

   protected:
      struct D;
      D* m_D;
   };
}

#pragma warning( default: 4250 )
#endif // __OPENGLDRAWABLEMESHNORMALS_H__