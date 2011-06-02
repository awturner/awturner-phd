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
#ifndef __CIMGDISPINPUTEVENTMANAGER__
#define __CIMGDISPINPUTEVENTMANAGER__

#include "Cimg.h"
#include <vector>

namespace AWT
{
   class MouseListener
   {
   public:
      virtual void mousePressed( int x, int y, int button ) = 0;
      virtual void mouseReleased( int x, int y, int button ) = 0;
   };

   class MouseMoveListener
   {
   public:
      virtual void mouseMoved( int x, int y, int button ) = 0;
   };

   class MouseWheelListener
   {
   public:
      virtual void mouseWheeled( int amount ) = 0;
   };

   class KeyListener
   {
   public:
      virtual void keyPressed( int key ) = 0;
      virtual void keyReleased( int key ) = 0;
   };

   class CImgDispInputEventDispatcher
   {
   public:
      CImgDispInputEventDispatcher( );

      void processEvents( cimg_library::CImgDisplay& disp );

      void addMouseListener( MouseListener* v );
      void removeMouseListener( MouseListener* v );

      void addMouseWheelListener( MouseWheelListener* v );
      void removeMouseWheelListener( MouseWheelListener* v );

      void addMouseMoveListener( MouseMoveListener* v );
      void removeMouseMoveListener( MouseMoveListener* v );

      void addKeyListener( KeyListener* v );
      void removeKeyListener( KeyListener* v );

   protected:
      void notifyMousePressed( int x, int y, int button );
      void notifyMouseReleased( int x, int y, int button );
      void notifyMouseMoved( int x, int y, int button );
      void notifyMouseWheeled( int amount );
      void notifyKeyPressed( int key );
      void notifyKeyReleased( int key );

      template <class ListenerType>
      void removeListener( std::vector<ListenerType*>& vec, ListenerType* listener );

      template <class ListenerType>
      void addListener( std::vector<ListenerType*>& vec, ListenerType* listener );

      std::vector<MouseListener*>      m_MouseListeners;
      std::vector<MouseMoveListener*>  m_MouseMoveListeners;
      std::vector<MouseWheelListener*> m_MouseWheelListeners;
      std::vector<KeyListener*>        m_KeyListeners;

      // Current state of the mouse
      int mouseX, mouseY, mouseButton, mouseWheel;

      bool keyStates[256];
   };
}


#endif __CIMGDISPINPUTEVENTMANAGER__