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
#include "FacesInRadiusSearch.h"

#include "FacesInRadiusCallback.h"

#include "ObjectEKDTree/OEKDTreeBranch.h"

#include "MeshSearching/BoxQueries.h"
#include "MeshSearching/FaceQueries.h"
#include "MeshSearching/PointQueries.h"

template <class T>
struct AWT::FacesInRadiusSearch<T>::D
{
   T                                    m_RadiusSquared;
   typename FacesInRadiusCallback<T>::P m_Callback;

   T                                    m_TestPoint[3];

   typename OEKDTreeData<T,3>::P        data;
};

template <class T>
AWT::FacesInRadiusSearch<T>::FacesInRadiusSearch()
{
   m_D = new D;

   m_D->m_RadiusSquared = 1;
}

template <class T>
AWT::FacesInRadiusSearch<T>::~FacesInRadiusSearch()
{
   delete m_D;
}

template <class T>
GETNAMEMACRO(AWT::FacesInRadiusSearch<T>);

template <class T>
typename AWT::FacesInRadiusSearch<T>::P AWT::FacesInRadiusSearch<T>::getInstance()
{
   AUTOGETINSTANCE(FacesInRadiusSearch<T>, ());
}

template <class T>
void AWT::FacesInRadiusSearch<T>::setCallback(typename FacesInRadiusCallback<T>::P callback)
{
   m_D->m_Callback = callback;
}

template <class T>
void AWT::FacesInRadiusSearch<T>::calculateBoxDistanceBounds2(typename AWT::OEKDTree::OEKDTreeBranch<T,3>* in_Branch, typename AWT::OEKDTree::SqDistBounds<T>& bounds) const
{
   boxChecked();

   bounds.lower = BoxQueries<T>::getClosestDistance2(m_D->m_TestPoint, in_Branch, m_D->m_RadiusSquared);
   bounds.upper = std::numeric_limits<T>::max();
}

template <class T>
void AWT::FacesInRadiusSearch<T>::setTreeData(typename AWT::OEKDTree::OEKDTreeData<T,3>::P data)
{
   m_D->data = data;
}

template <class T>
typename AWT::OEKDTree::OEKDTreeData<T,3>::P AWT::FacesInRadiusSearch<T>::getTreeData()
{
   return m_D->data;
}

template <class T>
void AWT::FacesInRadiusSearch<T>::checkObject(const int in_Index)
{
   objectChecked();

   T nearestOnTriangle[3];

   FaceQueries<T>::getNearestPointOnTriangle(m_D->m_TestPoint, m_D->data, in_Index, nearestOnTriangle);
   
   T dist2 = PointQueries<T>::getDistanceSquared(m_D->m_TestPoint, nearestOnTriangle, m_D->m_RadiusSquared);

   if (dist2 >= m_D->m_RadiusSquared)
      return;

   if (*m_D->m_Callback != 0)
      m_D->m_Callback->faceFound(m_D->data, in_Index);
}

template <class T>
bool AWT::FacesInRadiusSearch<T>::shouldCheck(typename AWT::OEKDTree::OEKDTreeBranch<T,3>* in_Branch, const AWT::OEKDTree::SqDistBounds<T>& bounds) const
{
   return bounds.lower < m_D->m_RadiusSquared;
}

template <class T>
void AWT::FacesInRadiusSearch<T>::setRadius(const T radius)
{
   m_D->m_RadiusSquared = radius*radius;
}

template <class T>
T    AWT::FacesInRadiusSearch<T>::getRadius() const
{
   return sqrt(m_D->m_RadiusSquared);
}

template <class T>
void AWT::FacesInRadiusSearch<T>::setTestPoint(const T* in_TestPoint )
{
   for (int i = 0; i < 3; ++i)
      m_D->m_TestPoint[i] = in_TestPoint[i];
}

template <class T>
void AWT::FacesInRadiusSearch<T>::getTestPoint(T* out_TestPoint) const
{
   for (int i = 0; i < 3; ++i)
      out_TestPoint[i] = m_D->m_TestPoint[i];
}

template class AWT::FacesInRadiusSearch<double>;
template class AWT::FacesInRadiusSearch<float>;