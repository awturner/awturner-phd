#ifndef __OPENGLDRAWABLEAABB_H__
#define __OPENGLDRAWABLEAABB_H__

#include "OpenGLDrawDisplayList.h"

#include "ObjectEKDTree/AxisAlignedBoundingBox.h"

namespace AWT
{
   template <class T, unsigned char K>
   class AxisAlignedBoundingBox;

   template <class T>
   class OpenGLDrawableAABB : public OpenGLDrawDisplayList
   {
   public:
      typedef ManagedAutoPointer<OpenGLDrawableAABB<T>> P;

   protected:
      OpenGLDrawableAABB( typename AxisAlignedBoundingBox<T,3>::P box );
      virtual ~OpenGLDrawableAABB( );

   public:
      static P getInstance( typename AxisAlignedBoundingBox<T,3>::P box );
      virtual std::string getClassName( ) const;

      AxisAlignedBoundingBox<T,3>* getBox( );

      ModifiedTime getChildModifiedTime( );

      void buildList( AWT::DrawContext::P context );

      void updateBounds( );

      double getBoundImpl(unsigned int bound);

      virtual DrawMaterial::P getMaterial( );
      virtual void setMaterial( DrawMaterial::P mat );

   protected:
      struct D;
      D* m_D;

   };
}

#endif // __OPENGLDRAWABLEAABB_H__