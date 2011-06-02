#include "HalfEdgeVertex.h"

struct AWT::HalfEdgeVertex::D
{
   MeshIndex m_V;
   HalfEdge* m_Edge;
};

AWT::HalfEdgeVertex::HalfEdgeVertex( const MeshIndex v )
{
   m_D = new D;

   m_D->m_V = v;
   m_D->m_Edge = 0;
}

AWT::HalfEdgeVertex::~HalfEdgeVertex( )
{
   delete m_D;
}

AWT::MeshIndex AWT::HalfEdgeVertex::getIndex( ) const
{
   return m_D->m_V;
}

AWT::HalfEdge* AWT::HalfEdgeVertex::getEdge( ) const
{
   return m_D->m_Edge;
}

void AWT::HalfEdgeVertex::setEdge( AWT::HalfEdge* edge )
{
   m_D->m_Edge = edge;
}