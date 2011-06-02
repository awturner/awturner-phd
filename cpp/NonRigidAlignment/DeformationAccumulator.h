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
#ifndef __DEFORMATIONACCUMULATOR_H__
#define __DEFORMATIONACCUMULATOR_H__

#include "NonRigidAlignment.h"
#include "ControlPointSearcher.h"

#include "ControlPointSearcherCallback.h"

namespace AWT
{
   template <class T>
   class DeformationAccumulator : public ControlPointSearcherCallback<T>
   {
   public:
      typedef ManagedAutoPointer<DeformationAccumulator<T> > P;

   protected:
      DeformationAccumulator(typename ControlPointSet<T>::P cps);

      virtual ~DeformationAccumulator();

   public:
      static P getInstance(typename ControlPointSet<T>::P cps);

      virtual void reset();

      virtual void controlPointFound(const MeshIndex controlIndex);

      void getDeformedPoint(T* out_Point) const;

      void getDeformation(T* out_Point) const;

      void getTestPoint(T* out_Point) const;

      void setTestPoint(const T* in_Point);

      unsigned int getNumberOfControlPoints() const;

      virtual std::string getClassName() const;

   protected:
      struct D;
      D* m_D;
   };
}  

#endif // __DEFORMATIONACCUMULATOR_H__