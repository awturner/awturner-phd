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
      typedef ManagedAutoPointer<MoghariStateMachine<T> > P;

   protected:
      MoghariStateMachine(StateMachineStack::P stack);
      virtual ~MoghariStateMachine();

   public:
      static P getInstance(StateMachineStack::P stack);
      virtual std::string getClassName() const;

      virtual StateMachineStack* getStack();

      virtual bool step();

      virtual bool isRunning() const;

      virtual bool isError() const;

      virtual std::string getLastError() const;

      virtual std::string getStateName() const;

      virtual void setFragment(typename Fragment<T>::P mesh);

      virtual void setTargetMesh(typename Mesh<T>::P mesh);

      virtual void setMeasureFilename(const T r, const std::string& filename);

   protected:
      struct D;
      D* m_D;

   };
}

#endif // __MOGHARISTATEMACHINE_H__