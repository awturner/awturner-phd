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
//#ifndef __PointDataElement_H__
//#define __PointDataElement_H__
//
//#include "EKDTree/KDTreeDataElement.h"
//
//namespace AWT
//{
//   namespace KDTree
//   {
//      template <class T, unsigned char K>
//      class PointDataElement : public KDTreeDataElement<T,K>
//      {
//      public:
//         PointDataElement(T* in_Value, bool updateable = false);
//         ~PointDataElement();
//
//         virtual void GetPosition(T* out_Pos) const;
//
//         virtual void SetValue(const int in_Axis, const T in_Value);
//
//         virtual T GetValue(const int in_Axis) const;
//
//         virtual T GetMinBound(const int in_Axis) const;
//
//         virtual T GetMaxBound(const int in_Axis) const;
//
//         virtual void update();
//
//      protected:
//         T  m_Values[K];
//         T* m_Ptr;
//         bool m_Updateable;
//      };
//   }
//}
//
//namespace AWT { namespace KDTree {
//
//template <class T, unsigned char K>
//PointDataElement<T,K>::PointDataElement(T* in_Values, bool updateable)
//{
//   // Keep a hold of the pointer, not a copy
//   m_Ptr = in_Values;
//
//   m_Updateable = true;
//   update();
//   m_Updateable = updateable;
//}
//
//template <class T, unsigned char K>
//void PointDataElement<T,K>::update()
//{
//   if (!m_Updateable)
//      return;
//
//   for (int i = 0; i < K; ++i)
//      m_Values[i] = m_Ptr[i];
//}
//
//template <class T, unsigned char K>
//PointDataElement<T,K>::~PointDataElement()
//{
//}
//
//template <class T, unsigned char K>
//void PointDataElement<T,K>::GetPosition(T* out_Value) const
//{
//   for (int i = 0; i < K; ++i)
//      out_Value[i] = m_Values[i];
//}
//
//template <class T, unsigned char K>
//void PointDataElement<T,K>::SetValue(int in_Axis, T in_Value)
//{
//   m_Values[in_Axis] = in_Value;
//}
//
//template <class T, unsigned char K>
//T PointDataElement<T,K>::GetValue(const int in_Axis) const
//{
//   return m_Values[in_Axis];
//}
//
//template <class T, unsigned char K>
//T PointDataElement<T,K>::GetMinBound(const int in_Axis) const
//{
//   return m_Values[in_Axis];
//}
//
//template <class T, unsigned char K>
//T PointDataElement<T,K>::GetMaxBound(const int in_Axis) const
//{
//   return m_Values[in_Axis];
//}
//
//}
//}
//
//#endif // __PointDataElement_H__
