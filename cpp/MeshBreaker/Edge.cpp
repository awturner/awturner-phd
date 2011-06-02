#include "Edge.h"

struct AWT::Edge::D
{
   D( const MeshIndex v ) : m_Vertex( v ), m_Next( 0 ), m_Previous( 0 )
   {
   }

   EdgeBuilder* m_EdgeBuilder;
   const MeshIndex m_Vertex;
   Edge* m_Next;
   Edge* m_Previous;
};

AWT::Edge::Edge( EdgeBuilder::P eb, const MeshIndex vertex )
{
   m_D = new D( vertex );

   m_D->m_EdgeBuilder = *eb;
}

AWT::Edge::~Edge( )
{
   delete m_D;
}

void AWT::Edge::destroy( )
{
   m_D->m_EdgeBuilder->destroy( this );
}

AWT::Edge::P AWT::Edge::getInstance( AWT::EdgeBuilder::P eb, const AWT::MeshIndex v )
{
   AUTOGETINSTANCE( AWT::Edge, ( eb, v ) );
}

GETNAMEMACRO( AWT::Edge );

const AWT::MeshIndex AWT::Edge::getVertex( ) const
{
   return m_D->m_Vertex;
}

AWT::Edge* AWT::Edge::getPrevious( )
{
   return m_D->m_Previous;
}

void AWT::Edge::setPrevious( Edge* e )
{
   if ( m_D->m_Previous != e )
   {
      m_D->m_Previous = e;
      modified( );
   }
}

AWT::Edge* AWT::Edge::getNext( )
{
   return m_D->m_Next;
}

void AWT::Edge::setNext( Edge* e )
{
   if ( m_D->m_Next != e )
   {
      m_D->m_Next = e;
      modified( );
   }
}
