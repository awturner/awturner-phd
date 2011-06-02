#ifndef __OPENGLWIDGET_H__
#define __OPENGLWIDGET_H__

#include <QGLWidget>
#include <QTimer>
#include <QMouseEvent>
#include <QContextMenuEvent>

#include "ObjectManagement/ManagedObject.h"

#include "OpenGLWidgetMouseBehaviour.h"

#include "DrawableFramework/DrawContext.h"
#include "DrawableFramework/DrawableWorldOwner.h"

#include "MouseListener.h"

#include "WorldViewer.h"

class QMutex;

namespace AWT
{
   class DrawableWorld;
   class Light;

   class OpenGLWidget : public QGLWidget, public WorldViewer
   {
      Q_OBJECT

   public:
      OpenGLWidget( QWidget* parent = 0, QMutex* mutex = 0 );
      virtual ~OpenGLWidget( );

      DrawableWorld::P getDrawableWorld( );
      void setDrawableWorld( DrawableWorld::P world );
      
      std::string getClassName( ) const;

   public:
      void getOrbitPoint( double* v ) const;
      void getRotation( double* v ) const;
      void getEyePosition( double* v ) const;
      double getDistance( ) const;

      OpenGLWidgetMouseBehaviour::P getMouseBehaviour( );
      void setMouseBehaviour( OpenGLWidgetMouseBehaviour::P arg );

      void makeCurrent( );

   signals:
      void matrixSet( double* mtx );
      
   public slots:
      void setOrbitPoint( const double* v );
      void setRotation( const double* v );
      void setDistance( const double v );
      void setVisibleRadius( const double v );

      void resetView( const bool repaintAfter = true );

      bool isViewRelativeLighting( ) const;
      void setViewRelativeLighting( const bool v );

      void takeScreenshot( );

      void refresh( );

   public slots:
      void changeToUserBackground( );
      void changeToBlackBackground( );
      void changeToWhiteBackground( );

   protected:
      void initializeGL( );
      void resizeGL( int w, int h );
      void paintGL( );

      void contextMenuEvent(QContextMenuEvent *event);

      virtual void enterEvent( QEvent* ev );
      virtual void leaveEvent( QEvent* ev );

      virtual void mouseMoveEvent( QMouseEvent* ev );
      virtual void mousePressEvent( QMouseEvent* ev );
      virtual void mouseReleaseEvent( QMouseEvent* ev );

      void gluPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar);

   public:
      virtual void getViewPosition( double* pos );
      virtual void getMouseDirection( double* norm );

      virtual bool isMouseOver( ) const;

      virtual void drawText( int x, int y, const std::string& text );
      virtual void drawText( double x, double y, double z, const std::string& text );

   public:
      virtual void addMouseListener( MouseListener::P listener );
      virtual void removeMouseListener( MouseListener::P listener );

   protected:
      struct D;
      D* m_D;
   };
}

#endif // __OPENGLWIDGET_H__