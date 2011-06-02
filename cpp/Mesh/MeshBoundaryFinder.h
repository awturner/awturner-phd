#ifndef __MESHBOUNDARYFINDER_H__
#define __MESHBOUNDARYFINDER_H__

#include "ObjectManagement/ManagedObject.h"

#include "Mesh/Tuples.h"
#include "Mesh/Mesh.h"

namespace AWT
{
   template <class T>
   class MeshBoundaryFinder : public ManagedObject
   {
   public:
      static typename Tuples<T>::P findBoundaryVertices( typename Mesh<T>::P mesh );

   protected:
      MeshBoundaryFinder( ) {}
      virtual ~MeshBoundaryFinder( ) {}
   };
}

#endif // __MESHBOUNDARYFINDER_H__