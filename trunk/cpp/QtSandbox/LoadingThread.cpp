/*
#include "LoadingThread.h"

#include "StateMachine/SaveableStateMachine.h"
#include "ObjectManagement/ManagedAutoPointer.h"

struct AWT::LoadingThread::D
{
   enum State
   {
      STATE_NOFILENAME,
      STATE_WAITINGTOLOAD,
      STATE_LOADING,
      STATE_LOADED,
      STATE_ERROR,
      STATE_UNDEFINED
   };

   void load( )
   {
      m_State = STATE_LOADING;

      (*m_StateMachine)->load( m_LoadFilename );

      m_State = STATE_LOADED;
   }

   ManagedAutoPointer<SaveableStateMachine> m_StateMachine;

   bool        m_Error;
   State       m_State;
   std::string m_LoadFilename;
   std::string m_LastError;
};

AWT::LoadingThread::LoadingThread( )
{
   m_D = new D( );

   m_D->m_State = D::STATE_NOFILENAME;
   m_D->m_Error = false;
}

AWT::LoadingThread::~LoadingThread( )
{
   delete m_D;
}

AWT::LoadingThread* AWT::LoadingThread::getInstance( )
{
   return new LoadingThread( );
}

GETNAMEMACRO( AWT::LoadingThread );

std::string AWT::LoadingThread::getLoadFilename( ) const
{
   return m_D->m_LoadFilename;
}

void AWT::LoadingThread::setLoadFilename( const std::string& str )
{
   m_D->m_LoadFilename = str;
   m_D->m_State = D::STATE_WAITINGTOLOAD;
}

bool AWT::LoadingThread::step( )
{
   D::State currentState = m_D->m_State;
   m_D->m_State = D::STATE_UNDEFINED;

   switch ( currentState )
   {
   case D::STATE_NOFILENAME:
      m_D->m_Error = true;
      m_D->m_LastError = "Filename not set";
      break;
   case D::STATE_WAITINGTOLOAD:
      m_D->load( );
      break;
   case D::STATE_LOADING:
      break;
   case D::STATE_LOADED:
      break;
   case D::STATE_ERROR:
      m_D->m_Error = true;
      break;
   case D::STATE_UNDEFINED:
   default:
      m_D->m_Error = true;
      m_D->m_LastError = "Undefined state!";
   }

   return true;
}

bool AWT::LoadingThread::isRunning( ) const
{
   switch ( m_D->m_State )
   {
   case D::STATE_WAITINGTOLOAD:
   case D::STATE_LOADING:
      return true;
   default:
      return false;
   }
}

bool AWT::LoadingThread::isError( ) const
{
   return m_D->m_Error;
}

std::string AWT::LoadingThread::getLastError( ) const
{
   return m_D->m_LastError;
}

std::string AWT::LoadingThread::getStateName( ) const
{
   switch ( m_D->m_State )
   {
   case D::STATE_NOFILENAME:
      return "STATE_NOFILENAME";
   case D::STATE_WAITINGTOLOAD:
      return "STATE_WAITINGTOLOAD";
   case D::STATE_LOADING:
      return "STATE_LOADING";
   case D::STATE_LOADED:
      return "STATE_LOADED";
   case D::STATE_ERROR:
      return "STATE_ERROR";
   case D::STATE_UNDEFINED:
   default:
      return "STATE_UNDEFINED";
   }
}

void AWT::LoadingThread::setStateMachine( AWT::SaveableStateMachine* sm )
{
   m_D->m_StateMachine.set( sm );
}

AWT::SaveableStateMachine* AWT::LoadingThread::getStateMachine( )
{
   return *m_D->m_StateMachine;
}
*/