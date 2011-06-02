#ifndef __STATEMACHINESTACK_H__
#define __STATEMACHINESTACK_H__

#include "ObjectManagement/ManagedObject.h"

#include "BasicStateMachine.h"

namespace AWT
{
   class StateMachineStack : public BasicStateMachine
   {
   public:
      typedef ManagedAutoPointer<StateMachineStack> P;

   protected:
      StateMachineStack( );
      virtual ~StateMachineStack( );

   public:
      static P getInstance( );
      virtual std::string getClassName( ) const;

      virtual StateMachineStack* getStack( );

      virtual void push( BasicStateMachine::P m );

      virtual BasicStateMachine::P getTop( ) const;

      virtual void clear( );

      virtual BasicStateMachine::P pop( );

      virtual bool step( );

      virtual bool isRunning( ) const;

      virtual bool isError( ) const;

      virtual std::string getLastError( ) const;

      virtual std::string getStateName( ) const;

   protected:
      struct D;
      D* m_D;

   };
}

#endif // __STATEMACHINESTACK_H__