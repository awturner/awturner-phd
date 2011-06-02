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
