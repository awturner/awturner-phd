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
#ifndef __SIMPLEMESHNEARESTSEARCH_H__
#define __SIMPLEMESHNEARESTSEARCH_H__

#include "Mesh.h"
#include "SearchAgent.h"

#include "SearchFilter.h"

namespace AWT
{
   namespace SimpleMesh
   {
      class NearestSearch : public SearchAgent
      {
      public:
         typedef ManagedAutoPointer<NearestSearch> P;

      protected:
         NearestSearch();
         virtual ~NearestSearch();

      public:
         static P getInstance();
         virtual std::string getClassName() const;
      
         virtual void calculateBoxDistanceBounds2(OEKDTree::OEKDTreeBranch<double,3>* in_Branch, OEKDTree::SqDistBounds<double>& bounds) const;
         virtual bool shouldCheck(AWT::OEKDTree::OEKDTreeBranch<double,3>* in_Branch, const OEKDTree::SqDistBounds<double>& bounds) const;
         
         virtual void checkObject(const int in_Index);

         virtual void setTestPoint(const Point in_TestPoint );
         virtual Point getTestPoint() const;

         virtual bool isPointValid() const;
         virtual PointIndexWeights getNearestPoint() const;

         virtual SearchFilter::P getSearchFilter();
         virtual void setSearchFilter(SearchFilter::P v);

         virtual void reset();

         virtual void setData(AWT::OEKDTree::OEKDTreeData<double,3>::P data);

      protected:
         struct D;
         D* m_D;

      };
   }
}

#endif // __SIMPLEMESHNEARESTSEARCH_H__