#ifndef __STATEMACHINETHREAD_H__
#define __STATEMACHINETHREAD_H__

#include <QThread>
#include <QString>

#include "ObjectManagement/ManagedObject.h"

#include "StateMachine/BasicStateMachine.h"

class QWidget;   
class QMutex;

namespace AWT
{
   class BasicStateMachine;

   class StateMachineThread : public QThread, public ManagedObject
   {
      Q_OBJECT;
   public:
      typedef ManagedAutoPointer<StateMachineThread> P;

   signals:
      void repaintWidget( );
      void clientError( QString str );
      void clientWarning( QString str );

   protected:
      StateMachineThread( QWidget* form, BasicStateMachine::P stateMachine, QMutex* mutex = 0 );
      virtual ~StateMachineThread( );

   public:
      static P getInstance( QWidget* form, BasicStateMachine::P stateMachine, QMutex* mutex = 0 );

      virtual std::string getClassName( ) const;

   public slots:
      void requestStop( );

      void run( );
      void singleStep( );

   protected slots:

      void step( );

   protected:
      struct D;
      D* m_D;
   };
}

#endif // __STATEMACHINETHREAD_H__