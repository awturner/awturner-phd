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
#ifndef __POINTPICKMOUSEBEHAVIOUR_H__
#define __POINTPICKMOUSEBEHAVIOUR_H__

#include "OpenGLWidgetMouseBehaviour.h"

#include "Mesh/Mesh.h"

#include <QObject>

#include "Notifiable.h"

namespace AWT
{
   class OpenGLWidget;

   class PointPickMouseBehaviour : public OpenGLWidgetMouseBehaviour
   {
      typedef double T;

   public:
      typedef ManagedAutoPointer<PointPickMouseBehaviour> P;

   protected:
      PointPickMouseBehaviour( );
      virtual ~PointPickMouseBehaviour( );

   public:
      static P getInstance( );
      virtual std::string getClassName( ) const;

      virtual void setTarget( Mesh<T>::P mesh );
      virtual Mesh<T>::P getTarget( );

      virtual void setNotifiable( Notifiable* not );

      virtual void getLastPicked( double pnt[3] );

      virtual bool mousePressed( OpenGLWidget* widget, int x, int y, int button, int keys );
      virtual bool mouseMoved( OpenGLWidget* widget, int x, int y, int button, int keys );

   protected:
      struct D;
      D* m_D;
   };
}

#endif // __POINTPICKMOUSEBEHAVIOUR_H__