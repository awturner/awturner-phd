#ifndef __FLATTENMESHPAIR_H__
#define __FLATTENMESHPAIR_H__

#include "ObjectManagement/ManagedObject.h"

#include "Mesh/Mesh.h"

namespace AWT
{
   template <class T>
   class FlattenMeshPair : public ManagedObject
   {
   public:
      typedef ManagedAutoPointer<FlattenMeshPair<T>> P;

   protected:
      FlattenMeshPair( typename Mesh<T>::P mesh, typename Mesh<T>::P flat );
      virtual ~FlattenMeshPair( );

   public:
      static P getInstance( typename Mesh<T>::P mesh, typename Mesh<T>::P flat );
      virtual std::string getClassName( ) const;

      virtual typename AWT::Mesh<T>::P getMesh( );
      virtual typename AWT::Mesh<T>::P getFlattenedMesh( );

      virtual MeshIndex mapMeshToFlattened( const T* meshCoords, T* flatCoords );
      virtual MeshIndex mapFlattenedToMesh( const T* flatCoords, T* meshCoords );

      typename AWT::Mesh<T>::P toShapeImage( const MeshIndex nu, const MeshIndex nv );

   protected:
      struct D;
      D* m_D;
   };
}

#endif // __FLATTENMESHPAIR_H__