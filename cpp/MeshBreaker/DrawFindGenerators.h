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
#ifndef __DRAWFINDGENERATORS_H__
#define __DRAWFINDGENERATORS_H__

#include "ObjectManagement/ManagedObject.h"

#include "OpenGLDrawables/OpenGLDrawableImpl.h"

#include "FindGenerators.h"

namespace AWT
{
   template <class T>
   class DrawFindGenerators : public OpenGLDrawableImpl
   {
   public:
      typedef ManagedAutoPointer<DrawFindGenerators> P;

   protected:
      DrawFindGenerators( typename FindGenerators<T>::P mb );
      virtual ~DrawFindGenerators( );

   public:
      static P getInstance( typename FindGenerators<T>::P mb  );
      virtual std::string getClassName( ) const;

      virtual DrawMaterial::P getMaterial( );
      virtual void setMaterial( DrawMaterial::P mat );

      virtual void getBounds( double* bounds );

   protected:
      virtual void drawImpl( DrawContext::P context );

      struct D;
      D* m_D;

   };
}

#endif // __DRAWFINDGENERATORS_H__