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
#include "include/OpenGLWidgetMouseBehaviour.h"

#include "include/OpenGLWidget.h"

#include "Useful/PrintMacros.h"

AWT::OpenGLWidgetMouseBehaviour::OpenGLWidgetMouseBehaviour()
{
   m_D = new SavedState;

   for (unsigned int i = 0; i < 3; ++i)
      m_D->rotation[i] = m_D->orbit[i] = 0;

   m_D->mouse[0] = m_D->mouse[1] = 0;

   m_D->button = 0;
}

AWT::OpenGLWidgetMouseBehaviour::~OpenGLWidgetMouseBehaviour()
{
   delete m_D;
}

const AWT::OpenGLWidgetMouseBehaviour::SavedState* AWT::OpenGLWidgetMouseBehaviour::getSavedState()
{
   return m_D;
}

bool AWT::OpenGLWidgetMouseBehaviour::mousePressed(OpenGLWidget* widget, int x, int y, int button, int keys)
{
   double rot[3], orb[3];

   widget->getRotation(rot);
   widget->getOrbitPoint(orb);
   for (unsigned int i = 0; i < 3; ++i)
   {
      m_D->rotation[i] = rot[i];
      m_D->orbit[i] = orb[i];
   }

   m_D->mouse[0] = x;
   m_D->mouse[1] = y;

   m_D->distance = widget->getDistance();

   m_D->button |= button;

   return false;
}

bool AWT::OpenGLWidgetMouseBehaviour::mouseReleased(OpenGLWidget* /*widget*/, int /*x*/, int /*y*/, int button, int keys)
{
   m_D->button &= ~button;

   return false;
}
