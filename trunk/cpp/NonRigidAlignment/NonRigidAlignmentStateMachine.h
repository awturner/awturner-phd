#ifndef __NONRIGIDALIGNMENTSTATEMACHINE_H__
#define __NONRIGIDALIGNMENTSTATEMACHINE_H__

#include <string>
#include <ostream>

#include "StateMachine/SaveableStateMachine.h"

#include "ControlPointSet.h"

#include "StateMachine/StateMachineStack.h"


namespace AWT
{
   template <class T>
   class NonRigidAlignment;

   template <class T>
   class NonRigidAlignmentStateMachine : public SaveableStateMachine
   {
   public:
      typedef ManagedAutoPointer<NonRigidAlignmentStateMachine<T>> P;

   protected:
      NonRigidAlignmentStateMachine( StateMachineStack::P stack, const bool rigidOnly = false );
      ~NonRigidAlignmentStateMachine( );
      
   public:
      static P getInstance( StateMachineStack::P stack, const bool rigidOnly );

      virtual std::string getClassName( ) const;

      virtual StateMachineStack* getStack( );

      void externalStop( );
      void loadOnly( );

      bool step( );
      bool isRunning( ) const;
      bool isError( ) const;

      std::string getLastError( ) const;

      virtual std::string getStateName( ) const;

      bool load( const std::string& filename );
      void save( const std::string& filename );
      std::string getMagic( ) const;

      NonRigidAlignment<T>* getNonRigidAlignment( );

      bool setSourceMeshFilename( const char* fn );
      bool setTargetMeshFilename( const char* fn );

      bool saveDeformedModel( const char* fn ) const;
      bool saveProjected( const char* fn ) const;
      bool saveTargetModel( const char* fn ) const;

      bool aboutToRefine( ) const;

      bool setControlPoints( typename ControlPointSet<T>::P cps );

      bool addControlPointSpacing( T spacing );

      void getBounds( T* bounds );
      
   protected:
      struct D;
      D* m_D;
   };
}

#endif // __NONRIGIDALIGNMENTSTATEMACHINE_H__