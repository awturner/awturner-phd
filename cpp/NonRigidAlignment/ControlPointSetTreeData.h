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
#ifndef __CONTROLPOINTSETTREEDATA_H__
#define __CONTROLPOINTSETTREEDATA_H__

#include "ObjectEKDTree/OEKDTreeData.h"
#include "ControlPoint.h"
#include <vector>

namespace AWT
{
   template <class T>
   class ControlPoint;

   template <class T>
   class ControlPointSetTreeData : public OEKDTree::OEKDTreeData<T,3>
   {
   public:
      typedef ManagedAutoPointer< ControlPointSetTreeData<T> > P;

   protected:
      ControlPointSetTreeData(std::vector<typename ControlPoint<T>::P>* vec);
      virtual ~ControlPointSetTreeData();

   public:
      static P getInstance(std::vector<typename ControlPoint<T>::P>* vec);

      virtual OEKDTree::ObjectIndex getNumberOfObjects() const;

      virtual T getValue(OEKDTree::ObjectIndex objectIndex, OEKDTree::AxisIndex axis) const;

      virtual T getCoordinate(OEKDTree::ObjectIndex objectIndex, OEKDTree::AxisIndex axis) const;
      virtual void getPosition(const OEKDTree::ObjectIndex objectIndex, T* pos) const;

      virtual T getMinimumBound(OEKDTree::ObjectIndex objectIndex, OEKDTree::AxisIndex axis) const;
      virtual T getMaximumBound(OEKDTree::ObjectIndex objectIndex, OEKDTree::AxisIndex axis) const;

      virtual std::string getClassName() const;
   protected:
      struct D;
      D* m_D;
   };
}

#endif // __CONTROLPOINTSETTREEDATA_H__