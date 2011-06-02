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
#ifndef __PRIMITIVES_AXISALIGNEDBOX_H__
#define __PRIMITIVES_AXISALIGNEDBOX_H__

#include "GeometricPrimitive.h"

namespace AWT
{
   namespace GeometricPrimitives
   {
      template <class T>
      class AxisAlignedBox : public GeometricPrimitive<T>
      {
         friend class GeometryQueries<T>;

      public:
         static AxisAlignedBox* getInstance(const T* in_bounds);

         void getBounds(T* out_bounds) const;
         void setBounds(const T* in_bounds);
         
         T getMinimumBound(const unsigned int axis) const;
         T getMaximumBound(const unsigned int axis) const;

         void setMinimumBound(const unsigned int axis, T value);
         void setMaximumBound(const unsigned int axis, T value);

      protected:
         AxisAlignedBox(const T* in_bounds);
         virtual ~AxisAlignedBox();

         T bounds[6];
      };
   }
}

namespace AWT
{
   namespace GeometricPrimitives
   {
      template <class T>
      AxisAlignedBox<T>* AxisAlignedBox<T>::getInstance(const T* in_bounds)
      {
         return new AxisAlignedBox(in_bounds);
      }

      template <class T>
      void AxisAlignedBox<T>::getBounds(T* out_bounds) const
      {
         for (int i = 0; i < 6; ++i)
            out_bounds[i] = bounds[i];
      }

      template <class T>
      void AxisAlignedBox<T>::setBounds(const T in_bounds[])
      {
         for (int i = 0; i < 6; ++i)
            bounds[i] = in_bounds[i];

      }

      template <class T>
      T AxisAlignedBox<T>::getMinimumBound(const unsigned int axis) const
      {
         return bounds[3*axis+0];
      }

      template <class T>
      T AxisAlignedBox<T>::getMaximumBound(const unsigned int axis) const
      {
         return bounds[3*axis+1];
      }

      template <class T>
      void AxisAlignedBox<T>::setMinimumBound(const unsigned int axis, T value)
      {
         bounds[3*axis+0] = value;
         modified();
      }

      template <class T>
      void AxisAlignedBox<T>::setMaximumBound(const unsigned int axis, T value)
      {
         bounds[3*axis+1] = value;
         modified();
      }

      template <class T>
      AxisAlignedBox<T>::AxisAlignedBox(const T* in_bounds)
      {
         for (int i = 0; i < 6; ++i)
            bounds[i] = in_bounds[i];
      }

      template <class T>
      AxisAlignedBox<T>::~AxisAlignedBox()
      {
      }
   }
}
#endif // __PRIMITIVES_AXISALIGNEDBOX_H__