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
#ifndef __DRAWABLE_H__
#define __DRAWABLE_H__

#include "ObjectManagement/ManagedObject.h"
#include "DrawContext.h"

namespace AWT
{
   class Mutex;

   class DrawableAssembly;

   class DrawContext;

   class Drawable : public virtual ManagedObject
   {
   protected:
      Drawable( );
      virtual ~Drawable( );

   public:
      typedef ManagedAutoPointer<Drawable> P;

   public:
      virtual void draw( DrawContext::P context, const bool transparentPass ) = 0;

      virtual void setVisible( const bool v ) = 0;

      virtual bool isVisible( ) const = 0;

      virtual void getBounds( double* out_Bounds ) = 0;

      virtual void setParent( DrawableAssembly* p ) = 0;
      virtual DrawableAssembly* getParent( ) = 0;

      //static bool acquireMutexIfNotLocked( void* obj );
      //static void acquireMutex( void* obj );
      //static void releaseMutex( void* obj );

      virtual ModifiedTime getTimeObjectModified( );

      virtual void setTimeObject( ManagedObject::P obj );
      virtual ManagedObject::P getTimeObject( );

   protected:
      struct D;
      D* m_D;
      //static Mutex* ms_Mutex;
   };
}

#endif __DRAWABLE_H__