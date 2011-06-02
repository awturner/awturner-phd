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