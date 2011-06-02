#ifndef __MOVECAMERAMOUSEBEHAVIOUR_H__
#define __MOVECAMERAMOUSEBEHAVIOUR_H__

#include "OpenGLWidgetMouseBehaviour.h"

namespace AWT
{
   class OpenGLWidget;

   class MoveCameraMouseBehaviour : public OpenGLWidgetMouseBehaviour
   {
   public:
      typedef ManagedAutoPointer<MoveCameraMouseBehaviour> P;

   protected:
      MoveCameraMouseBehaviour( );
      virtual ~MoveCameraMouseBehaviour( );

   public:
      static P getInstance( );
      virtual std::string getClassName( ) const;

      virtual bool mouseMoved( OpenGLWidget* widget, int x, int y, int button, int keys );

   protected:
      struct D;
      D* m_D;
   };
}

#endif // __MOVECAMERAMOUSEBEHAVIOUR_H__