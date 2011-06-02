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
#ifndef __OPENGLDRAWABLEMESH_H__
#define __OPENGLDRAWABLEMESH_H__
#pragma warning(disable: 4250)

#include "OpenGLDrawDisplayList.h"

#include "DrawableFramework/DrawableMesh.h"

#include "DrawableFramework/DrawMaterial.h"

#include "Mesh/Mesh.h"

namespace AWT
{
   template <class T>
   class Mesh;

   class DrawMaterial;

   template <class T>
   class OpenGLDrawableMesh : public OpenGLDrawDisplayList, public virtual DrawableMesh<T>
   {
   public:
      typedef ManagedAutoPointer<OpenGLDrawableMesh<T> > P;

   protected:
      OpenGLDrawableMesh(typename Mesh<T>::P mesh);
      virtual ~OpenGLDrawableMesh();

   public:
      static P getInstance(typename Mesh<T>::P mesh);

      ModifiedTime getChildModifiedTime();

      virtual DrawMeshType getDrawAs() const;

      virtual void setDrawAs(const DrawMeshType drawAs);

      virtual std::string getClassName() const;

      virtual typename Mesh<T>::P getMesh();

      virtual void setMesh(typename Mesh<T>::P mesh);

      virtual DrawMaterial::P getMaterial();
      virtual void setMaterial(DrawMaterial::P mat);

      virtual float getLineWidth() const;
      virtual void setLineWidth(const float v);

      virtual float getPointSize() const;
      virtual void setPointSize(const float v);

      virtual bool isAutoNormals() const;
      virtual void setAutoNormals(const bool v);

      virtual bool isHideBackFaces() const;
      virtual void setHideBackFaces(const bool v);

   protected:
      void beforeCallList(DrawContext::P context);

      void afterCallList(DrawContext::P context);

      void buildList(DrawContext::P context);

      void updateBounds();

      void updateBoundsForVertex(T* vtx);

      double getBoundImpl(unsigned int i);

   protected:
      struct D;
      D* m_D;
   };
}

#pragma warning(default: 4250)
#endif // __OPENGLDRAWABLEMESH_H__