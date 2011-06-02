#ifndef __SAMPLEDMESHFACTORY_H__
#define __SAMPLEDMESHFACTORY_H__

#include "ObjectManagement/ManagedObject.h"

#include "Mesh/Mesh.h"
#include "SampledSurface.h"

namespace AWT
{
   namespace AlignParametric
   {
      class SampledSurfaceFactory : public ManagedObject
      {
      public:
         typedef ManagedAutoPointer<SampledSurfaceFactory> P;

         virtual SampledSurface::P create( MeshType::P mesh, MeshType::P sampleMesh ) = 0;
      };

   }
}

#endif // __SAMPLEDMESHFACTORY_H__