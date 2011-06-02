#ifndef __WORLDVIEWER_H__
#define __WORLDVIEWER_H__

#include "DrawableFramework/DrawContext.h"

#include "DrawableFramework/DrawableWorld.h"

#include "MouseListener.h"

namespace AWT
{
   class WorldViewer : public DrawContext
   {
   public:
      typedef ManagedAutoPointer<WorldViewer> P;

      virtual DrawableWorld::P getDrawableWorld( ) = 0;
      virtual void setDrawableWorld( DrawableWorld::P world ) = 0;

      virtual void getOrbitPoint( double* v ) const = 0;
      virtual void setOrbitPoint( const double* v ) = 0;

      virtual void getRotation( double* v ) const = 0;
      virtual void setRotation( const double* v ) = 0;

      virtual double getDistance( ) const = 0;
      virtual void setDistance( const double v ) = 0;

      virtual void setVisibleRadius( const double v ) = 0;
   
      virtual void addMouseListener( MouseListener::P listener ) = 0;
      virtual void removeMouseListener( MouseListener::P listener ) = 0;

      virtual void refresh( ) = 0;
   };
}

#endif // __WORLDVIEWER_H__