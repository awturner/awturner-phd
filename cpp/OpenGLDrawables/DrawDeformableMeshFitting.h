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
#ifndef __DrawDeformableMeshFitting_H__
#define __DrawDeformableMeshFitting_H__


#include "DeformableMeshFitting/DeformableMeshFittingListener.h"

#include "DrawableFramework/Drawable.h"


namespace AWT
{
   template <class T>
   class DeformableMeshFitting;

   template <class T>
   class DrawableFactory;

   class DrawableAssembly;

   template <class T>
   class DrawDeformableMeshFitting : public Drawable/*, public DeformableMeshFittingListener<T>*/
   {
   public:
      typedef ManagedAutoPointer<DrawDeformableMeshFitting> P;

   protected:
      DrawDeformableMeshFitting(typename DeformableMeshFitting<T>::P dmf, typename DrawableFactory<T>::P fact);
      virtual ~DrawDeformableMeshFitting();

   public:
      static  P getInstance(typename DeformableMeshFitting<T>::P dmf, typename DrawableFactory<T>::P fact);

      void draw(DrawContext::P context, const bool transparentPass);

      void setVisible(const bool v);

      bool isVisible() const;

      void getBounds(double* out_Bounds);

      virtual std::string getClassName() const;

      virtual void correspondencesFound(typename DeformableMeshFitting<T>::P dmf);

      virtual void setParent(DrawableAssembly* assm);
      virtual DrawableAssembly* getParent();

   protected:
      struct D;
      D* m_D;
   };
}

#endif // __DrawDeformableMeshFitting_H__