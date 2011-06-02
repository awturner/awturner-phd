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
//#ifndef __TriangleDataElement_H__
//#define __TriangleDataElement_H__
//
//#include "EKDTree/KDTreeDataElement.h"
//
//#include <ostream>
//
//template <class T, unsigned char K>
//class AABBDataElement : public KDTreeDataElement<T,K>
//{
//public:
//   AABBDataElement(T* p, T* size);
//   ~AABBDataElement();
//
//   virtual T GetValue(int in_Axis);
//
//   virtual T GetMinBound(int in_Axis);
//
//   virtual T GetMaxBound(int in_Axis);
//
//protected:
//   T m_Point[K];
//   T m_Size[K];
//};
//
//template <class T>
//AABBDataElement<T>::AABBDataElement(T* p, T* size)
//{
//   for (int i = 0; i < K; ++i)
//   {
//      m_Point[i] = p[i];
//      m_Size[i] = size[i] / 2;
//   }
//}
//
//template <class T>
//AABBDataElement<T>::~AABBDataElement()
//{
//}
//
//template <class T>
//T AABBDataElement<T>::GetValue(int in_Axis)
//{
//   return m_Point[in_Axis];
//}
//
//template <class T>
//T AABBDataElement<T>::GetMinBound(int in_Axis)
//{
//   return m_Point[in_Axis] - m_Size[in_Axis];
//}
//
//template <class T>
//T AABBDataElement<T>::GetMaxBound(int in_Axis)
//{
//   return m_Point[in_Axis] + m_Size[in_Axis];
//}
//
//#endif // __TriangleDataElement_H__
