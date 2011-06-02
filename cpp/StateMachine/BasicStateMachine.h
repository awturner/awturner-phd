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

      virtual StateMachineStack* getStack() = 0;

      virtual bool step() = 0;

      virtual bool isRunning() const = 0;

      virtual bool isError() const = 0;

      virtual std::string getLastError() const = 0;

      virtual std::string getStateName() const = 0;
   };
}

#endif // __BASICSTATEMACHINE_H__