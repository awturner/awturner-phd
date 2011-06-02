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
#ifndef __FACEQUERIES_H__
#define __FACEQUERIES_H__

#include "ObjectEKDTree/OEKDTreeData.h"

#include "Mesh/Mesh.h"

namespace AWT
{
   template <class T>
   class FaceQueries
   {
   public:
      static void getNormal                (const typename OEKDTree::OEKDTreeData<T,3>::P in_Data, const int in_Index, T* out_Normal);
      static void getNearestPointOnTriangle(const T* in_TestPoint, const typename OEKDTree::OEKDTreeData<T,3>::P in_Data, const int in_Index, T* out_Nearest);
      static void getNearestPointOnTriangle(const T* in_TestPoint, const T* pnt0, const T* pnt1, const T* pnt2, T* out_Nearest);
      static void getCentroid              (const typename OEKDTree::OEKDTreeData<T,3>::P in_Data, const int in_Index, T* out_Centroid);
      static bool rayIntersectsTriangle    (const typename OEKDTree::OEKDTreeData<T,3>::P in_Data, const int in_Index, const T* origin, const T* direction, T* intersectionPoint, T* distance);
   };
}

#endif // __FACEQUERIES_H__