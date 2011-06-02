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
#ifndef __DRAWABLEQUIVER_H__
#define __DRAWABLEQUIVER_H__

#include "Drawable.h"

#include "DrawMaterial.h"

#include "Mesh/Tuples.h"

namespace AWT
{
   template <class T>
   class DrawableQuiver : public virtual Drawable
   {
   public:
      typedef ManagedAutoPointer<DrawableQuiver<T> > P;

      virtual typename Tuples<T>::P getData() = 0;
      virtual void setData(typename Tuples<T>::P points) = 0;

      virtual typename Tuples<T>::P getData2() = 0;
      virtual void setData2(typename Tuples<T>::P points) = 0;

      virtual bool isRelative() const = 0;
      virtual void setRelative(const bool v) = 0;

      virtual float getPointSize() = 0;
      virtual void setPointSize(const float s) = 0;

      virtual float getLineWidth() = 0;
      virtual void setLineWidth(const float s) = 0;

      virtual DrawMaterial::P getMaterial() = 0;
      virtual void setMaterial(DrawMaterial::P mat) = 0;
   };
}

#endif // __DRAWABLEQUIVER_H__