/*
#ifndef __LOADINGTHREAD_H__
#define __LOADINGTHREAD_H__

#include "StateMachine/BasicStateMachine.h"

namespace AWT
{
   class SaveableStateMachine;

   class LoadingThread : public BasicStateMachine
   {
   protected:
      LoadingThread( );
      virtual ~LoadingThread( );

   public:
      static LoadingThread* getInstance( );

      virtual std::string getClassName( ) const;

      virtual void setLoadFilename( const std::string& str );

      virtual std::string getLoadFilename( ) const;

      virtual bool step( );

      virtual bool isRunning( ) const;

      virtual bool isError( ) const;

      virtual std::string getLastError( ) const;

      virtual std::string getStateName( ) const;

      virtual void setStateMachine( SaveableStateMachine* sm );

      virtual SaveableStateMachine* getStateMachine( );
   protected:
      struct D;
      D* m_D;

   };
}

#endif // __LOADINGTHREAD_H__
*/