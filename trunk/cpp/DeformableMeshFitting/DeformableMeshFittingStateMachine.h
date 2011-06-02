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
#ifndef __DEFORMABLEMESHFITTINGSTATEMACHINE_H__
#define __DEFORMABLEMESHFITTINGSTATEMACHINE_H__

#include "StateMachine/SaveableStateMachine.h"

#include "DeformableMeshFittingTypeDefs.h"

#include "DeformableMeshFitting.h"
#include "Mesh/MeshSimilarity.h"
#include "Mesh/Pose.h"

#include "StateMachine/StateMachineStack.h"

namespace AWT
{
   template <class T>
   class DeformableMeshFittingStateMachine : public SaveableStateMachine
   {
   public:
      typedef ManagedAutoPointer< DeformableMeshFittingStateMachine<T> > P;

   protected:
      DeformableMeshFittingStateMachine( StateMachineStack::P stack );
      virtual ~DeformableMeshFittingStateMachine( );

   public:
      static P getInstance( StateMachineStack::P stack );

      virtual std::string getClassName( ) const;

      virtual StateMachineStack* getStack( );

      virtual bool step( );

      virtual std::string getStateName( ) const;

      virtual bool isRunning( ) const;

      virtual bool isError( ) const;

      virtual std::string getLastError( ) const;

      virtual void setMoghariFilename( const T radius, const std::string& filename );

      virtual typename DeformableMeshFitting<T>::P getDeformableMeshFitting( );

      virtual void setDeformableFilename( std::string& filename );

      virtual TargetIndex getNumberOfTargetFilenames( ) const;

      virtual void addTargetFilename( std::string& filename );

      virtual void setInitialPose( const TargetIndex i, typename Pose<T>::P pose );

      virtual void setInitialParameters( const T* params );

      virtual bool load( const std::string& filename );

      virtual void save( const std::string& filename );

      virtual std::string getMagic( ) const;

      virtual void evalOnly( );

   protected:
      struct D;
      D* m_D;
   };
}

#endif // __DEFORMABLEMESHFITTINGSTATEMACHINE_H__