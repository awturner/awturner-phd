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
#ifndef __ORCUTTINGFUNCTION_H__
#define __ORCUTTINGFUNCTION_H__

#include "CuttingFunction.h"

namespace AWT
{
   template <class T>
   class OrCuttingFunction : public CuttingFunction<T>
   {
   public:
      typedef ManagedAutoPointer<OrCuttingFunction<T> > P;

   protected:
      OrCuttingFunction(CuttingFunction<T>::P funcA, CuttingFunction<T>::P funcB);
      virtual ~OrCuttingFunction();

   public:
      static P getInstance(CuttingFunction<T>::P funcA, CuttingFunction<T>::P funcB);
      virtual std::string getClassName() const;

      virtual bool accept(T* vtx);

   protected:
      struct D;
      D* m_D;

   };
}

#endif // __ORCUTTINGFUNCTION_H__