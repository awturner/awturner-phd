#ifndef __SHAPEIMAGE_H__
#define __SHAPEIMAGE_H__

#include "ObjectManagement/ManagedObject.h"

#include "FlattenMeshPair.h"

namespace AWT
{
   template <class T>
   class ShapeImage : public ManagedObject
   {
   public:
      typedef ManagedAutoPointer<ShapeImage> P;

   protected:
      ShapeImage( typename FlattenMeshPair<T>::P flat, unsigned int ngrid );
      virtual ~ShapeImage( );

   public:
      static P getInstance( typename FlattenMeshPair<T>::P flat, unsigned int ngrid );
      virtual std::string getClassName( ) const;

      virtual MeshIndex mapMeshToFlattened( const T* meshCoords, T* flatCoords );
      virtual MeshIndex mapFlattenedToMesh( const T* flatCoords, T* meshCoords );

   protected:
      struct D;
      D* m_D;
   };
}

#endif // __SHAPEIMAGE_H__