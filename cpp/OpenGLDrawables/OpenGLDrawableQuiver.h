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
#ifndef __OPENGLDRAWABLEQUIVER_H__
#define __OPENGLDRAWABLEQUIVER_H__

#include "DrawableFramework/DrawableQuiver.h"
#include "OpenGLDrawDisplayList.h"

#pragma warning(disable: 4250)

namespace AWT
{
   template <class T>
   class OpenGLDrawableQuiver : public OpenGLDrawDisplayList, public DrawableQuiver<T>
   {
   public:
      typedef ManagedAutoPointer<OpenGLDrawableQuiver> P;

   protected:
      OpenGLDrawableQuiver(typename Tuples<T>::P points, typename Tuples<T>::P points2);
      virtual ~OpenGLDrawableQuiver();

   public:
      static P getInstance(typename Tuples<T>::P points, typename Tuples<T>::P points2);
      virtual std::string getClassName() const;

      virtual typename Tuples<T>::P getData();
      virtual void setData(typename Tuples<T>::P points);

      virtual typename Tuples<T>::P getData2();
      virtual void setData2(typename Tuples<T>::P points);

      virtual bool isRelative() const;
      virtual void setRelative(const bool v);

      virtual float getPointSize();
      virtual void setPointSize(const float s);

      virtual float getLineWidth();
      virtual void setLineWidth(const float s);

      virtual DrawMaterial::P getMaterial();
      virtual void setMaterial(DrawMaterial::P mat);

      virtual ModifiedTime getChildModifiedTime();

      virtual void buildList(DrawContext::P context);

      virtual void updateBounds();

      virtual double getBoundImpl(unsigned int bound);

   protected:
      struct D;
      D* m_D;

   };
}

#endif // __OPENGLDRAWABLEQUIVER_H__