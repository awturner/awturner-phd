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
#ifndef __MESHCONNECTIVITY_H__
#define __MESHCONNECTIVITY_H__

#include "ObjectManagement/ManagedObject.h"

#include "MeshTypeDefs.h"

#include "Useful/Iterator.h"

#include "Mesh.h"

namespace AWT
{
   template <class T>
   class MeshConnectivity : public ManagedObject
   {
   protected:
      MeshConnectivity(typename Mesh<T>::P mesh);
      virtual ~MeshConnectivity();

   public:
      typedef ManagedAutoPointer< MeshConnectivity<T> > P;

      static P getInstance(typename Mesh<T>::P mesh);
      virtual std::string getClassName() const;

      typename Mesh<T>::P getMesh();

      typename Container::Iterator<MeshIndex>::P getAdjacentVertices(const MeshIndex v) const;
      typename Container::Iterator<MeshIndex>::P getFacesUsingVertex(const MeshIndex v) const;

      bool isVertexUsed(const MeshIndex v) const;

      bool isVertexOnBoundary(const MeshIndex v) const;

      bool isFaceOnBoundary(const MeshIndex f) const;

      bool areFacesAdjacent(const MeshIndex a, const MeshIndex b) const;

      MeshIndex getAdjacentFaces(const MeshIndex v, MeshIndex* vs) const;

      MeshIndex getBoundaryNeighbours(const MeshIndex v, MeshIndex* ns) const;

      MeshIndex getFacesUsingEdge(const MeshIndex va, const MeshIndex vb, MeshIndex* fs) const;

      MeshIndex getFaceIndex(const MeshIndex va, const MeshIndex vb, const MeshIndex vc) const;

   protected:
      struct D;
      D* m_D;

   };
}

#endif // __MESHCONNECTIVITY_H__