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
#ifndef __OPENGLDRAWABLEWORLD_H__
#define __OPENGLDRAWABLEWORLD_H__
#pragma warning(disable: 4250)

#include "DrawableFramework/DrawableAssembly.h"
#include "DrawableFramework/DrawableWorld.h"

namespace AWT
{
   class OpenGLDrawableWorld : public virtual DrawableWorld
   {
   public:
      typedef ManagedAutoPointer<OpenGLDrawableWorld> P;

   protected:
      OpenGLDrawableWorld(AWT::DrawContext::P ctx);

      ~OpenGLDrawableWorld();

   public:
      static P getInstance(AWT::DrawContext::P ctx);

      virtual void draw(DrawContext::P context, const bool transparentPass);

      /*
      void getBounds(double* out_Bounds);

      void setVisible(const bool v);

      bool isVisible() const;
      */

      void setLight(Light::P light);
      Light::P getLight();

      void setBackground(Drawable::P background);
      Drawable::P getBackground();

      virtual BackgroundMode getBackgroundMode() const;
      virtual void setBackgroundMode(const BackgroundMode mode);

      void doLighting();

      virtual std::string getClassName() const;

      virtual DrawableFactory<double>* getFactory();

   protected:
      struct D;
      D* m_D;
   };
}

#endif // __OPENGLDRAWABLEWORLD_H__