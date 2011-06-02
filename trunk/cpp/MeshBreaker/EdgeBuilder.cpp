#include "EdgeBuilder.h"

#include "Edge.h"

#include <vector>

struct AWT::EdgeBuilder::D
{
   std::vector<Edge::P> m_Store;
};

AWT::EdgeBuilder::EdgeBuilder( )
{
   m_D = new D;
}

AWT::EdgeBuilder::~EdgeBuilder( )
{
   delete m_D;
}

AWT::EdgeBuilder::P AWT::EdgeBuilder::getInstance( )
{
   AUTOGETINSTANCE( AWT::EdgeBuilder, ( ) );
}

GETNAMEMACRO( AWT::EdgeBuilder );

AWT::Edge* AWT::EdgeBuilder::createEdge( const MeshIndex v )
{
   Edge::P retP = Edge::getInstance( this, v );

   // Keep the managed autopoitner to this so that we will clean up later
   m_D->m_Store.push_back( retP );

   // and just return the pointer to the edge
   return *retP;
}

void AWT::EdgeBuilder::destroy(AWT::Edge *edge)
{
   std::vector<Edge::P>::iterator iter    = m_D->m_Store.begin();
   std::vector<Edge::P>::iterator iterEnd = m_D->m_Store.end();

   while ( iter != iterEnd )
   {
      if ( iter->getDataConst( ) == edge )
      {
         m_D->m_Store.erase( iter, iter );
         break;
      }

      ++iter;
   }

   //delete edge;
}