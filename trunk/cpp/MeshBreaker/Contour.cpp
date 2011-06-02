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
#include "Contour.h"

#include "Useful/Exception.h"
#include "Mesh/TuplesImpl.h"

#include "Edge.h"
#include "EdgeBuilder.h"

#include <set>
#include <limits>

using namespace AWT;

typedef std::pair<MeshIndex,MeshIndex> Edge;

#include <fstream>
#include <set>

struct AWT::Contour::D
{
   D( ) : m_Edge( 0 )
   {
   }

   unsigned int getLength( ) const
   {
      unsigned int ret = 0;

      Edge* contStart = m_Edge;
      Edge* contEdge  = contStart;

      do
      {
         ++ret;

         // Move to the next edge
         contEdge = contEdge->getNext( );
      }
      while ( contEdge != contStart );

      return ret;
   }

   AWT::Tuples<MeshIndex>::P getContour( )
   {
      AWT::Tuples<MeshIndex>::P ret = AWT::TuplesImpl<MeshIndex>::getInstance( 1, getLength( ) );

      Edge* contStart = m_Edge;
      Edge* contEdge  = contStart;

      do
      {
         MeshIndex v = contEdge->getVertex( );
         ret->addPoint( &v );

         // Move to the next edge
         contEdge = contEdge->getNext( );
      }
      while ( contEdge != contStart );

      return ret;
   }

   static void printVertices( std::ostream& os, Contour* c )
   {
      Tuples<MeshIndex>::P cont = c->getContour();

      for ( MeshIndex v = 0; v < cont->getNumberOfPoints( ); ++v )
      {
         os << cont->getPointElement( v, 0 ) << ";";
      }
      os << std::endl;
   }

   static void printEdge( std::ostream& os, Edge* edge, const char* label = 0 )
   {
      if ( label != 0 )
         os <<  "\"" << edge << "\" [label=" << label << "]" << std::endl;

      //os << "\"v" << edge->getPrevious( )->getVertex( ) << "\" -> \"v" << edge->getVertex( ) << "\" [label=\"" << edge << "\"]" << std::endl;

      // Style the edge node
      os << "\"" << edge << "\" [style=filled, label=\"\", shape=point]" << std::endl;

      // From previous vertex to edge
      os << "\"v" << edge->getPrevious( )->getVertex( ) << "\" -> \"" << edge << "\" [style=dashed]" << std::endl;

      // From edge onward to vertex
      os << "\"" << edge << "\" -> \"v" << edge->getVertex( ) << "\"" << std::endl;

      // From edge to next
      os << "\"" << edge << "\" -> \"" << edge->getNext( ) << "\" [color=blue]" << std::endl;

      // From edge to next
      os << "\"" << edge << "\" -> \"" << edge->getPrevious( ) << "\" [color=red]" << std::endl;
   }

   static void printContours( std::ostream& os, std::vector<Contour::P>& m_Contours )
   {
      std::vector<Contour::P>::iterator iter = m_Contours.begin();
      std::vector<Contour::P>::iterator iterEnd = m_Contours.end();

      os << "digraph foo {" << std::endl;
      os << "overlap=scale" << std::endl;

      for ( ; iter != iterEnd; ++iter )
      {
         std::cerr << (*iter)->getLength( ) << std::endl;

         Edge* edgeStart = (*iter)->getEdge();
         Edge* edge = edgeStart;

         do
         {
            printEdge( os, edge );

            edge = edge->getNext();
         }
         while ( edge != edgeStart );
      }

      os << "}" << std::endl;
   }

   void zip( Edge*& p0, Edge*& np )
   {
      Edge* np0 = p0->getPrevious( );
      Edge* nnp = np->getNext( );

      np0->setNext( nnp );
      nnp->setPrevious( np0 );

      // Make sure that we don't lose hold of the edge
      if ( m_Edge == p0 )
         m_Edge = np0;
      else if ( m_Edge = np )
         m_Edge = nnp;

      p0->destroy( );
      np->destroy( );

      p0 = np0;
      np = nnp;
   }

   static bool getCommonEdges( AWT::Contour::P c1, AWT::Contour::P c2, AWT::Edge*& e1, AWT::Edge*& e2, bool debug = false )
   {
      // We need to find a common edge between the contours
      // We will do the initial merge about this edge
      // Let's assume that contour is shorter than this

      Edge* contStart = c1->m_D->m_Edge;
      Edge* contEdge  = contStart;

      Edge* thisEdge  = 0;

      bool found = false;

      do
      {
         // Get the start and end vertices of contEdge
         MeshIndex contEdgeSt = contEdge->getPrevious( )->getVertex( );
         MeshIndex contEdgeEn = contEdge->getVertex( );

         if ( debug )
         {
            DEBUGMACRO( contEdgeSt << "  " << contEdgeEn );
         }

         // Now, go around this contour looking for an edge which is the same but reversed
         Edge* thisStart = c2->m_D->m_Edge;
         thisEdge  = thisStart;

         do
         {
            MeshIndex thisEdgeSt = thisEdge->getPrevious( )->getVertex( );
            MeshIndex thisEdgeEn = thisEdge->getVertex( );

            if ( debug )
               DEBUGMACRO( "  " << thisEdgeEn << "  " << thisEdgeSt );

            // Do the comparison flipped, since the triangles will be
            // oppositely ordered if compatible
            if ( thisEdgeSt == contEdgeEn && thisEdgeEn == contEdgeSt )
            {
               // Bingo!  We've found a join edge
               e1 = contEdge;
               e2 = thisEdge;

               return true;
            }

            // Move to the next edge
            thisEdge = thisEdge->getNext( );
         }
         while ( thisStart != thisEdge );

         if ( found )
            break;

         // Just to make sure that we don't "find" it
         // Shouldn't get to here if we have found a common edge
         thisEdge = 0;

         // Move to the next edge
         contEdge = contEdge->getNext( );
      }
      while ( contEdge != contStart );

      return false;
   }

   Edge* m_Edge;

   EdgeBuilder::P m_EdgeBuilder;
};

AWT::Contour::Contour( EdgeBuilder::P eb )
{
   m_D = new D;

   m_D->m_EdgeBuilder = eb;
}

AWT::Contour::~Contour( )
{
   delete m_D;
}

AWT::Contour::P AWT::Contour::getInstance( AWT::EdgeBuilder::P eb )
{
   AUTOGETINSTANCE( AWT::Contour, ( eb ) );
}

AWT::Contour::P AWT::Contour::getInstance( EdgeBuilder::P eb, const MeshIndex* vs, const unsigned int n )
{
   P ret = getInstance( eb );

   D* d = ret->m_D;

   // Create the starting edge
   d->m_Edge = eb->createEdge( vs[0] );
   Edge* edges[2];

   edges[0] = d->m_Edge;

   for ( unsigned int i = 0; i < n-1; ++i )
   {
      edges[1] = eb->createEdge( vs[i+1] );

      edges[0]->setNext( edges[1] );
      edges[1]->setPrevious( edges[0] );

      edges[0] = edges[1];
   }

   // Close the loop
   edges[0]->setNext( d->m_Edge );
   d->m_Edge->setPrevious( edges[0] );

   return ret;
}

GETNAMEMACRO( AWT::Contour );

void AWT::Contour::merge( AWT::Contour::P contour )
{
   Edge* contEdge = 0;
   Edge* thisEdge = 0;

   bool found = D::getCommonEdges( contour, Contour::P( this ), contEdge, thisEdge );

   if ( !found )
      AWTEXCEPTIONTHROW( "Could not find common edge to merge on" );

   // At this point, we've got an edge in each of the two contours which
   // we can join together.

   // Perhaps not the most elegant way, but it works (cue for a bug)
   Edge* pp = contEdge->getPrevious( );
   Edge* ep = contEdge;
   Edge* np = contEdge->getNext( );

   Edge* p0 = thisEdge->getPrevious( );
   Edge* e0 = thisEdge;
   Edge* n0 = thisEdge->getNext( );

   pp->setNext( n0 );
   n0->setPrevious( pp );
   p0->setNext( np );
   np->setPrevious( p0 );

   // Delete common edges, but first make sure that we won't lose pointer
   // Shift along the contour by 1 if nec
   if ( this->m_D->m_Edge == e0 )
      this->m_D->m_Edge = this->m_D->m_Edge->getNext( );

   // Don't delete directly, but let internals handle it
   e0->destroy( );
   ep->destroy( );

   // Now we need to zip up common edges
   if ( np->getVertex( ) == p0->getPrevious( )->getVertex( ) )
      m_D->zip( p0, np );

   if ( n0->getVertex( ) == pp->getPrevious( )->getVertex( ) )
      m_D->zip( pp, n0 );
}

bool AWT::Contour::getEdgeToVertex( AWT::MeshIndex v, AWT::Edge*& e )
{
   Edge* contStart = m_D->m_Edge;
   Edge* contEdge  = contStart;

   do
   {
      if ( contEdge->getVertex() == v )
      {
         e = contEdge;
         return true;
      }

      // Move to the next edge
      contEdge = contEdge->getNext( );
   }
   while ( contEdge != contStart );

   return false;
}

AWT::Tuples<MeshIndex>::P AWT::Contour::getContour( )
{
   return m_D->getContour( );
}

AWT::Edge* AWT::Contour::getEdge( )
{
   return m_D->m_Edge;
}

unsigned int AWT::Contour::getLength( ) const
{
   return m_D->getLength( );
}

void AWT::Contour::insert( std::vector<AWT::Contour::P>& contours, EdgeBuilder::P eb, MeshIndex* vs, bool isSplit )
{
   // Firstly, let's make a contour of the vertices provided
   Contour::P newCont = Contour::getInstance( eb, vs, 3 );

   // Now, we want to work out which of the contours shares a common edge with this new contour
   // We will do this in reverse, because later we will bubble the active contour up to the back
   // of contours
   std::vector<P>::iterator iterEnd = contours.end();
   std::vector<P>::iterator iterBeg = contours.begin();

   if ( iterEnd != iterBeg )
   {
      Edge* newEdge = 0;
      Edge* mrgEdge = 0;

      do
      {
         // Move the end iterator one place back
         --iterEnd;

         // Look to see if there are common edges
         if ( D::getCommonEdges( newCont, *iterEnd, newEdge, mrgEdge ) )
         {
            DEBUGMACRO( "Common edge found!" );

            // We've found a common edge, so we know that we can join
            // these two contours together.  
            Contour::P mergeCont( *iterEnd );

            // We should get better average performance) if we bubble the current 
            // contour to the end of the vector, since we will likely access this
            // lots of times
            contours.erase( iterEnd );
            contours.push_back( mergeCont );

            // It's not //quite// that simple
            // as this if isSplit == true, since then we need to do some
            // surgery on the existing contours

            if ( isSplit )
            {
               DEBUGLINE;

               // If a split occurs, we know that we have a situation where
               // there is one common edge and one distal vertex.

               // Firstly, we need to identify this vertex
               MeshIndex distal;
               for ( distal = 0; distal < 3; ++distal )
               {
                  if ( vs[distal] != newEdge->getVertex() && vs[distal] != newEdge->getPrevious()->getVertex() )
                     break;
               }

               if ( distal == 3 )
                  AWTEXCEPTIONTHROW( "Could not find distal vertex" );

               // Right: now we need to work out which of the existing contours contains this
               // distal vertex already.  If mergeCont contains it, then we need to split it
               // in two.  If another contour contains it, then we need to merge the contours
               // together.

               Edge* a;

               // Reset the iterator
               iterEnd = contours.end();

               do
               {
                  // Move the iterator backward
                  --iterEnd;

                  // Can we find the merge vertex?
                  if ( (*iterEnd)->getEdgeToVertex( vs[distal], a ) )
                  {
                     // These are just labels to make things easier to read
                     Edge* b = a->getNext();

                     Edge* d = mrgEdge;
                     Edge* c = mrgEdge->getPrevious();
                     Edge* e = mrgEdge->getNext();

                     // Create new edges, pointing at the target vertices of a and d
                     Edge* f = eb->createEdge( a->getVertex() );
                     Edge* g = eb->createEdge( d->getVertex() );

                     // Repoint so that g follows a
                     a->setNext( g );
                     g->setPrevious( a );

                     // Repoint so that e follows g
                     g->setNext( e );
                     e->setPrevious( g );

                     // Repoint so that f follows c
                     c->setNext( f );
                     f->setPrevious( c );

                     // Report so that b follows f
                     f->setNext( b );
                     b->setPrevious( f );

                     // Before we destroy d, make sure that we won't lose the contour's first edge
                     if ( d == (*iterEnd)->m_D->m_Edge )
                        (*iterEnd)->m_D->m_Edge = c;

                     // Now it is safe to destroy d
                     d->destroy( );

                     // Now, the contours are merged.  The question is whether we
                     // are left with one contour or two.  This is dependent upon
                     // whether (*iterEnd) = mergeCont or not

                     if ( *iterEnd == mergeCont )
                     {
                        //DEBUGLINE;
                        // We have now created two contours.  Create two new contours
                        // to put into the vector

                        // Get rid of the old contour
                        contours.pop_back( );

                        // ... and put two new ones in
                        Contour::P ca = getInstance( eb );
                        ca->m_D->m_Edge = a;

                        contours.push_back( ca );
                        //D::printVertices( std::cerr, *ca );

                        Contour::P cb = getInstance( eb );
                        cb->m_D->m_Edge = b;

                        contours.push_back( cb );
                        //D::printVertices( std::cerr, *cb );
                     }
                     else
                     {
                        //DEBUGLINE;
                        // We have now merged two contours into one.  We need to get rid of the
                        // old contour from the vector
                        contours.erase( iterEnd );
                     }

                     break;
                  }

               } 
               while ( iterEnd != iterBeg );

               /*
               if ( iterEnd == iterBeg )
               {
                  AWTEXCEPTIONTHROW( "Split contour not found!" );
               }
               */

               DEBUGLINE;
               mergeCont->merge( newCont );
               DEBUGLINE;
            }
            else
            {
               // This is nice and simple, we just need to merge them together.
               mergeCont->merge( newCont );
            }

            // If the resulting merged contour is not even a triangle, get rid of it
            if ( mergeCont->getLength() < 3 )
               contours.pop_back();

            // No need to keep going, we've merged
            //PRINTVBL( contours.size() );
            return;
         }
         else
         {
            // If not, keep on moving backwards
         }
      }
      while ( iterEnd != iterBeg );

      std::vector<Contour::P>::iterator it = contours.begin( );

      D::printVertices( std::cerr, *newCont );

      while ( it != contours.end( ) )
      {
         D::printVertices( std::cerr, (*it++).getData( ) );
      }

      AWTEXCEPTIONTHROW( "Could not merge triangle into existing contours" );
   }

   /*
   DEBUGMACRO( "There are no contours: adding the new contour" );
   PRINTVBL( contours.size( ) );
   */

   // The only reason why we are here is that there was no common edge found.
   // This means that we can simply add the new contour to the contours set
   contours.push_back( newCont );
}

void AWT::Contour::printVertices( std::ostream& os, std::vector<Contour::P>& contours )
{
   std::vector<Contour::P>::iterator it = contours.begin( );
   while ( it != contours.end( ) )
   {
      D::printVertices( os, (*it++).getData( ) );
   }
}

void AWT::Contour::printContours( std::ostream& os, std::vector<Contour::P>& contours )
{
   D::printContours( os, contours );
}

#include <sstream>

void AWT::Contour::renderGraph( const char* inputFilename, const char* outputFilename )
{
   std::stringstream sstr;
   sstr << "neato -o" << outputFilename << " -Tsvg -Lg " << inputFilename;

   std::cerr << sstr.str( ) << std::endl;

   system( sstr.str( ).c_str( ) );
}