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
#ifndef __NELDERMEAD_H__
#define __NELDERMEAD_H__

#include "ObjectManagement/ManagedObject.h"

#include "vnl/vnl_matrix.h"
#include "OptimizationFunction.h"

namespace AWT
{
   template <class T>
   class NelderMead : public ManagedObject
   {
   public:
      typedef ManagedAutoPointer<NelderMead> P;

   protected:
      NelderMead(typename OptimizationFunction<T>::P func);
      virtual ~NelderMead();

   public:
      static P getInstance(typename OptimizationFunction<T>::P func);
      virtual std::string getClassName() const;

      void setInitialGuess(const vnl_matrix<T>& vertex, const vnl_matrix<T>& stdev);

      T getRange() const;

      vnl_matrix<T> getCentroid() const;

      vnl_matrix<T> getSimplex() const;

      void initialize();

      void step();

      T getAlpha() const;
      void setAlpha(const T v);

      T getGamma() const;
      void setGamma(const T v);

      T getRho() const;
      void setRho(const T v);

      T getSigma() const;
      void setSigma(const T v);

   protected:
      struct D;
      D* m_D;

   };
}

#endif // __NELDERMEAD_H__