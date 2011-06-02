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