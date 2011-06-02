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
#include "MeshEdgeSearchFilter.h"

#include <queue>

#include "Connectivity.h"

struct AWT::SimpleMesh::MeshEdgeSearchFilter::D
{
   Index* faceWalkFromBoundary;
   bool* faceNotOnBoundary;
   Index nf;
};

AWT::SimpleMesh::MeshEdgeSearchFilter::MeshEdgeSearchFilter( Mesh::P mesh )
{
   m_D = new D;

   m_D->nf = mesh->nf;

   // Rather than constantly query the connectivity, just pre-cache result for all faces
   Connectivity::P conn = Connectivity::getInstance( mesh );

   m_D->faceWalkFromBoundary = new Index[ mesh->nf ];
   for ( Index f = 0; f < mesh->nf; ++f )
      m_D->faceWalkFromBoundary[f] = INVALID_INDEX;

   typedef std::pair<Index,Index> FacePair;
   typedef std::priority_queue<FacePair> Queue;

   Queue q;
   for ( Index f = 0; f < mesh->nf; ++f )
   {
      if ( conn->isFaceOnBoundary( f ) )
         q.push( FacePair( 2, f ) );
   }

   Index adjFaces[20];

   while ( !q.empty() )
   {
      FacePair fp = q.top();
      q.pop();

      if ( fp.first < m_D->faceWalkFromBoundary[fp.second] )
      {
         m_D->faceWalkFromBoundary[fp.second] = fp.first;

         if ( fp.first >= 1 )
         {
            const Index nadj = conn->getAdjacentFaces( fp.second, adjFaces );
            for ( Index i = 0; i < nadj; ++i )
               q.push( FacePair( fp.first-1, adjFaces[i] ) );
         }
      }
   }

   m_D->faceNotOnBoundary = new bool[ mesh->nf ];
   for ( Index f = 0; f < mesh->nf; ++f )
      m_D->faceNotOnBoundary[f] = !conn->isFaceOnBoundary( f );

}

AWT::SimpleMesh::MeshEdgeSearchFilter::~MeshEdgeSearchFilter( )
{
   delete [] m_D->faceNotOnBoundary;

   delete m_D;
}

AWT::SimpleMesh::MeshEdgeSearchFilter::P AWT::SimpleMesh::MeshEdgeSearchFilter::getInstance( AWT::SimpleMesh::Mesh::P mesh )
{
   AUTOGETINSTANCE( AWT::SimpleMesh::MeshEdgeSearchFilter, ( mesh ) );
}

GETNAMEMACRO( AWT::SimpleMesh::MeshEdgeSearchFilter );

bool AWT::SimpleMesh::MeshEdgeSearchFilter::check( const Point p, const Index i ) const
{
   return m_D->faceWalkFromBoundary[i] == INVALID_INDEX;
   //return m_D->faceNotOnBoundary[ i ];
}
