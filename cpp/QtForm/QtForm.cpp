#include "include/QtForm.h"

#include "StateMachine/StateMachineStack.h"

#include "include/StateMachineThread.h"

#include <QFileDialog>
#include <QMutex>
#include <QMessageBox>
#include <QColor>

class QDebugStream : public std::basic_streambuf<char>
{
public:
   QDebugStream(std::ostream &stream, QTextEdit* text_edit, const QColor& col) : m_stream(stream), m_Col( col )
   {
      log_window = text_edit;
      m_old_buf = stream.rdbuf();
      stream.rdbuf(this);
   }
   ~QDebugStream()
   {
      // output anything that is left
      if (!m_string.empty())
         log_window->append(m_string.c_str());

      m_stream.rdbuf(m_old_buf);
   }

protected:
   virtual int_type overflow(int_type v)
   {
      log_window->setTextColor( m_Col );
      if (v == '\n')
      {
         log_window->append(m_string.c_str());
         m_string.erase(m_string.begin(), m_string.end());
      }
      else
         m_string += v;

      log_window->moveCursor( QTextCursor::End, QTextCursor::MoveAnchor );
      log_window->moveCursor( QTextCursor::StartOfLine, QTextCursor::MoveAnchor );
      return v;
   }

   virtual std::streamsize xsputn(const char *p, std::streamsize n)
   {
      log_window->setTextColor( m_Col );
      m_string.append(p, p + n);

      int pos = 0;
      while (pos != std::string::npos)
      {
         pos = m_string.find('\n');
         if (pos != std::string::npos)
         {
            std::string tmp(m_string.begin(), m_string.begin() + pos);
            log_window->append(tmp.c_str());
            m_string.erase(m_string.begin(), m_string.begin() + pos + 1);
         }
      }

      log_window->moveCursor( QTextCursor::End, QTextCursor::MoveAnchor );
      log_window->moveCursor( QTextCursor::StartOfLine, QTextCursor::MoveAnchor );
      return n;
   }

private:
   std::ostream &m_stream;
   std::streambuf *m_old_buf;
   std::string m_string;
   QTextEdit* log_window;
   const QColor m_Col;
};

struct AWT::QtForm::D
{
   StateMachineStack::P     m_StateMachineStack; 
   StateMachineThread::P    m_StateMachineThread;
   SaveableStateMachine::P  m_StateMachine;      

   WorldTreeModel*          m_WorldTreeModel;
   QString                  m_LastDir;

   QDebugStream*            m_Stderr;
   QDebugStream*            m_Stdout;

   QMutex* m_Mutex;
};

#include <QList>
#include <QDesktopWidget>

AWT::QtForm::QtForm( unsigned int rows, unsigned int cols, QMutex* mutex, QWidget* in_Parent, Qt::WFlags in_Fl )
: QMainWindow( in_Parent, in_Fl )
{
   m_D = new D;
   m_D->m_Mutex = mutex;

   setupUi( this );

   {
      QDesktopWidget widge;
      const QRect geom = widge.screenGeometry( );
      QList<int> sizeList;
      sizeList.push_back( 3*geom.height() / 4 );
      sizeList.push_back( 1*geom.height() / 4 );
      splitter->setSizes( sizeList );
   }

   // Re-route the standard error and output streams through the GUI
   //m_D->m_Stderr = new QDebugStream( std::cerr, txtCerr, QColor( 127, 0, 0 ) );
   //m_D->m_Stdout = new QDebugStream( std::cout, txtCout, QColor( 0, 0, 255 ) );

   worldViewWidget->initialize( rows, cols, mutex );

   connect( actionStart, SIGNAL( triggered( ) ), this, SLOT( actionStartTriggered( ) ) );
   actionStart->setShortcut( tr( "Ctrl+G" ) );

   connect( actionStep, SIGNAL( triggered( ) ), this, SLOT( actionStepTriggered( ) ) );
   actionStep->setShortcut( tr( "Ctrl+Alt+G" ) );

   connect( actionStop,  SIGNAL( triggered( ) ), this, SLOT( actionStopTriggered( ) ) );
   actionStop->setShortcut( tr( "Ctrl+Shift+G" ) );

   connect( actionReset, SIGNAL( triggered( ) ), this, SLOT( actionResetTriggered( ) ) );
   actionReset->setShortcut( QKeySequence::Refresh );

   connect( actionOpen,  SIGNAL( triggered( ) ), this, SLOT( actionOpenTriggered( ) ) );
   actionOpen->setShortcut( QKeySequence::Open );

   connect( actionSave,  SIGNAL( triggered( ) ), this, SLOT( actionSaveTriggered( ) ) );
   actionSave->setShortcut( QKeySequence::Save );

   connect( actionExit,  SIGNAL( triggered( ) ), this, SLOT( actionExitTriggered( ) ) );

   connect( actionReport, SIGNAL( triggered( ) ), this, SLOT( actionReportTriggered( ) ) );

   connect( *m_D->m_StateMachineThread, SIGNAL( repaintWidget( ) ), worldViewWidget, SLOT( repaintViews( ) ) );

   this->worldTreeView->setModel( worldViewWidget->getWorldTreeModel( ) );

   connect( worldViewWidget->getWorldTreeModel( ), SIGNAL( dataChanged( const QModelIndex&, const QModelIndex& ) ), worldTreeView, SLOT( expandAll( ) ) );
}

AWT::QtForm::~QtForm( )
{
   //delete m_D->m_Stderr;
   //delete m_D->m_Stdout;

   if ( *m_D->m_StateMachineStack != 0 )
   {
      PRINTVBL( m_D->m_StateMachineStack->getReferenceCount( ) );

      /*
      while ( m_D->m_StateMachineStack->getReferenceCount( ) > 1)
         m_D->m_StateMachineStack->release( );
         */
   }

   DEBUGLINE;
   delete m_D;
   DEBUGLINE;
}

AWT::QtForm::P AWT::QtForm::getInstance( unsigned int rows, unsigned int cols, QMutex* mutex, QWidget* in_Parent, Qt::WFlags in_Fl )
{
   AUTOGETINSTANCE( AWT::QtForm, ( rows, cols, mutex, in_Parent, in_Fl ) );
}

GETNAMEMACRO( AWT::QtForm );

void AWT::QtForm::processingFinished( )
{
   actionStep->setEnabled( true );
   actionStart->setEnabled( true );
   actionStop->setEnabled( false );

   //centreView( );
   actionSaveTriggered( );
}

void AWT::QtForm::actionOpenTriggered( )
{
   typedef double T;

   if ( *m_D->m_StateMachineThread != 0 && m_D->m_StateMachineThread->isRunning( ) )
      return;

   QString filename = QFileDialog::getOpenFileName( 
      this, 
      QString( tr( "Load" ) ), 
      m_D->m_LastDir,
      QString( ) 
      );

   if ( filename.length( ) != 0 )
   {
      load( filename );
   }
}

void AWT::QtForm::load( const QString& /*filename*/ )
{
   QMessageBox::warning( this, "Not implemented", "Loading hasn't been implemented" );
}

void AWT::QtForm::centreView( )
{
   worldViewWidget->centreViews( );
}

void AWT::QtForm::setStateMachine( AWT::SaveableStateMachine::P sm )
{
   m_D->m_StateMachine = sm;

   m_D->m_StateMachineStack = sm->getStack( );
   m_D->m_StateMachineStack->clear( );
   m_D->m_StateMachineStack->push( sm );

   if ( *m_D->m_StateMachineThread != 0 )
      disconnect( *m_D->m_StateMachineThread, SIGNAL( repaintWidth( ) ), worldViewWidget, SLOT( repaintViews( ) ) );

   m_D->m_StateMachineThread = StateMachineThread::getInstance( this, m_D->m_StateMachineStack, m_D->m_Mutex );
   connect( *m_D->m_StateMachineThread, SIGNAL( repaintWidget( ) ), worldViewWidget, SLOT( repaintViews( ) ) );
}

void AWT::QtForm::actionSaveTriggered( )
{
   QString filename = QFileDialog::getSaveFileName( 
      this, 
      QString( tr( "Saving Data" ) ), 
      m_D->m_LastDir, 
      QString( /*tr( "XML Files (*.xml);;All Files (*.*)" )*/ )
      );

   if ( filename.length( ) != 0 )
   {
      m_D->m_StateMachine->save( filename.toStdString( ) );
   }
}

void AWT::QtForm::actionExitTriggered( )
{
   QApplication::exit( );
}

void AWT::QtForm::clientWarning(QString str)
{
   if ( str.length( ) == 0 )
      str = tr( "A client warning has occurred, but no message was provided" );

   QMessageBox::warning( this, "Client warning", str );
}

void AWT::QtForm::clientError(QString str)
{
   if ( str.length( ) == 0 )
      str = tr( "A client error has occurred, but no message was provided" );

   QMessageBox::critical( this, "Client error", str );
}

void AWT::QtForm::actionStartTriggered( )
{
   if ( *m_D->m_StateMachineThread == 0 )
   {
      QMessageBox::warning( this, tr("Error"), tr( "State machine not initialized" ) );
      return;
   }

   actionStep->setEnabled( false );
   actionStart->setEnabled( false );
   actionStop->setEnabled( true );
   m_D->m_StateMachineThread->start( );
}

void AWT::QtForm::actionStepTriggered( )
{
   if ( *m_D->m_StateMachineThread == 0 )
   {
      QMessageBox::warning( this, tr("Error"), tr( "State machine not initialized" ) );
      return;
   }

   m_D->m_StateMachineThread->singleStep( );
}

void AWT::QtForm::actionStopTriggered( )
{
   if ( *m_D->m_StateMachineThread == 0 )
   {
      QMessageBox::warning( this, tr("Error"), tr( "State machine not initialized" ) );
      return;
   }

   m_D->m_StateMachineThread->requestStop( );
}

void AWT::QtForm::actionResetTriggered( )
{
}

void AWT::QtForm::actionReportTriggered( )
{
   AWT::ReferenceCountedObject::report( false );
}

AWT::WorldViewer::P AWT::QtForm::getViewer( const unsigned int i )
{
   return worldViewWidget->getViewer( i );
}

unsigned int AWT::QtForm::getNumberOfViewers( ) const
{
   return worldViewWidget->getNumberOfViewers( );
}