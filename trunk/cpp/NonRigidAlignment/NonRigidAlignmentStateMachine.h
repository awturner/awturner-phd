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