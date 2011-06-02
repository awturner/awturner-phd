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
#ifndef __OPENGLDRAWDISPLAYLIST_H__
#define __OPENGLDRAWDISPLAYLIST_H__

#include "OpenGLDrawableImpl.h"

namespace AWT
{
   class OpenGLDrawDisplayList : public OpenGLDrawableImpl
   {
   public:
      typedef ManagedAutoPointer<OpenGLDrawDisplayList> P;

   protected:
      OpenGLDrawDisplayList();

      virtual ~OpenGLDrawDisplayList();

   public:
      virtual void getBounds(double* out_Bounds);

   protected:
      virtual ModifiedTime getChildModifiedTime() = 0;

      virtual void beforeCallList(DrawContext::P context);

      virtual void afterCallList(DrawContext::P context);

      virtual void drawImpl(DrawContext::P context);

      virtual void buildList(DrawContext::P context) = 0;
         
      virtual void updateBounds() = 0;

      virtual double getBoundImpl(unsigned int bound) = 0;

      struct D;

      D* m_D;
   };
}

#endif // __OPENGLDRAWDISPLAYLIST_H__