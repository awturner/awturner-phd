#ifndef __OPENGLWIDGETMOUSEBEHAVIOUR_H__
#define __OPENGLWIDGETMOUSEBEHAVIOUR_H__

#include "ObjectManagement/ManagedObject.h"

#include <QObject>

namespace AWT
{
   class OpenGLWidget;

   class OpenGLWidgetMouseBehaviour : public ManagedObject
   {
   public:
      typedef ManagedAutoPointer<OpenGLWidgetMouseBehaviour> P;
      static P getInstance( );

   protected:
      OpenGLWidgetMouseBehaviour( );
      virtual ~OpenGLWidgetMouseBehaviour( );

   public:
      virtual bool mouseMoved( OpenGLWidget* widget, int x, int y, int button, int keys ) = 0;
      virtual bool mousePressed( OpenGLWidget* widget, int x, int y, int button, int keys );
      virtual bool mouseReleased( OpenGLWidget* widget, int x, int y, int button, int keys );

      struct SavedState
      {  
         double rotation[3];
         double orbit[3];
         double distance;

         int mouse[2];
         int button;
      };

      virtual const SavedState* getSavedState( );

   protected:
      SavedState* m_D;
   };
}

#endif // __OPENGLWIDGETMOUSEBEHAVIOUR_H__