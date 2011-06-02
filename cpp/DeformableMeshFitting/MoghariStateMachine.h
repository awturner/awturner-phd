#ifndef __MOGHARISTATEMACHINE_H__
#define __MOGHARISTATEMACHINE_H__

#include "StateMachine/BasicStateMachine.h"

#include "Mesh/Mesh.h"

#include "StateMachine/StateMachineStack.h"

#include "Fragment.h"

namespace AWT
{
   template <class T>
   class MoghariStateMachine : public BasicStateMachine
   {
   public:
      typedef ManagedAutoPointer<MoghariStateMachine<T>> P;

   protected:
      MoghariStateMachine( StateMachineStack::P stack );
      virtual ~MoghariStateMachine( );

   public:
      static P getInstance( StateMachineStack::P stack );
      virtual std::string getClassName( ) const;

      virtual StateMachineStack* getStack( );

      virtual bool step( );

      virtual bool isRunning( ) const;

      virtual bool isError( ) const;

      virtual std::string getLastError( ) const;

      virtual std::string getStateName( ) const;

      virtual void setFragment( typename Fragment<T>::P mesh );

      virtual void setTargetMesh( typename Mesh<T>::P mesh );

      virtual void setMeasureFilename( const T r, const std::string& filename );

   protected:
      struct D;
      D* m_D;

   };
}

#endif // __MOGHARISTATEMACHINE_H__