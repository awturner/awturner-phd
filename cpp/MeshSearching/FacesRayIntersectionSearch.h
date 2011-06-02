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
#ifndef __FACESRAYINTERSECTIONSEARCH_H__
#define __FACESRAYINTERSECTIONSEARCH_H__

#include "FacesSearch.h"

#include "Mesh/TriangleProjection.h"

#include "MeshSearching/FacesTreeData.h"
#include "MeshSearching/FaceQueries.h"
#include "MeshSearching/PointQueries.h"
#include "Useful/ArrayVectorFunctions.h"

#include <limits>

namespace AWT
{
   template <class T>
   class FacesRayIntersectionSearch : public FacesSearch<T>
   {
      template <class T, unsigned char K = 3>
      friend class OEKDTree::OEKDTree;

      template <class T, unsigned char K = 3>
      friend class OEKDTree::OEKDTreeBranch;

   public:
      typedef ManagedAutoPointer<FacesRayIntersectionSearch<T> > P;

      static P getInstance();

   protected:
      FacesRayIntersectionSearch(const T in_MaxDistance = std::numeric_limits<T>::infinity());
      virtual ~FacesRayIntersectionSearch();

   public:
      virtual void calculateBoxDistanceBounds2(typename OEKDTree::OEKDTreeBranch<T,3>* in_Branch, AWT::OEKDTree::SqDistBounds<T>& in_Bounds) const;
      virtual bool shouldCheck(typename AWT::OEKDTree::OEKDTreeBranch<T,3>* in_Branch, const AWT::OEKDTree::SqDistBounds<T>& in_Bounds) const;

      virtual void checkObject(const int in_Index);

      virtual void setTestPoint(const T* in_TestPoint );
      virtual void getTestPoint(T* out_TestPoint) const;

      virtual void setTestNormal(const T* in_TestNormal);
      virtual void getTestNormal(T* out_TestNormal) const;

      virtual int  getNearestPoint(T* out_NearestPoint = 0) const;

      virtual void reset();

      virtual bool isAllowedBackwards() const;
      virtual void setAllowedBackwards(const bool v);

      virtual bool isEnforceNormalCompatibility() const;
      virtual void setEnforceNormalCompatibility(const bool v);

      virtual std::string getClassName() const;
   protected:

      int m_NearestIndex;
      T   m_NearestPoint[3];

      T m_InitMaxDistance2;
      T m_MaxDistance2;

      T m_TestPoint[3];
      T m_TestNormal[3];

      bool m_AllowedBackwards;
      bool m_EnforceNormalCompatibility;

      T m_Tolerance;
   };
}

#include "Useful/ArrayVectorFunctions.h"

template <class T>
GETNAMEMACRO(AWT::FacesRayIntersectionSearch<T>);

template <class T>
typename AWT::FacesRayIntersectionSearch<T>::P AWT::FacesRayIntersectionSearch<T>::getInstance()
{
   AUTOGETINSTANCE(AWT::FacesRayIntersectionSearch<T>, ());
}

template <class T>
AWT::FacesRayIntersectionSearch<T>::FacesRayIntersectionSearch(const T in_MaxDistance)
{
   m_InitMaxDistance2 = in_MaxDistance*in_MaxDistance;

   m_AllowedBackwards = false;
   m_EnforceNormalCompatibility = false;

   m_Tolerance = 1e-2;

   reset();
}

template <class T>
AWT::FacesRayIntersectionSearch<T>::~FacesRayIntersectionSearch()
{
}

template <class T>
void AWT::FacesRayIntersectionSearch<T>::calculateBoxDistanceBounds2(typename AWT::OEKDTree::OEKDTreeBranch<T,3>* in_Branch, AWT::OEKDTree::SqDistBounds<T>& in_bounds) const
{
   boxChecked();

   int intersectionsFound = 0;
   T projected;

   T bounds[6];

   T  bestDistances[2];

   T lambda;

   int ax, i, j;

   bool inside = true;

   for (ax = 0; ax < 3; ax++)
   {
      bounds[2*ax+0] = in_Branch->getMinimumBound(ax);
      bounds[2*ax+1] = in_Branch->getMaximumBound(ax);

      if (m_TestPoint[ax] < bounds[2*ax+0] || m_TestPoint[ax] > bounds[2*ax+1])
         inside = false;
   }

   if (inside)
   {
      in_bounds.lower = 0;
      in_bounds.upper = std::numeric_limits<T>::max();
      return;
   }

   for (ax = 0; ax < 3 && intersectionsFound < 2; ax++)
   {
      if (m_TestNormal[ax] == 0)
         continue;

      for (i = 0; i < 2 && intersectionsFound < 2; i++)
      {
         lambda = (bounds[2*ax+i] - m_TestPoint[ax]) / m_TestNormal[ax];

         bestDistances[intersectionsFound] = lambda*lambda;

         for (j = 0; j < 3; j++)
         {
            // Project the point onto the plane
            projected = lambda * m_TestNormal[j];

            //bestDistances[intersectionsFound] += lambda*lambda;

            // Fast break on distance

            if (bestDistances[intersectionsFound] > m_MaxDistance2)
               break;

            // Don't want to check on the axis in question in case of round off errors
            if (j != ax)
            {
               projected += m_TestPoint[j];

               if (projected < bounds[2*j+0] || projected > bounds[2*j+1])
                  break;
            }
         }

         // Unless J==K, we broke out of the last loop, so the point isn't in the bounds
         if (j == 3)
         {
            ++intersectionsFound;

            // There can only be two intersections of a line with the hypercuboid
            if (intersectionsFound == 2)
            {
               if (bestDistances[1] < bestDistances[0])
                  bestDistances[0] = bestDistances[1];
            }
         }
      }
   }

   if (intersectionsFound > 0)
      in_bounds.lower = bestDistances[0];
   else
      in_bounds.lower = std::numeric_limits<T>::max();

   in_bounds.upper = std::numeric_limits<T>::max();
}

template <class T>
bool AWT::FacesRayIntersectionSearch<T>::shouldCheck(typename AWT::OEKDTree::OEKDTreeBranch<T,3>* in_Branch, const AWT::OEKDTree::SqDistBounds<T>& in_Bounds) const
{
   return in_Bounds.lower < m_MaxDistance2;
}

template <class T>
void AWT::FacesRayIntersectionSearch<T>::checkObject(const int in_Index)
{
   objectChecked();

   T offset[3];
   T point1[3];
   T point2[3];

   m_Data->getVertex(in_Index, 0, offset);
   m_Data->getVertex(in_Index, 1, point1);
   m_Data->getVertex(in_Index, 2, point2);

   T ws[2];
   TriangleProjection<T>::projectOntoTriangle(offset, point1, point2, m_TestPoint, ws);
   
   if (ws[0] < -m_Tolerance || ws[1] < -m_Tolerance || ws[0]+ws[1] > 1+m_Tolerance)
      return;

   T intPoint[3];
   TriangleProjection<T>::projectFromTriangle(offset, point1, point2, ws, intPoint);

   const T norm2 = deltaNormSquared(intPoint, m_TestPoint, 3);
   if (norm2 < m_MaxDistance2)
   {
      m_NearestPoint[0] = intPoint[0];
      m_NearestPoint[1] = intPoint[1];
      m_NearestPoint[2] = intPoint[2];

      m_MaxDistance2 = norm2;
      m_NearestIndex = in_Index;
   }
}

template <class T>
void AWT::FacesRayIntersectionSearch<T>::setTestPoint(const T* in_TestPoint )
{
   for (int i = 0; i < 3; i++)
      m_TestPoint[i] = in_TestPoint[i];
}

template <class T>
void AWT::FacesRayIntersectionSearch<T>::getTestPoint(T* out_TestPoint) const
{
   for (int i = 0; i < 3; i++)
      out_TestPoint[i] = m_TestPoint[i];
}

template <class T>
void AWT::FacesRayIntersectionSearch<T>::setTestNormal(const T* in_TestNormal )
{
   for (int i = 0; i < 3; i++)
      m_TestNormal[i] = in_TestNormal[i];
}

template <class T>
void AWT::FacesRayIntersectionSearch<T>::getTestNormal(T* out_TestNormal) const
{
   for (int i = 0; i < 3; i++)
      out_TestNormal[i] = m_TestNormal[i];
}

template <class T>
int  AWT::FacesRayIntersectionSearch<T>::getNearestPoint(T* out_NearestPoint = 0) const
{
   if (m_NearestIndex != -1 && out_NearestPoint != 0)
   {
      for (int i = 0; i < 3; i++)
         out_NearestPoint[i] = m_NearestPoint[i];
   }

   return m_NearestIndex;
}

template <class T>
void AWT::FacesRayIntersectionSearch<T>::reset()
{
   FacesSearch<T>::reset();

   m_NearestIndex = -1;
   m_MaxDistance2 = m_InitMaxDistance2;
}

template <class T>
bool AWT::FacesRayIntersectionSearch<T>::isAllowedBackwards() const
{
   return m_AllowedBackwards;
}

template <class T>
void AWT::FacesRayIntersectionSearch<T>::setAllowedBackwards(const bool v)
{
   m_AllowedBackwards = v;
}

template <class T>
bool AWT::FacesRayIntersectionSearch<T>::isEnforceNormalCompatibility() const
{
   return m_EnforceNormalCompatibility;
}

template <class T>
void AWT::FacesRayIntersectionSearch<T>::setEnforceNormalCompatibility(const bool v)
{
   m_EnforceNormalCompatibility = v;
}

#endif __FACESRAYINTERSECTIONSEARCH_H__