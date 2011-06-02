#ifndef __OPENGLDRAWABLECORRESPONDENCES_H__
#define __OPENGLDRAWABLECORRESPONDENCES_H__

#include "OpenGLDrawDisplayList.h"

#include "NonRigidAlignment/CorrespondenceFinder.h"

namespace AWT
{
   class DrawMaterial;

   template <class T>
   class OpenGLDrawableCorrespondences : public OpenGLDrawDisplayList
   {
   public:
      typedef ManagedAutoPointer<OpenGLDrawableCorrespondences> P;

   protected:
      OpenGLDrawableCorrespondences( typename CorrespondenceFinder<T>::P corr );
      virtual ~OpenGLDrawableCorrespondences( );

   public:
      static P getInstance( typename CorrespondenceFinder<T>::P corr );

      virtual std::string getClassName( ) const;

      virtual void setCorrespondences( typename CorrespondenceFinder<T>::P corr );

   protected:
      virtual ModifiedTime getChildModifiedTime( );

      virtual void buildList( AWT::DrawContext::P context );
         
      virtual void updateBounds( );

      virtual void updateBoundsForVertex( T* vtx );

      virtual double getBoundImpl( unsigned int bound );

      virtual DrawMaterial::P getMaterial( );
      virtual void setMaterial( DrawMaterial::P mat );

   protected:
      struct D;
      D* m_D;
   };
}

#endif __OPENGLDRAWABLECORRESPONDENCES_H__