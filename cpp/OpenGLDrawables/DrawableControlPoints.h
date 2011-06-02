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
#ifndef __DRAWABLECONTROLPOINTS_H__
#define __DRAWABLECONTROLPOINTS_H__

#include "NonRigidAlignment/ControlPoint.h"
#include "NonRigidAlignment/ControlPointSet.h"

#include "OpenGLDrawableImpl.h"
#include "OpenGLHeaders.h"


namespace AWT
{
   class DrawMaterial;

   template <class T>
   class DrawableControlPoints : public OpenGLDrawableImpl
   {
   public:
      typedef ManagedAutoPointer<DrawableControlPoints> P;

   protected:
      DrawableControlPoints(typename ControlPointSet<T>::P cps);

      ~DrawableControlPoints();

   public:
      static P getInstance(typename ControlPointSet<T>::P cps);

      virtual std::string getClassName() const;

      virtual void getBounds(double* out_Bounds);

      virtual void drawImpl(DrawContext::P context);

      virtual void setDrawThreshold(const T in_Val);

      virtual T getDrawThreshold() const;

      virtual DrawMaterial::P getMaterial();
      virtual void setMaterial(DrawMaterial::P mat);

   protected:
      struct D;
      D* m_D;
   };
}

#endif // __DRAWABLECONTROLPOINTS_H__