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
#ifndef __OPENGLSIMPLECOLOUR_H__
#define __OPENGLSIMPLECOLOUR_H__

#include "DrawableFramework/DrawColour.h"

namespace AWT
{
   class OpenGLSimpleColour : public DrawColour
   {
   public:
      typedef ManagedAutoPointer<OpenGLSimpleColour> P;

   protected:
      OpenGLSimpleColour( float r = 1.f , float g = 1.f , float b = 1.f , float a = 1.f, bool lighting = false );

      virtual ~OpenGLSimpleColour( );

   public:
      static P getInstance( float r = 1.f , float g = 1.f , float b = 1.f , float a = 1.f, bool lighting = false );

      virtual std::string getClassName( ) const;

      virtual bool isOpaque( ) const;

      virtual void prepare( );

      virtual void unprepare( );

      virtual void tweak( const TweakType tw, const MeshIndex v );

      void setColour( const float* col );

      void setColour( const float r, const float g, const float b, const float a = 1.0 );

      void getColour( float* col ) const;

      bool isLighting( ) const;

      void setLighting( const bool v );

   protected:
      struct D;
      D* m_D;
   };
}

#endif // __OPENGLSIMPLECOLOUR_H__