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
#ifndef __OPENGLDRAWABLEPOINTS_H__
#define __OPENGLDRAWABLEPOINTS_H__

#include "OpenGLDrawDisplayList.h"
#include "DrawableFramework/DrawablePoints.h"

#include "DrawableFramework/DrawMaterial.h"

#pragma warning(disable: 4250)

namespace AWT
{
   template <class T>
   class OpenGLDrawablePoints : public OpenGLDrawDisplayList, public DrawablePoints<T>
   {
   public:
      typedef ManagedAutoPointer<OpenGLDrawablePoints<T> > P;

   protected:
      OpenGLDrawablePoints(typename Tuples<T>::P points);
      virtual ~OpenGLDrawablePoints();

   public:
      static P getInstance(typename Tuples<T>::P points);
      virtual std::string getClassName() const;

      virtual typename Tuples<T>::P getData();
      virtual void setData(typename Tuples<T>::P points);

      virtual typename Tuples<T>::P getNormals();
      virtual void setNormals(typename Tuples<T>::P points);

      virtual float getPointSize();
      virtual void setPointSize(const float s);

      virtual float getLineWidth();
      virtual void setLineWidth(const float s);

      virtual bool isContiguous() const;
      virtual void setContiguous(const bool v);

      virtual bool isClosed() const;
      virtual void setClosed(const bool v);

      virtual ModifiedTime getChildModifiedTime();

      virtual void buildList(DrawContext::P context);
         
      virtual void updateBounds();

      virtual double getBoundImpl(unsigned int bound);

      virtual DrawMaterial::P getMaterial();
      virtual void setMaterial(DrawMaterial::P mat);

   protected:
      struct D;
      D* m_D;

   };
}

#pragma warning(default: 4250)
#endif // __OPENGLDRAWABLEPOINTS_H__