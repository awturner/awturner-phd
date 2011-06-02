#ifndef __QTSANDBOXFORM_H__
#define __QTSANDBOXFORM_H__

#include <QWidget>
#include <QMoveEvent>
#include <QDesktopWidget>
#include "QtForm/ui_QtForm.h"

#include "ObjectManagement/ManagedObject.h"

#include "StateMachine/SaveableStateMachine.h"

#include "QtForm/include/WorldViewer.h"
#include "QtForm/include/MouseListener.h"

class QMutex;

namespace AWT
{
   class QtForm : public QMainWindow, public Ui::QtForm, public ManagedObject
   {
      Q_OBJECT

   public:
      typedef ManagedAutoPointer<QtForm> P;

   protected:
      QtForm( unsigned int rows = 1, unsigned int cols = 1, QMutex* mutex = 0, QWidget* in_Parent = 0, Qt::WFlags in_Fl = 0 );
      virtual ~QtForm( );

   public:
      static P getInstance( unsigned int rows = 1, unsigned int cols = 1, QMutex* mutex = 0, QWidget* in_Parent = 0, Qt::WFlags in_Fl = 0 );

      virtual std::string getClassName( ) const;

      virtual unsigned int getNumberOfViewers( ) const;

      virtual WorldViewer::P getViewer( const unsigned int i );

   signals:
      void widgetNeedsRepainting( );

   protected slots:
      void clientError( QString str );
      void clientWarning( QString str );

   public slots:
      void centreView( );

      void processingFinished( );

      void load( const QString& filename );

   public slots:

      void setStateMachine( SaveableStateMachine::P sm );

   public slots:
      void actionOpenTriggered( );
      void actionSaveTriggered( );
      void actionExitTriggered( );
      
      void actionStartTriggered( );
      void actionStepTriggered( );
      void actionStopTriggered( );
      void actionResetTriggered( );

      void actionReportTriggered( );

   protected:

      struct D;
      D* m_D;
   };
}

#endif // __QTSANDBOXFORM_H__