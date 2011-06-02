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
#ifndef __SIMPLEMESHCONNECTIVITY_H__
#define __SIMPLEMESHCONNECTIVITY_H__

#include "ObjectManagement/ManagedObject.h"

#include "Useful/Iterator.h"

#include "Mesh.h"

namespace AWT
{
   namespace SimpleMesh
   {
      class Connectivity : public ManagedObject
      {
      protected:
         Connectivity(Mesh::P mesh);
         virtual ~Connectivity();

      public:
         typedef ManagedAutoPointer<Connectivity> P;

         typedef Container::Iterator<Index> IndexIterator;

         static P getInstance(Mesh::P mesh);
         virtual std::string getClassName() const;

         Mesh::P getMesh();

         IndexIterator::P getAdjacentVertices(const Index v) const;
         IndexIterator::P getFacesUsingVertex(const Index v) const;

         bool isVertexUsed(const Index v) const;

         bool isVertexOnBoundary(const Index v) const;

         bool isFaceOnBoundary(const Index f) const;

         bool areFacesAdjacent(const Index a, const Index b) const;

         Index getAdjacentFaces(const Index v, Index* vs) const;

         Index getBoundaryNeighbours(const Index v, Index* ns) const;

         Index getFacesUsingEdge(const Index va, const Index vb, Index* fs) const;

         Index getFaceIndex(const Index va, const Index vb, const Index vc) const;

         Index aFaceUsingVertex(const Index v) const;

      protected:
         struct D;
         D* m_D;

      };
   }
}

#endif // __SIMPLEMESHCONNECTIVITY_H__