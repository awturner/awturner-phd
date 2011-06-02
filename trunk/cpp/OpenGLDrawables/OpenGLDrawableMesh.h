#ifndef __OPENGLDRAWABLEMESH_H__
#define __OPENGLDRAWABLEMESH_H__
#pragma warning( disable: 4250 )

#include "OpenGLDrawDisplayList.h"

#include "DrawableFramework/DrawableMesh.h"

#include "DrawableFramework/DrawMaterial.h"

#include "Mesh/Mesh.h"

namespace AWT
{
   template <class T>
   class Mesh;

   class DrawMaterial;

   template <class T>
   class OpenGLDrawableMesh : public OpenGLDrawDisplayList, public virtual DrawableMesh<T>
   {
   public:
      typedef ManagedAutoPointer<OpenGLDrawableMesh<T>> P;

   protected:
      OpenGLDrawableMesh( typename Mesh<T>::P mesh );
      virtual ~OpenGLDrawableMesh( );

   public:
      static P getInstance( typename Mesh<T>::P mesh );

      ModifiedTime getChildModifiedTime( );

      virtual DrawMeshType getDrawAs( ) const;

      virtual void setDrawAs( const DrawMeshType drawAs );

      virtual std::string getClassName( ) const;

      virtual typename Mesh<T>::P getMesh( );

      virtual void setMesh( typename Mesh<T>::P mesh );

      virtual DrawMaterial::P getMaterial( );
      virtual void setMaterial( DrawMaterial::P mat );

      virtual float getLineWidth( ) const;
      virtual void setLineWidth( const float v );

      virtual float getPointSize( ) const;
      virtual void setPointSize( const float v );

      virtual bool isAutoNormals( ) const;
      virtual void setAutoNormals( const bool v );

      virtual bool isHideBackFaces( ) const;
      virtual void setHideBackFaces( const bool v );

   protected:
      void beforeCallList( DrawContext::P context );

      void afterCallList( DrawContext::P context );

      void buildList( DrawContext::P context );

      void updateBounds( );

      void updateBoundsForVertex( T* vtx );

      double getBoundImpl( unsigned int i );

   protected:
      struct D;
      D* m_D;
   };
}

#pragma warning( default: 4250 )
#endif // __OPENGLDRAWABLEMESH_H__