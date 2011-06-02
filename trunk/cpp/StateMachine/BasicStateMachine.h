#ifndef __BASICSTATEMACHINE_H__
#define __BASICSTATEMACHINE_H__

#include "ObjectManagement/ManagedObject.h"

#include <string>

namespace AWT
{
   class StateMachineStack;

   class BasicStateMachine : public ManagedObject
   {
   public:
      typedef ManagedAutoPointer<BasicStateMachine> P;

      virtual StateMachineStack* getStack( ) = 0;

      virtual bool step( ) = 0;

      virtual bool isRunning( ) const = 0;

      virtual bool isError( ) const = 0;

      virtual std::string getLastError( ) const = 0;

      virtual std::string getStateName( ) const = 0;
   };
}

#endif // __BASICSTATEMACHINE_H__