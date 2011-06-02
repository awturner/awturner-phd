/*
* ################################################################################
* ### MIT License
* ################################################################################
* 
* Copyright (c) 2006-2011 Andy Turner
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/
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