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
#ifndef __COLOURMAPPER_H__
#define __COLOURMAPPER_H__

#include "DrawMaterial.h"

#include "Mesh/Tuples.h"

namespace AWT
{
   template <class T>
   class ColourMapper : public DrawMaterial
   {
   public:
      typedef ManagedAutoPointer<ColourMapper<T> > P;

   protected:
      ColourMapper();
      virtual ~ColourMapper();

   public:
      void setData(typename Tuples<T>::P data);
      typename Tuples<T>::P getData();

      // This is used in the bias/scaling
      void setMin(T v);
      T getMin() const;

      // This is used in the bias/scaling
      void setMax(T v);
      T getMax() const;

      // This needs to be implemented in an API-specific way
      virtual void tweak(const TweakType tw, const MeshIndex v) = 0;

   protected:
      struct D;
      D* m_D;

   };
}

#endif // __COLOURMAPPER_H__