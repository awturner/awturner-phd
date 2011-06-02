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

#include "VerticesSearch.h"

#include "MeshSearching/VerticesTreeData.h"

#include <limits>

namespace AWT
{
   template <class T>
   class VerticesNearestVerticesSearch : public VerticesSearch<T>
   {
   public:
      typedef ManagedAutoPointer< VerticesNearestVerticesSearch<T> > P;
      static P getInstance();

   protected:
      VerticesNearestVerticesSearch(const T in_MaxDistance = 1);
      virtual ~VerticesNearestVerticesSearch();

   public:
      virtual std::string getClassName() const;

      virtual void calculateBoxDistanceBounds2(typename OEKDTree::OEKDTreeBranch<T,3>* in_Branch, AWT::OEKDTree::SqDistBounds<T>& in_Bounds) const;
      virtual bool shouldCheck(typename AWT::OEKDTree::OEKDTreeBranch<T,3>* in_Branch, const AWT::OEKDTree::SqDistBounds<T>& in_Bounds) const;

      virtual void checkObject(const int in_Index);

      virtual void setTestPoint(const T* in_TestPoint );
      virtual void getTestPoint(T* out_TestPoint) const;

      virtual void setRadius(const T in_Radius);
      virtual T    getRadius() const;

      virtual MeshIndex getNumberOfPointsFound() const;
      virtual MeshIndex getPointFoundIndex(MeshIndex idx) const;

      virtual void reset();

   protected:
      std::vector<int> m_NearestPoints;
      int m_NearestIndex;
      T   m_NearestPoint[3];

      T m_InitMaxDistance2;
      T m_MaxDistance2;
      T m_TestPoint[3];
   };
}

template <class T>
GETNAMEMACRO(AWT::VerticesNearestVerticesSearch<T>);

template <class T>
typename AWT::VerticesNearestVerticesSearch<T>::P AWT::VerticesNearestVerticesSearch<T>::getInstance()
{
   AUTOGETINSTANCE(AWT::VerticesNearestVerticesSearch<T>, ());
}

template <class T>
AWT::VerticesNearestVerticesSearch<T>::VerticesNearestVerticesSearch(const T in_MaxDistance)
{
   m_InitMaxDistance2 = in_MaxDistance*in_MaxDistance;

   reset();
}

template <class T>
AWT::VerticesNearestVerticesSearch<T>::~VerticesNearestVerticesSearch()
{

}

template <class T>
void AWT::VerticesNearestVerticesSearch<T>::calculateBoxDistanceBounds2(typename AWT::OEKDTree::OEKDTreeBranch<T,3>* in_Branch, AWT::OEKDTree::SqDistBounds<T>& in_bounds) const
{
   boxChecked();
   T ret = 0;
   T tmp;

   for (int i = 0; i < 3; i++)
   {
      tmp = std::min(std::max(m_TestPoint[i], in_Branch->getMinimumBound(i)), in_Branch->getMaximumBound(i));

      tmp -= m_TestPoint[i];

      ret += tmp*tmp;

      if (ret > m_MaxDistance2)
         break;
   }

   in_bounds.lower = ret;
   in_bounds.upper = std::numeric_limits<T>::max();
}

template <class T>
bool AWT::VerticesNearestVerticesSearch<T>::shouldCheck(typename AWT::OEKDTree::OEKDTreeBranch<T,3>* in_Branch, const AWT::OEKDTree::SqDistBounds<T>& in_Bounds) const
{
   return in_Bounds.lower < m_MaxDistance2;
}

template <class T>
void AWT::VerticesNearestVerticesSearch<T>::checkObject(const int in_Index)
{
   objectChecked();

   T dist2 = 0;
   T tmp;

   T pnt[3];
   m_Data->getPosition(in_Index, pnt);

   for (int i = 0; i < 3; ++i)
   {
      tmp = pnt[i] - m_TestPoint[i];
      dist2 += tmp*tmp;

      if (dist2 >= m_MaxDistance2)
         return;
   }

   m_NearestPoints.push_back(in_Index);
}

template <class T>
void AWT::VerticesNearestVerticesSearch<T>::setTestPoint(const T* in_TestPoint )
{
   for (int i = 0; i < 3; i++)
      m_TestPoint[i] = in_TestPoint[i];
}

template <class T>
void AWT::VerticesNearestVerticesSearch<T>::getTestPoint(T* out_TestPoint) const
{
   for (int i = 0; i < 3; i++)
      out_TestPoint[i] = m_TestPoint[i];
}

template <class T>
AWT::MeshIndex AWT::VerticesNearestVerticesSearch<T>::getNumberOfPointsFound() const
{
   return static_cast<MeshIndex>(m_NearestPoints.size());
}

template <class T>
AWT::MeshIndex AWT::VerticesNearestVerticesSearch<T>::getPointFoundIndex(AWT::MeshIndex idx) const
{
   return m_NearestPoints[idx];
}

template <class T>
void AWT::VerticesNearestVerticesSearch<T>::setRadius(const T in_Radius)
{
   m_MaxDistance2 = m_InitMaxDistance2 = in_Radius*in_Radius;
}

template <class T>
T    AWT::VerticesNearestVerticesSearch<T>::getRadius() const
{
   return sqrt(m_MaxDistance2);
}

template <class T>
void AWT::VerticesNearestVerticesSearch<T>::reset()
{
   VerticesSearch<T>::reset();

   m_NearestPoints.clear();
   m_MaxDistance2 = m_InitMaxDistance2;
}

#endif __POINTSNEARESTPOINTSSEARCH_H__