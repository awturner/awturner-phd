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
#ifndef __HALFEDGEMESH_H__
#define __HALFEDGEMESH_H__

#include "ObjectManagement/ManagedObject.h"

#include "Mesh/MeshTypeDefs.h"
#include <vector>

namespace AWT
{
   template <class T>
   class FindGenerators;

   template <class T>
   class FlattenMesh;

   class HalfEdgeVertex;
   class HalfEdge;

   class HalfEdgeMesh : public ManagedObject
   {
      template <class T>
      friend class FindGenerators;

      template <class T>
      friend class FlattenMesh;

   public:
      typedef ManagedAutoPointer<HalfEdgeMesh> P;

   protected:
      HalfEdgeMesh();
      virtual ~HalfEdgeMesh();

   public:
      static P getInstance();
      virtual std::string getClassName() const;

      virtual bool canAddTriangle(const MeshIndex* vs);
      virtual void addTriangle(const MeshIndex* vs, const MeshIndex faceIndex, bool& isMerge, MeshIndex& mergePoint);

      virtual MeshIndex getNumberOfContours();

      virtual void print(std::ostream& os);

      virtual void ensureVertices(const MeshIndex v);

      virtual void getCounts(MeshIndex* counts) const;

   protected:
      virtual std::vector< HalfEdgeVertex* >& getVertices();
      virtual std::vector< HalfEdge* >&       getEdges();

      struct D;
      D* m_D;
   };
}

#endif // __HALFEDGEMESH_H__