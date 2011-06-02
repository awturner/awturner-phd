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
#include "HalfEdge.h"

#include "HalfEdgeVertex.h"
#include <limits>

bool AWT::halfEdgeVerbose = false;
bool AWT::halfEdgePrintComment = false;

struct AWT::HalfEdge::D
{
   HalfEdge* m_Pair;
   HalfEdge* m_Next;

   MeshIndex m_Label;

   HalfEdgeVertex* m_Vertex;

   bool m_Crossable;

   MeshIndex m_Face;
};

AWT::HalfEdge::HalfEdge( )
{
   m_D = new D;

   m_D->m_Pair = 0;
   m_D->m_Next = 0;

   m_D->m_Vertex = 0;

   m_D->m_Face = std::numeric_limits<MeshIndex>::max( );

   m_D->m_Crossable = true;

   m_D->m_Label = std::numeric_limits<MeshIndex>::max( );
}

AWT::HalfEdge::~HalfEdge( )
{
   delete m_D;
}

AWT::HalfEdge* AWT::HalfEdge::getPair( ) const
{
   return m_D->m_Pair;
}

void AWT::HalfEdge::setPair( AWT::HalfEdge* p )
{
   m_D->m_Pair = p;
}

#include "Useful/Exception.h"
#include "Useful/PrintMacros.h"

AWT::HalfEdge* AWT::HalfEdge::getPrevious( )
{
   // e is an outgoing edge from a vertex; get the incoming pair
   HalfEdge* start = getPair( );
   HalfEdge* e     = start;

   //DEBUGMACRO( "Trying to find previous of " << *this );

   do
   {
      //DEBUGMACRO( "e = " << *e );
      if ( e->getNext( ) == this )
         return e;

      // Move on to the next incoming edge - getNext( ) is outgoing
      e = e->getNext( )->getPair( );
   }
   while ( e != start );

   AWTEXCEPTIONTHROW( "Could not find previous edge!" );
}

AWT::HalfEdge* AWT::HalfEdge::getNext( ) const
{
   return m_D->m_Next;
}

void AWT::HalfEdge::setNext( AWT::HalfEdge* e )
{
   m_D->m_Next = e;
}

AWT::HalfEdgeVertex* AWT::HalfEdge::getVertex( ) const
{
   return m_D->m_Vertex;
}

void AWT::HalfEdge::setVertex( HalfEdgeVertex* v )
{
   m_D->m_Vertex = v;
}

bool AWT::HalfEdge::hasFace( ) const
{
   return m_D->m_Face != std::numeric_limits<MeshIndex>::max( );
}

AWT::MeshIndex AWT::HalfEdge::getFace( ) const
{
   return m_D->m_Face;
}

void AWT::HalfEdge::setFace( const AWT::MeshIndex f )
{
   m_D->m_Face = f;
}

bool AWT::HalfEdge::isCrossable( ) const
{
   return m_D->m_Crossable;
}

void AWT::HalfEdge::setCrossable( const bool c )
{
   m_D->m_Crossable = c;
   getPair( )->m_D->m_Crossable = c;
}

AWT::MeshIndex AWT::HalfEdge::getLabel( ) const
{
   return m_D->m_Label;
}

void AWT::HalfEdge::setLabel( const AWT::MeshIndex v )
{
   m_D->m_Label = v;
}

std::ostream& AWT::operator<<( std::ostream& os, const AWT::HalfEdge& e )
{
   
   os << "  \"" << e.getPair( )->getVertex( )->getIndex( ) << "\"";
   os << " -> ";
   os << "\"" << e.getVertex( )->getIndex( ) << "\"";

   if ( halfEdgeVerbose )
   {
      os << " [arrowhead=halfopen,"; //label=\"" << &e << "\"";

      MeshIndex label = e.getLabel( );
      
      if ( !e.isCrossable( ) )
      {
         os << "penwidth=4,";
      }

      if ( label == std::numeric_limits<MeshIndex>::max( ) )
      {
         os << "style=dashed,color=";

         if ( e.isCrossable( ) )
            os << "gray";
         else
            os << "gray";
      }
      else
      {
         os << "color=";
         switch ( label % 7 )
         {
         case 0: os << "red"; break;
         case 1: os << "blue"; break;
         case 2: os << "green"; break;
         case 3: os << "\"#7f7f00\""; break;
         case 4: os << "magenta"; break;
         case 5: os << "cyan"; break;
         case 6: os << "black"; break;
         }
      }

      os << "]";
   }
   
   if ( halfEdgePrintComment )
      os << " /* this=" << &e << ", pair=" << e.getPair( ) << ", next=" << e.getNext( ) << ", label=" << e.getLabel( ) << ", hasFace=" << (e.hasFace() ? "true" : "false") << " */";

   return os;
}