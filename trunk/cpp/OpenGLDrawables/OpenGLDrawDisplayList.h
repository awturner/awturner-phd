#ifndef __OPENGLDRAWDISPLAYLIST_H__
#define __OPENGLDRAWDISPLAYLIST_H__

#include "OpenGLDrawableImpl.h"

namespace AWT
{
   class OpenGLDrawDisplayList : public OpenGLDrawableImpl
   {
   public:
      typedef ManagedAutoPointer<OpenGLDrawDisplayList> P;

   protected:
      OpenGLDrawDisplayList( );

      virtual ~OpenGLDrawDisplayList( );

   public:
      virtual void getBounds( double* out_Bounds );

   protected:
      virtual ModifiedTime getChildModifiedTime( ) = 0;

      virtual void beforeCallList( DrawContext::P context );

      virtual void afterCallList( DrawContext::P context );

      virtual void drawImpl( DrawContext::P context );

      virtual void buildList( DrawContext::P context ) = 0;
         
      virtual void updateBounds( ) = 0;

      virtual double getBoundImpl( unsigned int bound ) = 0;

      struct D;

      D* m_D;
   };
}

#endif // __OPENGLDRAWDISPLAYLIST_H__