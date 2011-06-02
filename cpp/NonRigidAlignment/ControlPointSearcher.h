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
#ifndef __CONTROLPOINTSEARCHER_H__
#define __CONTROLPOINTSEARCHER_H__

#include "ObjectEKDTree/OEKDTypeDefs.h"
#include "Mesh/MeshTypeDefs.h"

#include "ObjectEKDTree/OEKDNearestSearcher.h"

#include "ControlPointSetTreeData.h"

namespace AWT
{
   namespace OEKDTree
   {
      template <class T, unsigned char K>
      class OEKDTreeBranch;

      template <class T, unsigned char K>
      class OEKDTreeData;
   }

   template <class T>
   class ControlPointSearcherCallback;

   template <class T>
   class ControlPointSearcher : public OEKDTree::OEKDNearestSearcher<T,3>
   {
   public:
      typedef ManagedAutoPointer<ControlPointSearcher<T> > P;

   protected:
      ControlPointSearcher();

      virtual ~ControlPointSearcher();

   public:
      static P getInstance();

      virtual void setCallback(typename ControlPointSearcherCallback<T>::P callback);

      virtual void calculateBoxDistanceBounds2(typename OEKDTree::OEKDTreeBranch<T,3>* in_Branch, OEKDTree::SqDistBounds<T>& bounds ) const;

      virtual void checkObject(const int in_Index);

      virtual bool shouldCheck(OEKDTree::OEKDTreeBranch<T,3> *, const OEKDTree::SqDistBounds<T> &) const;

      virtual void setTestPoint(const MeshIndex in_Index, const T* in_TestPoint );

      virtual void getTestPoint(T* out_TestPoint) const;

      virtual std::string getClassName() const;

      typename ControlPointSetTreeData<T>::P getData();
      void setData(typename OEKDTree::OEKDTreeData<T,3>::P data);

   protected:
      struct D;
      D* m_D;
   };
}

#endif // __CONTROLPOINTSEARCHER_H__