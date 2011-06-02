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
#ifndef __MESHSIMILARITY_H__
#define __MESHSIMILARITY_H__

#include "ObjectManagement/ManagedObject.h"

namespace AWT
{
   template <class T>
   class Mesh;

   template <class T>
   class MeshSimilarity : public ManagedObject
   {
   public:
      typedef ManagedAutoPointer<MeshSimilarity<T> > P;

   protected:
      MeshSimilarity(typename Mesh<T>::P meshA, typename Mesh<T>::P meshB, const T aodThresh, const unsigned int subdivs, const bool inCorr);
      virtual ~MeshSimilarity();

   public:
      static P getInstance(typename Mesh<T>::P meshA, typename Mesh<T>::P meshB, const T aodThresh, const unsigned int subdivs, const bool inCorr = false);

      std::string getClassName() const;

      T getMeanDistance()    const;
      T getRmsDistance()     const;
      T getMaximumDistance() const;
      T getAreaOfDeviation() const;

      T getAreaOfDeviationThreshold() const;
      unsigned int getNumberOfSubdivisions() const;

   protected:
      struct D;
      D* m_D;
   };
}

#endif // __MESHSIMILARITY_H__