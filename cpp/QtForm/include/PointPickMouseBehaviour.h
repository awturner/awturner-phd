#ifndef __POINTPICKMOUSEBEHAVIOUR_H__
#define __POINTPICKMOUSEBEHAVIOUR_H__

#include "OpenGLWidgetMouseBehaviour.h"

#include "Mesh/Mesh.h"

#include <QObject>

#include "Notifiable.h"

namespace AWT
{
   class OpenGLWidget;

   class PointPickMouseBehaviour : public OpenGLWidgetMouseBehaviour
   {
      typedef double T;

   public:
      typedef ManagedAutoPointer<PointPickMouseBehaviour> P;

   protected:
      PointPickMouseBehaviour( );
      virtual ~PointPickMouseBehaviour( );

   public:
      static P getInstance( );
      virtual std::string getClassName( ) const;

      virtual void setTarget( Mesh<T>::P mesh );
      virtual Mesh<T>::P getTarget( );

      virtual void setNotifiable( Notifiable* not );

      virtual void getLastPicked( double pnt[3] );

      virtual bool mousePressed( OpenGLWidget* widget, int x, int y, int button, int keys );
      virtual bool mouseMoved( OpenGLWidget* widget, int x, int y, int button, int keys );

   protected:
      struct D;
      D* m_D;
   };
}

#endif // __POINTPICKMOUSEBEHAVIOUR_H__