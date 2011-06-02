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
#ifndef __MESHSLICER_H__
#define __MESHSLICER_H__

#include "ObjectManagement/ManagedObject.h"

#include "MeshSlicedPair.h"

#include "CuttingFunction.h"

namespace AWT
{
   template <class T>
   class CuttingFunction;

   template <class T>
   class Mesh;

   template <class T>
   class MeshSlicer : public ManagedObject
   {
   public:
      typedef ManagedAutoPointer<MeshSlicer<T>> P;

   protected:
      MeshSlicer( );
      virtual ~MeshSlicer( );

   public:
      static P getInstance( );
      virtual std::string getClassName( ) const;

      virtual void setCuttingFunction( typename CuttingFunction<T>::P cutF );
      virtual typename CuttingFunction<T>::P getCuttingFunction( );

      virtual typename MeshSlicedPair<T>::P sliceMesh( typename Mesh<T>::P mesh );

   protected:
      struct D;
      D* m_D;

   };
}

#endif // __MESHSLICER_H__