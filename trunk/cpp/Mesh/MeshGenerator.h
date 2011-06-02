#ifndef __MESHGENERATOR_H__
#define __MESHGENERATOR_H__

#include "ObjectManagement/ManagedObject.h"

#include "Mesh.h"

namespace AWT
{
   template <class T>
   class MeshGenerator : public ManagedObject
   {
   public:
      static void generateTorus( typename Mesh<T>::P mesh, T majorRadius, T minorRadius, MeshIndex resU, MeshIndex resV );
      static void generatePlane( typename Mesh<T>::P mesh, MeshIndex resU, MeshIndex resV );
   };
}

#endif // __MESHGENERATOR_H__