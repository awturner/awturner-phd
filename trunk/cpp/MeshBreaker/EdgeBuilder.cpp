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