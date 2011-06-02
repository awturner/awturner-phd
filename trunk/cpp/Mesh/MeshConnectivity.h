#ifndef __MESHCONNECTIVITY_H__
#define __MESHCONNECTIVITY_H__

#include "ObjectManagement/ManagedObject.h"

#include "MeshTypeDefs.h"

#include "Useful/Iterator.h"

#include "Mesh.h"

namespace AWT
{
   template <class T>
   class MeshConnectivity : public ManagedObject
   {
   protected:
      MeshConnectivity( typename Mesh<T>::P mesh );
      virtual ~MeshConnectivity( );

   public:
      typedef ManagedAutoPointer< MeshConnectivity<T> > P;

      static P getInstance( typename Mesh<T>::P mesh );
      virtual std::string getClassName( ) const;

      typename Mesh<T>::P getMesh( );

      typename Container::Iterator<MeshIndex>::P getAdjacentVertices( const MeshIndex v ) const;
      typename Container::Iterator<MeshIndex>::P getFacesUsingVertex( const MeshIndex v ) const;

      bool isVertexUsed( const MeshIndex v ) const;

      bool isVertexOnBoundary( const MeshIndex v ) const;

      bool isFaceOnBoundary( const MeshIndex f ) const;

      bool areFacesAdjacent( const MeshIndex a, const MeshIndex b ) const;

      MeshIndex getAdjacentFaces( const MeshIndex v, MeshIndex* vs ) const;

      MeshIndex getBoundaryNeighbours( const MeshIndex v, MeshIndex* ns ) const;

      MeshIndex getFacesUsingEdge( const MeshIndex va, const MeshIndex vb, MeshIndex* fs ) const;

      MeshIndex getFaceIndex( const MeshIndex va, const MeshIndex vb, const MeshIndex vc ) const;

   protected:
      struct D;
      D* m_D;

   };
}

#endif // __MESHCONNECTIVITY_H__