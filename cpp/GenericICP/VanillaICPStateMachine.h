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
#ifndef __VANILLAICPSTATEMACHINE_H__
#define __VANILLAICPSTATEMACHINE_H__

#include "StateMachine/SaveableStateMachine.h"

#include "Mesh/Mesh.h"
#include "DeformableMesh/DeformableMesh.h"

#include "StateMachine/StateMachineStack.h"

namespace AWT
{
   template <class T>
   class VanillaICPStateMachine : public SaveableStateMachine
   {
   public:
      typedef ManagedAutoPointer<VanillaICPStateMachine<T> > P;

   protected:
      VanillaICPStateMachine(StateMachineStack::P stack);
      virtual ~VanillaICPStateMachine();

   public:
      static P getInstance(StateMachineStack::P stack);
      virtual std::string getClassName() const;

      virtual StateMachineStack* getStack();

      virtual bool step();

      virtual bool isRunning() const;

      virtual bool isError() const;

      virtual std::string getLastError() const;

      virtual std::string getStateName() const;

      virtual void addFragment(typename Mesh<T>::P mesh);
      virtual typename Mesh<T>::P getFragment(MeshIndex i);

      virtual void setDeformableMesh(typename DeformableMesh<T>::P mesh);
      virtual typename DeformableMesh<T>::P getDeformableMesh();

      virtual typename Mesh<T>::P getRemainingMesh();

      virtual typename Tuples<T>::P getFragmentPoints(MeshIndex i);
      virtual typename Tuples<T>::P getIntactPoints(MeshIndex i);

      virtual vnl_matrix_fixed<T,4,4> getTransformation() const;
      virtual vnl_vector<T> getModeWeights() const;

      virtual T getRms() const;
      virtual T getMaxParallelDistance() const;
      virtual T getMaxPerpendicularDistance() const;

      virtual std::string getMagic() const { return ""; }

      virtual bool load(const std::string& /*filename*/) { return false; }

      virtual void save(const std::string& /*filename*/) { }
   protected:
      struct D;
      D* m_D;

   };
}

#endif // __VANILLAICPSTATEMACHINE_H__