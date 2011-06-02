#ifndef __WAGGLEMODESTATEMACHINE_H__
#define __WAGGLEMODESTATEMACHINE_H__

#include "StateMachine/SaveableStateMachine.h"

#include "StateMachine/StateMachineStack.h"

#include "DeformableMesh.h"

namespace AWT
{
   template <class T>
   class WaggleModeStateMachine : public SaveableStateMachine
   {
   public:
      typedef ManagedAutoPointer<WaggleModeStateMachine<T>> P;

   protected:
      WaggleModeStateMachine( StateMachineStack::P stack );
       

   public:
      static P getInstance( StateMachineStack::P stack );

      void setMesh( typename DeformableMesh<T>::P mesh );

      StateMachineStack* getStack( );

      bool step( );

      bool isRunning( ) const;

      bool isError( ) const;

      std::string getLastError( ) const;

      std::string getStateName( ) const;

      std::string getMagic( ) const;

      bool load( const std::string& );

      void save( const std::string& );

      std::string getClassName( ) const;

   protected:
      bool m_Finished;
      unsigned int m_Step;
      StateMachineStack::P m_Stack;
      typename DeformableMesh<T>::P m_Mesh;
   };
}

#endif // __WAGGLEMODESTATEMACHINE_H__