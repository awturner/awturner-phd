#ifndef __SIMPLEMESHCONNECTIVITY_H__
#define __SIMPLEMESHCONNECTIVITY_H__

#include "ObjectManagement/ManagedObject.h"

#include "Useful/Iterator.h"

#include "Mesh.h"

namespace AWT
{
   namespace SimpleMesh
   {
      class Connectivity : public ManagedObject
      {
      protected:
         Connectivity( Mesh::P mesh );
         virtual ~Connectivity( );

      public:
         typedef ManagedAutoPointer<Connectivity> P;

         typedef Container::Iterator<Index> IndexIterator;

         static P getInstance( Mesh::P mesh );
         virtual std::string getClassName( ) const;

         Mesh::P getMesh( );

         IndexIterator::P getAdjacentVertices( const Index v ) const;
         IndexIterator::P getFacesUsingVertex( const Index v ) const;

         bool isVertexUsed( const Index v ) const;

         bool isVertexOnBoundary( const Index v ) const;

         bool isFaceOnBoundary( const Index f ) const;

         bool areFacesAdjacent( const Index a, const Index b ) const;

         Index getAdjacentFaces( const Index v, Index* vs ) const;

         Index getBoundaryNeighbours( const Index v, Index* ns ) const;

         Index getFacesUsingEdge( const Index va, const Index vb, Index* fs ) const;

         Index getFaceIndex( const Index va, const Index vb, const Index vc ) const;

         Index aFaceUsingVertex( const Index v ) const;

      protected:
         struct D;
         D* m_D;

      };
   }
}

#endif // __SIMPLEMESHCONNECTIVITY_H__