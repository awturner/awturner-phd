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