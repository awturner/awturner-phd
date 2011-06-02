#ifndef __HALFEDGE_H__
#define __HALFEDGE_H__

#include "Mesh/MeshTypeDefs.h"
#include <ostream>

namespace AWT
{
   class HalfEdgeVertex;

   class HalfEdge
   {
      friend class HalfEdgeMesh;

   protected:
      HalfEdge( );
      virtual ~HalfEdge( );

   public:
      HalfEdge* getPair( ) const;
      void setPair( HalfEdge* p );

      HalfEdge* getPrevious( );

      HalfEdge* getNext( ) const;
      void setNext( HalfEdge* e );

      HalfEdgeVertex* getVertex( ) const;
      void setVertex( HalfEdgeVertex* v );

      bool hasFace( ) const;

      MeshIndex getFace( ) const;
      void setFace( const MeshIndex f );

      bool isCrossable( ) const;
      void setCrossable( const bool c );

      MeshIndex getLabel( ) const;
      void setLabel( const MeshIndex v );

   protected:
      struct D;
      D* m_D;

   };

   extern bool halfEdgeVerbose;
   extern bool halfEdgePrintComment;
   std::ostream& operator<<( std::ostream& os, const HalfEdge& e );
}

#endif // __HALFEDGE_H__