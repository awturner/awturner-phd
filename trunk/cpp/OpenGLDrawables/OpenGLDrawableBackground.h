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
#ifndef __OPENGLDRAWABLEBACKGROUND_H__
#define __OPENGLDRAWABLEBACKGROUND_H__

#include "DrawableFramework/DrawableBackground.h"
#include "OpenGLDrawDisplayList.h"

namespace AWT
{
   class DrawMaterial;

   class OpenGLDrawableBackground : public virtual OpenGLDrawDisplayList
   {
   public:
      typedef ManagedAutoPointer<OpenGLDrawableBackground> P;

   protected:
      OpenGLDrawableBackground( );
      virtual ~OpenGLDrawableBackground( );

   public:
      static P getInstance( );

      virtual std::string getClassName( ) const;

      virtual void getTopColor( float* col ) const;

      virtual void setTopColor( const float* col );

      virtual void getBottomColor( float* col ) const;

      virtual void setBottomColor( const float* col );

      virtual DrawMaterial::P getMaterial( );
      virtual void setMaterial( DrawMaterial::P mat );

   protected:
      struct D;
      D* m_D;

      virtual ModifiedTime getChildModifiedTime( );

      virtual void buildList( AWT::DrawContext::P context );

      virtual void updateBounds( );

      virtual double getBoundImpl( unsigned int bound );
   };
}

#endif // __OPENGLDRAWABLEBACKGROUND_H__