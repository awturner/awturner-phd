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