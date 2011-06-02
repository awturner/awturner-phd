#ifndef __MESHSAMPLER_H__
#define __MESHSAMPLER_H__

#include "ObjectManagement/ManagedObject.h"

#include "Mesh.h"
#include "SamplePoints.h"

namespace AWT
{
   template <class T>
   class MeshSampler : public virtual ManagedObject
   {
   public:
      typedef ManagedAutoPointer<MeshSampler<T>> P;

   public:
      virtual SamplePoints::P sampleMesh( typename Mesh<T>::P mesh ) = 0;

      virtual unsigned int getMaximumNumberOfSamples( ) const = 0;

      virtual void setMaximumNumberOfSamples( const unsigned int s ) = 0;
   };
}

#endif // __MESHSAMPLER_H__