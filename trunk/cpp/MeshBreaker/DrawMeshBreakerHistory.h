#ifndef __DRAWMESHBREAKERHISTORY_H__
#define __DRAWMESHBREAKERHISTORY_H__

#include "ObjectManagement/ManagedObject.h"

#include "OpenGLDrawables/OpenGLDrawableImpl.h"

#include "MeshBreaker.h"

namespace AWT
{
   template <class T>
   class DrawMeshBreakerHistory : public OpenGLDrawableImpl
   {
   public:
      typedef ManagedAutoPointer<DrawMeshBreakerHistory> P;

   protected:
      DrawMeshBreakerHistory( typename MeshBreaker<T>::P mb );
      virtual ~DrawMeshBreakerHistory( );

   public:
      static P getInstance( typename MeshBreaker<T>::P mb  );
      virtual std::string getClassName( ) const;

      virtual void setStart( MeshIndex v );
      virtual MeshIndex getStart( ) const;

      virtual void setEnd( MeshIndex v );
      virtual MeshIndex getEnd( ) const;

      virtual DrawMaterial::P getMaterial( );
      virtual void setMaterial( DrawMaterial::P mat );

      virtual void getBounds( double* bounds );

   protected:
      virtual void drawImpl( DrawContext::P context );

      struct D;
      D* m_D;

   };
}

#endif // __DRAWMESHBREAKERHISTORY_H__