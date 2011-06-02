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
#ifndef __OPENGLTRANSFORMATION_H__
#define __OPENGLTRANSFORMATION_H__

#include "DrawableFramework/Transformation.h"

namespace AWT
{
   template <class T>
   class OpenGLTransformation : public Transformation<T>
   {
   public:
      typedef ManagedAutoPointer<OpenGLTransformation> P;

   protected:
      OpenGLTransformation( Drawable::P d, typename Pose<T>::P pose = 0 );

      virtual ~OpenGLTransformation( );

   public:
      static P getInstance( Drawable::P d, typename Pose<T>::P pose = 0 );

      virtual void draw( DrawContext::P context, const bool transparentPass );

      virtual void setVisible( const bool v );

      virtual bool isVisible( ) const;

      virtual void getBounds( double* out_Bounds );

      virtual void setDrawable( Drawable::P d );

      virtual Drawable::P getDrawable( );

      virtual void setPose( typename Pose<T>::P pose );

      virtual typename Pose<T>::P getPose( );

      virtual std::string getClassName( ) const;

      virtual void setParent( DrawableAssembly* p );

      virtual DrawableAssembly* getParent( );

   protected:
      struct D;
      D* m_D;
   };
}

#endif // __OPENGLTRANSFORMATION_H__