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
#ifndef __DRAWABLEMESH_H__
#define __DRAWABLEMESH_H__

#include "Drawable.h"

#include "Mesh/Mesh.h"

#include "DrawMaterial.h"

namespace AWT
{
   enum DrawMeshType { DRAWAS_POINTS, DRAWAS_LINES, DRAWAS_SOLID };

   template <class T>
   class DrawableMesh : public virtual Drawable
   {
   public:
      typedef ManagedAutoPointer<DrawableMesh<T> > P;

   public:
      virtual typename Mesh<T>::P getMesh() = 0;
      virtual void setMesh(typename Mesh<T>::P mesh) = 0;

      virtual DrawMeshType getDrawAs() const = 0;
      virtual void setDrawAs(const DrawMeshType da) = 0;

      virtual float getLineWidth() const = 0;
      virtual void  setLineWidth(const float v) = 0;

      virtual float getPointSize() const = 0;
      virtual void setPointSize(const float v) = 0;

      virtual DrawMaterial::P getMaterial() = 0;
      virtual void setMaterial(DrawMaterial::P mat) = 0;

      virtual bool isAutoNormals() const = 0;
      virtual void setAutoNormals(const bool v) = 0;

      virtual bool isHideBackFaces() const = 0;
      virtual void setHideBackFaces(const bool v) = 0;
   };
}

#endif // __DRAWABLEMESH_H__