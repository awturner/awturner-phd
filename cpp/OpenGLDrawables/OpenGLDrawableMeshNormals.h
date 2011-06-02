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
#ifndef __OPENGLDRAWABLEMESHNORMALS_H__
#define __OPENGLDRAWABLEMESHNORMALS_H__
#pragma warning( disable: 4250 )

#include "OpenGLDrawDisplayList.h"

#include "DrawableFramework/DrawableMeshNormals.h"

#include "DrawableFramework/DrawMaterial.h"

#include "Mesh/Mesh.h"

namespace AWT
{
   template <class T>
   class OpenGLDrawableMeshNormals : public OpenGLDrawDisplayList, public virtual DrawableMeshNormals<T>
   {
   public:
      typedef ManagedAutoPointer<OpenGLDrawableMeshNormals<T>> P;

   protected:
      OpenGLDrawableMeshNormals( typename Mesh<T>::P mesh );
      virtual ~OpenGLDrawableMeshNormals( );

   public:
      static P getInstance( typename Mesh<T>::P mesh );

      ModifiedTime getChildModifiedTime( );

      virtual std::string getClassName( ) const;

      virtual typename Mesh<T>::P getMesh( );

      virtual void setMesh( typename Mesh<T>::P mesh );

      virtual DrawNormalsType getDrawNormalsType( ) const;
      virtual void setDrawNormalsType( const DrawNormalsType da );

      virtual T getScale( ) const;
      virtual void setScale( const T scale );

      virtual DrawMaterial::P getMaterial( );
      virtual void setMaterial( DrawMaterial::P mat );

   protected:
      void buildList( DrawContext::P context );

      void updateBounds( );

      double getBoundImpl( unsigned int i );

   protected:
      struct D;
      D* m_D;
   };
}

#pragma warning( default: 4250 )
#endif // __OPENGLDRAWABLEMESHNORMALS_H__