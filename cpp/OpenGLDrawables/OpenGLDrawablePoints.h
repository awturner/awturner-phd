#ifndef __OPENGLDRAWABLEPOINTS_H__
#define __OPENGLDRAWABLEPOINTS_H__

#include "OpenGLDrawDisplayList.h"
#include "DrawableFramework/DrawablePoints.h"

#include "DrawableFramework/DrawMaterial.h"

#pragma warning( disable: 4250 )

namespace AWT
{
   template <class T>
   class OpenGLDrawablePoints : public OpenGLDrawDisplayList, public DrawablePoints<T>
   {
   public:
      typedef ManagedAutoPointer<OpenGLDrawablePoints<T>> P;

   protected:
      OpenGLDrawablePoints( typename Tuples<T>::P points );
      virtual ~OpenGLDrawablePoints( );

   public:
      static P getInstance( typename Tuples<T>::P points );
      virtual std::string getClassName( ) const;

      virtual typename Tuples<T>::P getData( );
      virtual void setData( typename Tuples<T>::P points );

      virtual typename Tuples<T>::P getNormals( );
      virtual void setNormals( typename Tuples<T>::P points );

      virtual float getPointSize( );
      virtual void setPointSize( const float s );

      virtual float getLineWidth( );
      virtual void setLineWidth( const float s );

      virtual bool isContiguous( ) const;
      virtual void setContiguous( const bool v );

      virtual bool isClosed( ) const;
      virtual void setClosed( const bool v );

      virtual ModifiedTime getChildModifiedTime( );

      virtual void buildList( DrawContext::P context );
         
      virtual void updateBounds( );

      virtual double getBoundImpl( unsigned int bound );

      virtual DrawMaterial::P getMaterial( );
      virtual void setMaterial( DrawMaterial::P mat );

   protected:
      struct D;
      D* m_D;

   };
}

#pragma warning( default: 4250 )
#endif // __OPENGLDRAWABLEPOINTS_H__