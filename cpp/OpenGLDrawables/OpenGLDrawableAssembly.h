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
#if 0
#ifndef __OPENGLDRAWABLEASSEMBLY_H__
#define __OPENGLDRAWABLEASSEMBLY_H__

#include "DrawableFramework/DrawableAssembly.h"

namespace AWT
{
   class OpenGLDrawableAssembly : public virtual DrawableAssembly
   {
   public:
      typedef ManagedAutoPointer<OpenGLDrawableAssembly> P;

   protected:
      OpenGLDrawableAssembly();
      virtual ~OpenGLDrawableAssembly();

   public:
      static P getInstance();
      std::string getClassName() const;

      virtual void add(const std::string& name, Drawable::P drawable);
      virtual void remove(Drawable::P drawable);
      virtual void clear();

      virtual void setVisible(const bool v);
      virtual bool isVisible() const;

      virtual void getBounds(double* out_Bounds);

      virtual void draw(DrawContext::P context, const bool transparentPass);

      virtual unsigned int getNumberOfChildren() const;
      virtual Drawable::P getChild(const unsigned int i);
      virtual std::string getChildName(const unsigned int i);

      virtual void setParent(DrawableAssembly* p);
      virtual DrawableAssembly* getParent();

   protected:
      struct D;
      D* m_D;
   };
}

#endif // __OPENGLDRAWABLEASSEMBLY_H__
#endif