#ifndef __OPENGLDRAWABLEQUIVER_H__
#define __OPENGLDRAWABLEQUIVER_H__

#include "DrawableFramework/DrawableQuiver.h"
#include "OpenGLDrawDisplayList.h"

#pragma warning( disable: 4250 )

namespace AWT
{
   template <class T>
   class OpenGLDrawableQuiver : public OpenGLDrawDisplayList, public DrawableQuiver<T>
   {
   public:
      typedef ManagedAutoPointer<OpenGLDrawableQuiver> P;

   protected:
      OpenGLDrawableQuiver( typename Tuples<T>::P points, typename Tuples<T>::P points2 );
      virtual ~OpenGLDrawableQuiver( );

   public:
      static P getInstance( typename Tuples<T>::P points, typename Tuples<T>::P points2 );
      virtual std::string getClassName( ) const;

      virtual typename Tuples<T>::P getData( );
      virtual void setData( typename Tuples<T>::P points );

      virtual typename Tuples<T>::P getData2( );
      virtual void setData2( typename Tuples<T>::P points );

      virtual bool isRelative( ) const;
      virtual void setRelative( const bool v );

      virtual float getPointSize( );
      virtual void setPointSize( const float s );

      virtual float getLineWidth( );
      virtual void setLineWidth( const float s );

      virtual DrawMaterial::P getMaterial( );
      virtual void setMaterial( DrawMaterial::P mat );

      virtual ModifiedTime getChildModifiedTime( );

      virtual void buildList( DrawContext::P context );

      virtual void updateBounds( );

      virtual double getBoundImpl( unsigned int bound );

   protected:
      struct D;
      D* m_D;

   };
}

#endif // __OPENGLDRAWABLEQUIVER_H__