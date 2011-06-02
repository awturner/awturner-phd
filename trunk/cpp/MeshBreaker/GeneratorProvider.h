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
#ifndef __GENERATORPROVIDER_H__
#define __GENERATORPROVIDER_H__

#include "ObjectManagement/ManagedObject.h"

#include "HalfEdge.h"
#include "HalfEdgeMesh.h"
#include <vector>
#include "Mesh/Mesh.h"

namespace AWT
{
   template <class T>
   class GeneratorProvider : public virtual ManagedObject
   {
   public:
      typedef ManagedAutoPointer<GeneratorProvider<T> > P;

      virtual unsigned int                  getNumberOfGeneratorPairs() const = 0;
      virtual const std::vector<HalfEdge*>& getGenerator(const unsigned int pair, const unsigned int i) = 0;
      virtual typename Mesh<T>::P           getMesh() = 0;
      virtual HalfEdgeMesh::P               getHalfEdgeMesh() = 0;
   };
}

#endif // __GENERATORPROVIDER_H__