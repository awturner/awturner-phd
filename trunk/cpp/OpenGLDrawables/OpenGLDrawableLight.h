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
#ifndef __OPENGLDRAWABLELIGHT_H__
#define __OPENGLDRAWABLELIGHT_H__

#include "DrawableFramework/DrawableLight.h"
#include "DrawableFramework/Light.h"

#include <limits>

#include "OpenGLHeaders.h"

namespace AWT
{
   class OpenGLDrawableLight : public DrawableLight
   {
   public:
      typedef ManagedAutoPointer<OpenGLDrawableLight> P;

   protected:
      OpenGLDrawableLight(Light::P light)
      {
         m_Light = light;
         m_Visible = true;
         m_Parent = 0;
      }

      ~OpenGLDrawableLight()
      {
      }

   public:
      static P getInstance(Light::P light)
      {
         AUTOGETINSTANCE(OpenGLDrawableLight, (light));
      }

      virtual void setParent(DrawableAssembly* p)
      {
         m_Parent = p;
         modified();
      }

      virtual DrawableAssembly* getParent()
      {
         return m_Parent;
      }

      virtual void setLight(Light::P light)
      {
         m_Light = light;
         modified();
      }

      virtual Light::P getLight()
      {
         return m_Light;
      }

      virtual std::string getClassName() const
      {
         return "AWT::OpenGLDrawableLight";
      }

      virtual void draw(DrawContext::P context, const bool transparentPass)
      {
         if (transparentPass)
            return;

         glDisable(GL_LIGHTING);
         glColor3f(1.f, 1.f, 0.f);
         
         glPointSize(10.f);
         
         glBegin(GL_POINTS);

         float pos[4];
         m_Light->getPosition(pos);

         if (pos[3] != 0)
            glVertex3fv(pos);

         glEnd();
      }

      virtual void getBounds(double* out_Bounds)
      {
         float pos[4];
         m_Light->getPosition(pos);

         if (pos[3] != 0)
         {
            out_Bounds[0] = pos[0];
            out_Bounds[1] = pos[1];
            out_Bounds[2] = pos[2];
         }
         else
         {
            out_Bounds[0] = out_Bounds[1] = out_Bounds[2] = std::numeric_limits<double>::quiet_NaN();
         }
      }

      void setVisible(const bool v)
      {
         this->m_Visible = v;
         modified();
      }

      bool isVisible() const
      {
         return this->m_Visible;
      }

   protected:
      AWT::Light::P m_Light;
      bool          m_Visible;
      
      DrawableAssembly* m_Parent;
   };
}

#endif // __OPENGLDRAWABLELIGHT_H__