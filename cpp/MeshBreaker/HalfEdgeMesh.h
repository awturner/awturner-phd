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
      HalfEdgeMesh( );
      virtual ~HalfEdgeMesh( );

   public:
      static P getInstance( );
      virtual std::string getClassName( ) const;

      virtual bool canAddTriangle( const MeshIndex* vs );
      virtual void addTriangle( const MeshIndex* vs, const MeshIndex faceIndex, bool& isMerge, MeshIndex& mergePoint );

      virtual MeshIndex getNumberOfContours( );

      virtual void print( std::ostream& os );

      virtual void ensureVertices( const MeshIndex v );

      virtual void getCounts( MeshIndex* counts ) const;

   protected:
      virtual std::vector< HalfEdgeVertex* >& getVertices( );
      virtual std::vector< HalfEdge* >&       getEdges( );

      struct D;
      D* m_D;
   };
}

#endif // __HALFEDGEMESH_H__