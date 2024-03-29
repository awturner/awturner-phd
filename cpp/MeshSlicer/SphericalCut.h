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
#ifndef __SPHERICALCUT_H__
#define __SPHERICALCUT_H__

#include "CuttingFunction.h"

namespace AWT
{
   template <class T>
   class SphericalCut : public CuttingFunction<T>
   {
   public:
      typedef ManagedAutoPointer<SphericalCut<T> > P;

   protected:
      SphericalCut();
      virtual ~SphericalCut();

   public:
      static P getInstance();
      virtual std::string getClassName() const;

      virtual void getCentre(T* out_Centre) const;
      virtual void setCentre(const T* in_Centre);

      virtual T getRadius() const;
      virtual void setRadius(const T in_Radius);

      virtual bool accept(T* vtx);

   protected:
      struct D;
      D* m_D;

   };
}

#endif // __SPHERICALCUT_H__