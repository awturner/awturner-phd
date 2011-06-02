#ifndef __HALFEDGEVERTEX_H__
#define __HALFEDGEVERTEX_H__

#include "HalfEdge.h"

namespace AWT
{
   class HalfEdgeVertex
   {
      friend class HalfEdgeMesh;
   protected:
      HalfEdgeVertex( const MeshIndex v );
      virtual ~HalfEdgeVertex( );

   public:
      virtual MeshIndex getIndex( ) const;

      virtual HalfEdge* getEdge( ) const;

      virtual void setEdge( HalfEdge* edge );

   protected:
      struct D;
      D* m_D;

   };
}

#endif // __HALFEDGEVERTEX_H__