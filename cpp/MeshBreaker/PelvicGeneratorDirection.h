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
#ifndef __PELVICGENERATORDIRECTION_H__
#define __PELVICGENERATORDIRECTION_H__

#include "GeneratorProvider.h"

#include "HalfEdge.h"
#include "HalfEdgeVertex.h"

namespace AWT
{
   template <class T>
   class PelvicGeneratorDirection : public GeneratorProvider<T>
   {
   public:
      typedef ManagedAutoPointer<PelvicGeneratorDirection> P;

   protected:
      PelvicGeneratorDirection(typename GeneratorProvider<T>::P fg, const bool destructive);
      virtual ~PelvicGeneratorDirection();

   public:
      static P getInstance(typename GeneratorProvider<T>::P fg, const bool destructive);
      virtual std::string getClassName() const;

      void getBasis(T* origin, T* x, T* y, T* z) const;

      void normalizeMesh(int desiredSide, int desiredObGeneratorDirection, int desiredPubGeneratorDirection);

      typename Mesh<T>::P getMesh();
      const std::vector< HalfEdge* >& getObturatorGenerator();
      const std::vector< HalfEdge* >& getPubicGenerator();

      const std::vector< HalfEdge* >& getGenerator(const unsigned int pair, const unsigned int i);
      unsigned int                    getNumberOfGeneratorPairs() const;
      HalfEdgeMesh::P                 getHalfEdgeMesh();

   protected:
      struct D;
      D* m_D;

   };
}

#endif // __PELVICGENERATORDIRECTION_H__