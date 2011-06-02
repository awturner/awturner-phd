#ifndef __MESHBREAKER_H__
#define __MESHBREAKER_H__

#include "ObjectManagement/ManagedObject.h"

#include "Mesh/Mesh.h"
#include "Mesh/MeshConnectivity.h"

namespace AWT
{
   template <class T>
   class MeshBreaker : public ManagedObject
   {
   public:
      enum Symbol { SYM_UNDEF, SYM_C, SYM_L, SYM_R, SYM_E, SYM_S };

      typedef ManagedAutoPointer<MeshBreaker<T>> P;

   protected:
      MeshBreaker( typename Mesh<T>::P mesh );
      virtual ~MeshBreaker( );

   public:
      static P getInstance( typename Mesh<T>::P mesh );
      virtual std::string getClassName( ) const;

      virtual unsigned int getNumberOfBranches( ) const;
      virtual typename Tuples<T>::P getBranch( unsigned int i );

      virtual typename Mesh<T>::P getMesh( );
      virtual typename MeshConnectivity<T>::P getConnectivity( );

      virtual typename Mesh<T>::P getRegion( const unsigned int i );

      virtual unsigned int getNumberOfContours( ) const;
      virtual typename Tuples<T>::P getContour( const unsigned int i );

      virtual MeshIndex getHistoryLength( ) const;
      virtual void printHistoryItem( MeshIndex h ) const;

      virtual Symbol    getHistorySymbol( AWT::MeshIndex h ) const;
      virtual MeshIndex getHistoryFace( MeshIndex h ) const;
      virtual MeshIndex getHistoryLeftNeighbour( MeshIndex h ) const;
      virtual MeshIndex getHistoryRightNeighbour( MeshIndex h ) const;

      virtual MeshIndex getNumberOfDebugFaces( ) const;
      virtual MeshIndex getDebugFace( const MeshIndex i ) const;

   protected:
      struct D;
      D* m_D;

   };
}

#endif // __MESHBREAKER_H__