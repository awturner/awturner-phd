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
#ifndef __POINTSNEARESTPOINTSSEARCH_H__
#define __POINTSNEARESTPOINTSSEARCH_H__

#include "MeshSearching/VerticesSearch.h"
#include "MeshSearching/VerticesTreeData.h"

#include <limits>

#include "ObjectEKDTree/OEKDTreeBranch.h"
#include "MeshSearching/VerticesTreeData.h"

namespace AWT
{
   template <class T>
   class VerticesNearestVertexSearch : public VerticesSearch<T>
   {
   public:
      typedef ManagedAutoPointer< VerticesNearestVertexSearch<T> > P;
      static P getInstance();

   protected:
      VerticesNearestVertexSearch(const T in_MaxDistance = std::numeric_limits<T>::infinity());
      virtual ~VerticesNearestVertexSearch();

   public:

      virtual void calculateBoxDistanceBounds2(typename OEKDTreeBranch<T,3>* in_Branch, SqDistBounds<T>& bounds ) const;
      virtual bool shouldCheck(typename AWT::OEKDTree::OEKDTreeBranch<T,3>* in_Branch, const SqDistBounds<T>& bounds) const;

      virtual void checkObject(const int in_Index);

      virtual void setTreeData(typename VerticesTreeData<T>::P data);
      virtual typename VerticesTreeData<T>::P getTreeData();

      virtual void setTestPoint(const T* in_TestPoint );
      virtual void getTestPoint(T* out_TestPoint) const;

      virtual MeshIndex getNearestPoint(T* out_NearestPoint = 0) const;

      virtual void reset();

      virtual std::string getClassName() const;

      void setData(typename OEKDTreeData<T,3>::P in_Data);
      typename typename VerticesTreeData<T>::P getData();

   protected:
      struct D;
      D* m_D;
   };
}

#endif __POINTSNEARESTPOINTSSEARCH_H__