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
#include "StateMachineStack.h"

#include <vector>

struct AWT::StateMachineStack::D
{
   std::vector<BasicStateMachine::P> stack;
};

AWT::StateMachineStack::StateMachineStack()
{
   m_D = new D;
}

AWT::StateMachineStack::~StateMachineStack()
{
   delete m_D;
}

AWT::StateMachineStack::P AWT::StateMachineStack::getInstance()
{
   AUTOGETINSTANCE(AWT::StateMachineStack, ());
}

GETNAMEMACRO(AWT::StateMachineStack);

AWT::StateMachineStack* AWT::StateMachineStack::getStack()
{
   return this;
}

void AWT::StateMachineStack::push(AWT::BasicStateMachine::P m)
{
   m_D->stack.push_back(m);
}

AWT::BasicStateMachine::P AWT::StateMachineStack::getTop() const
{
   if (m_D->stack.empty())
      return AWT::BasicStateMachine::P();

   return m_D->stack.back();
}

AWT::BasicStateMachine::P AWT::StateMachineStack::pop()
{
   BasicStateMachine::P ret = m_D->stack.back();

   m_D->stack.pop_back();

   return ret;
}

bool AWT::StateMachineStack::step()
{
   //PRINTVBL(getTop()->getName());
   bool ret = getTop()->step();

   if (!isRunning() && !isError() && m_D->stack.size() > 1)
      pop();

   return ret;
}

bool AWT::StateMachineStack::isRunning() const
{
   return getTop()->isRunning();
}

bool AWT::StateMachineStack::isError() const
{
   return getTop()->isError();
}

std::string AWT::StateMachineStack::getLastError() const
{
   return getTop()->getLastError();
}

std::string AWT::StateMachineStack::getStateName() const
{
   return getTop()->getStateName();
}

void AWT::StateMachineStack::clear()
{
   m_D->stack.clear();
}