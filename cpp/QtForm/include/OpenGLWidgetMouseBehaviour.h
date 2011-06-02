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
#ifndef __OPENGLWIDGETMOUSEBEHAVIOUR_H__
#define __OPENGLWIDGETMOUSEBEHAVIOUR_H__

#include "ObjectManagement/ManagedObject.h"

#include <QObject>

namespace AWT
{
   class OpenGLWidget;

   class OpenGLWidgetMouseBehaviour : public ManagedObject
   {
   public:
      typedef ManagedAutoPointer<OpenGLWidgetMouseBehaviour> P;
      static P getInstance( );

   protected:
      OpenGLWidgetMouseBehaviour( );
      virtual ~OpenGLWidgetMouseBehaviour( );

   public:
      virtual bool mouseMoved( OpenGLWidget* widget, int x, int y, int button, int keys ) = 0;
      virtual bool mousePressed( OpenGLWidget* widget, int x, int y, int button, int keys );
      virtual bool mouseReleased( OpenGLWidget* widget, int x, int y, int button, int keys );

      struct SavedState
      {  
         double rotation[3];
         double orbit[3];
         double distance;

         int mouse[2];
         int button;
      };

      virtual const SavedState* getSavedState( );

   protected:
      SavedState* m_D;
   };
}

#endif // __OPENGLWIDGETMOUSEBEHAVIOUR_H__