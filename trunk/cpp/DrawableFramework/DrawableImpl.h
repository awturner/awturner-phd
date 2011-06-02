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
#ifndef __DRAWABLEIMPL_H__
#define __DRAWABLEIMPL_H__

#include "DrawableFramework/Drawable.h"

#include "DrawMaterial.h"

namespace AWT
{
   class DrawableAssembly;

   class DrawableImpl : public virtual Drawable
   {
   public:
      typedef ManagedAutoPointer<DrawableImpl> P;

   protected:
      DrawableImpl( );

      virtual ~DrawableImpl( );

   public:
      void draw( DrawContext::P context, const bool transparentPass );

      virtual bool isVisible( ) const;

      virtual void setVisible( const bool vis );

      virtual DrawMaterial::P getMaterial( ) = 0;

      virtual void setMaterial( DrawMaterial::P material ) = 0;

      virtual void setParent( DrawableAssembly* p );

      virtual DrawableAssembly* getParent( );

   protected:
      virtual void drawImpl( DrawContext::P context ) = 0;

      struct D;
      D* m_D;
   };
}

#endif // __DRAWABLEIMPL_H__