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
#ifndef __WORLDVIEWER_H__
#define __WORLDVIEWER_H__

#include "DrawableFramework/DrawContext.h"

#include "DrawableFramework/DrawableWorld.h"

#include "MouseListener.h"

namespace AWT
{
   class WorldViewer : public DrawContext
   {
   public:
      typedef ManagedAutoPointer<WorldViewer> P;

      virtual DrawableWorld::P getDrawableWorld() = 0;
      virtual void setDrawableWorld(DrawableWorld::P world) = 0;

      virtual void getOrbitPoint(double* v) const = 0;
      virtual void setOrbitPoint(const double* v) = 0;

      virtual void getRotation(double* v) const = 0;
      virtual void setRotation(const double* v) = 0;

      virtual double getDistance() const = 0;
      virtual void setDistance(const double v) = 0;

      virtual void setVisibleRadius(const double v) = 0;
   
      virtual void addMouseListener(MouseListener::P listener) = 0;
      virtual void removeMouseListener(MouseListener::P listener) = 0;

      virtual void refresh() = 0;
   };
}

#endif // __WORLDVIEWER_H__