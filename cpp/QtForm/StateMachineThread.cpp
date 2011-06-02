#include "include/StateMachineThread.h"

#include "NonRigidAlignment/NonRigidAlignment.h"
#include "NonRigidAlignment/NonRigidAlignmentStateMachine.h"
#include <QWidget>

#include "StateMachine/BasicStateMachine.h"

#include "Useful/PrintMacros.h"
#include "Useful/Exception.h"

#include "DrawableFramework/Drawable.h"

#include <fstream>
#include <sstream>
#include <QMutex>

#include "MimlabCore/Exceptions.h"

struct AWT::StateMachineThread::D
{
   QMutex*              m_Mutex;
   bool                 m_StopRequested;

   BasicStateMachine::P m_StateMachine;
};

AWT::StateMachineThread::StateMachineThread( QWidget* form, AWT::BasicStateMachine::P stateMachine, QMutex* mutex )
{
   m_D = new D;

   m_D->m_Mutex = mutex;

   m_D->m_StateMachine = stateMachine;

   m_D->m_StopRequested = false;

   connect( this, SIGNAL( repaintWidget( ) ),         form, SLOT( repaintWidget( ) )         );
   connect( this, SIGNAL( clientError( QString ) ),   form, SLOT( clientError( QString ) )   );
   connect( this, SIGNAL( clientWarning( QString ) ), form, SLOT( clientWarning( QString ) ) );
   connect( this, SIGNAL( finished( ) ),              form, SLOT( processingFinished( ) )   );
}

AWT::StateMachineThread::~StateMachineThread( )
{
   delete m_D;
}

AWT::StateMachineThread::P AWT::StateMachineThread::getInstance( QWidget* form, AWT::BasicStateMachine::P stateMachine, QMutex* mutex )
{
   AUTOGETINSTANCE( AWT::StateMachineThread, ( form, stateMachine, mutex ) );
}

GETNAMEMACRO( AWT::StateMachineThread );

void AWT::StateMachineThread::requestStop( )
{
   m_D->m_StopRequested = true;
}

void AWT::StateMachineThread::step( )
{
   try
   {
      {
         // Wrap so that the mutex locker goes out of scope before repaint is emitted
         QMutexLocker mutexLocker( m_D->m_Mutex );
         PRINTVBL( m_D->m_Mutex );

         DEBUGLINE;

         PRINTVBL2( stateName, m_D->m_StateMachine->getStateName( ) );

         //AWT::Drawable::acquireMutex( this );
         m_D->m_StateMachine->step( );
         //AWT::Drawable::releaseMutex( this );

         DEBUGLINE;
      }

      emit repaintWidget( );
   }
   catch ( AWT::Exception& ex )
   {
      QString msg( tr( "AWT::Exception caught:\n\n" ) );

      std::stringstream sstr;
      sstr << ex;

      msg.append( sstr.str( ).c_str( ) );

      emit clientError( msg );

      DEBUGMACRO( "An AWT::Exception was caught; stopping" );
      PRINTVBL( ex );
   }
   catch ( Mimlab::Exceptions::Exception& ex )
   {
      QString msg( tr( "Mimlab::Exception caught:\n\n" ) );

      std::stringstream sstr;
      sstr << ex;

      msg.append( sstr.str( ).c_str( ) );

      emit clientError( msg );

      DEBUGMACRO( "A Mimlab Exception was caught; stopping" );
      PRINTVBL( ex );
   }
   catch ( std::exception& ex )
   {
      QString msg( tr( "Exception caught:\n\n" ) );

      std::stringstream sstr;
      sstr << ex.what( );

      msg.append( sstr.str( ).c_str( ) );

      emit clientError( msg );

      DEBUGMACRO( "An std::Exception was caught; stopping" );
      PRINTVBL( ex.what( ) );
   }
   catch ( const char* ex )
   {
      QString msg( tr( "Exception caught:\n\n" ) );

      std::stringstream sstr;
      sstr << ex;

      msg.append( sstr.str( ).c_str( ) );

      emit clientError( msg );

      DEBUGMACRO( "An exception was caught; stopping" );
      PRINTVBL( ex );
   }
   catch ( ... )
   {
      QString msg( tr( "An unknown exception was caught" ) );
      emit clientError( msg );

      DEBUGMACRO( "An unknown exception was caught; stopping" );
   }

   if ( m_D->m_StateMachine->isError( ) )
   {
      emit clientError( tr( m_D->m_StateMachine->getLastError( ).c_str( ) ) );

      PRINTVBL( m_D->m_StateMachine->getLastError( ) );
   }

}

void AWT::StateMachineThread::singleStep( )
{
   if ( !m_D->m_StopRequested && m_D->m_StateMachine->isRunning( ) && !m_D->m_StateMachine->isError( ) )
   {
      step( );

      msleep( 20 );
   }

   m_D->m_StopRequested = false;
}

void AWT::StateMachineThread::run( )
{
   while ( !m_D->m_StopRequested && m_D->m_StateMachine->isRunning( ) && !m_D->m_StateMachine->isError( ) )
   {
      step( );

      msleep( 20 );
   }

   m_D->m_StopRequested = false;
}
