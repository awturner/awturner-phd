#include "CImgDispInputEventDispatcher.h"

#include "Cimg.h"

using namespace AWT;
using namespace cimg_library;

AWT::CImgDispInputEventDispatcher::CImgDispInputEventDispatcher( )
: mouseX( 0 ), mouseY( 0 ), mouseWheel( 0 ), mouseButton( 0 )
{
   for ( int i = 0; i < 256; ++i )
      keyStates[i] = false;
}

#define ADDREMOVELISTENERMACRO( Type, Vector ) \
   void AWT::CImgDispInputEventDispatcher::add##Type( Type* v ) { this->addListener<##Type>( Vector, v ); } \
   void AWT::CImgDispInputEventDispatcher::remove##Type( Type* v ) { this->removeListener<##Type>( Vector, v ); }

ADDREMOVELISTENERMACRO( MouseListener, m_MouseListeners );
ADDREMOVELISTENERMACRO( MouseMoveListener, m_MouseMoveListeners );
ADDREMOVELISTENERMACRO( MouseWheelListener, m_MouseWheelListeners );
ADDREMOVELISTENERMACRO( KeyListener, m_KeyListeners );

#undef ADDREMOVELISTENERMACRO

template <class ListenerType>
void AWT::CImgDispInputEventDispatcher::addListener( std::vector<ListenerType*>& vec, ListenerType* v )
{
   vec.push_back( v );
}

template <class ListenerType>
void AWT::CImgDispInputEventDispatcher::removeListener( std::vector<ListenerType*>& vec, ListenerType* v )
{
   for ( int i = 0; i < vec.size( ); ++i )
   {
      if ( vec[i] == v )
      {
         std::vector<ListenerType*>::iterator start  = vec.begin( );
         std::vector<ListenerType*>::iterator finish = vec.begin( );

         start += i;
         finish += i;

         vec.erase( start, finish );

         return;
      }
   }
}

void AWT::CImgDispInputEventDispatcher::notifyMousePressed(int x, int y, int button)
{
   for ( int i = 0; i < m_MouseListeners.size( ); ++i )
      m_MouseListeners[i]->mousePressed( x, y, button );
}

void AWT::CImgDispInputEventDispatcher::notifyMouseReleased(int x, int y, int button)
{
   for ( int i = 0; i < m_MouseListeners.size( ); ++i )
      m_MouseListeners[i]->mouseReleased( x, y, button );
}

void AWT::CImgDispInputEventDispatcher::notifyMouseMoved(int x, int y, int button)
{
   for ( int i = 0; i < m_MouseMoveListeners.size( ); ++i )
      m_MouseMoveListeners[i]->mouseMoved( x, y, button );
}

void AWT::CImgDispInputEventDispatcher::notifyMouseWheeled( int amount )
{
   for ( int i = 0; i < m_MouseWheelListeners.size( ); ++i )
      m_MouseWheelListeners[i]->mouseWheeled( amount );
}

void AWT::CImgDispInputEventDispatcher::notifyKeyPressed( int key )
{
   for ( int i = 0; i < m_KeyListeners.size( ); ++i )
      m_KeyListeners[i]->keyPressed( key );
}

void AWT::CImgDispInputEventDispatcher::notifyKeyReleased( int key )
{
   for ( int i = 0; i < m_KeyListeners.size( ); ++i )
      m_KeyListeners[i]->keyReleased( key );
}

#include <iostream>

void AWT::CImgDispInputEventDispatcher::processEvents( cimg_library::CImgDisplay& disp )
{
   if ( !disp.is_event )
      return;
   
   if ( mouseButton != disp.button )
   {
      int xor = mouseButton ^ disp.button;

      // Record the current state
      mouseButton = disp.button;

      for ( int i = 0; i < 3; ++i )
      {
         if ( xor & ( 1 << i ) )
         {
            // This button has changed
            if ( disp.button & ( 1 << i ) )
               notifyMousePressed( disp.mouse_x, disp.mouse_y, ( 1 << i ) );
            else
               notifyMouseReleased( disp.mouse_x, disp.mouse_y, ( 1 << i ) );
         }
      }
   }
   
   if ( mouseX != disp.mouse_x || mouseY != disp.mouse_y )
   {
      // Record the current state
      mouseX = disp.mouse_x;
      mouseY = disp.mouse_y;

      // Do the mouse moved stuff...
      notifyMouseMoved( mouseY, mouseX, mouseButton );
   }

   if ( mouseWheel != disp.wheel )
   {
      int oldWheel = mouseWheel;

      // Record the current state
      mouseWheel = disp.wheel;

      // Do the mouse wheeled stuff
      notifyMouseWheeled( mouseWheel - oldWheel );
   }

   for ( int i = 0; i < 512; ++i )
   {
      if ( disp.keys[i] || disp.released_keys[i] )
      {
         if ( disp.is_key( cimg::keySHIFTLEFT, cimg::keyA, true ) )
         {
            std::cerr << "Ctrl-A" << std::endl;
         }

         /*
         if ( disp.keys[i] )
         {
            if ( 1 || !keyStates[ disp.keys[i] ] )
            {
               std::cerr << disp.keys[i] << std::endl;
               keyStates[ disp.keys[i] ] = true;
               notifyKeyPressed( disp.keys[i] );
            }
            disp.keys[i] = 0;
         }

         if ( disp.released_keys[i] )
         {
            if ( 1 || keyStates[ disp.released_keys[i] ] )
            {
               keyStates[ disp.released_keys[i] ] = false;
               notifyKeyReleased( disp.released_keys[i] );
            }
            disp.released_keys[i] = 0;
         }
         */
      }
   }

   //disp.flush( );
}