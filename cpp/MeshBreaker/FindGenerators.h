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
#ifndef __FINDGENERATORS_H__
#define __FINDGENERATORS_H__

#include "GeneratorProvider.h"
#include "Mesh/Mesh.h"
#include "HalfEdgeMesh.h"

namespace AWT
{
   template <class T>
   class FindGenerators : public GeneratorProvider<T>
   {
   public:
      typedef ManagedAutoPointer<FindGenerators> P;

      enum TraversalOrder
      {
         TRAVERSE_RANDOM,
         TRAVERSE_DEPTH,
         TRAVERSE_BREADTH
      };

   protected:
      FindGenerators( );
      FindGenerators( typename Mesh<T>::P mesh, const TraversalOrder ord );
      virtual ~FindGenerators( );

   public:
      static P getInstance( typename Mesh<T>::P mesh, const TraversalOrder ord );
      virtual std::string getClassName( ) const;

      MeshIndex getNumberOfGeneratorPairs( ) const;
      void getGeneratorPair( const MeshIndex p, Tuples<MeshIndex>::P& contA, Tuples<MeshIndex>::P& contB );
      void getGeneratorPair( const MeshIndex p, std::vector< HalfEdge* >& contA, std::vector< HalfEdge* >& contB );

      const std::vector<HalfEdge*>& getGenerator( const unsigned int pair, const unsigned int i );

      static TraversalOrder getTraversalOrder( const char* wstr );

      TraversalOrder getTraversalOrder( ) const;
      void setTraversalOrder( const TraversalOrder ord );

      void flipGenerator( const MeshIndex p, const MeshIndex i );

      typename Mesh<T>::P getMesh( );

      bool isFaceVisited( const MeshIndex f ) const;

      MeshIndex getLastMergePoint( ) const;

      HalfEdgeMesh::P getHalfEdgeMesh( );

   protected:
      struct D;
      D* m_D;

   };
}

#endif // __FINDGENERATORS_H__