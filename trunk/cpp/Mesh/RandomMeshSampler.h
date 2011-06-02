#ifndef __RANDOMMESHSAMPLER_H__
#define __RANDOMMESHSAMPLER_H__

#include "MeshSampler.h"
#include "MeshTypeDefs.h"

#include "SamplePoints.h"

namespace AWT
{
   template <class T>
   class RandomMeshSampler : public MeshSampler<T>
   {
   public:
      typedef ManagedAutoPointer<RandomMeshSampler<T>> P;

   protected:
      RandomMeshSampler( );
      virtual ~RandomMeshSampler( );

   public:
      static P getInstance( );
      virtual SamplePoints::P sampleMesh( typename Mesh<T>::P mesh );

      virtual unsigned int getMaximumNumberOfSamples( ) const;
      virtual void setMaximumNumberOfSamples( const unsigned int s );

      virtual std::string getClassName( ) const;

   protected:
      struct D;
      D* m_D;
   };
}

#endif // __RANDOMMESHSAMPLER_H__