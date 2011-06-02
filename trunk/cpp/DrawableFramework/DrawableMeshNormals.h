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
#ifndef __DRAWABLEMESHNORMALS_H__
#define __DRAWABLEMESHNORMALS_H__

#include "Drawable.h"
#include "DrawMaterial.h"

#include "Mesh/Mesh.h"

namespace AWT
{
   enum DrawNormalsType { NORMALS_VERTEX, NORMALS_FACE };

   template <class T>
   class DrawableMeshNormals : public virtual Drawable
   {
   public:
      typedef ManagedAutoPointer<DrawableMeshNormals<T> > P;

   public:
      virtual typename Mesh<T>::P getMesh() = 0;
      virtual void setMesh(typename Mesh<T>::P mesh) = 0;

      virtual DrawNormalsType getDrawNormalsType() const = 0;
      virtual void setDrawNormalsType(const DrawNormalsType da) = 0;

      virtual T getScale() const = 0;
      virtual void setScale(const T scale) = 0;

      virtual DrawMaterial::P getMaterial() = 0;
      virtual void setMaterial(DrawMaterial::P mat) = 0;

   protected:
      struct D;
      D* m_D;

   };
}

#endif // __DRAWABLEMESHNORMALS_H__