#ifndef __SAMPLER_H__
#define __SAMPLER_H__

#include "ObjectManagement/ManagedObject.h"

#include "Mesh.h"

namespace AWT
{
   namespace SimpleMesh
   {
      class Sampler : public ManagedObject
      {
      protected:
         Sampler( Mesh::P mesh );
         virtual ~Sampler( );

      public:
         typedef ManagedAutoPointer<Sampler> P;

         Mesh::P getMesh( );

         virtual PointIndexWeights sample( );

      protected:
         virtual void calculateCumAreas( ) = 0;

         Mesh::P mesh;
         double * cumAreas;
      };

      class SamplerFactory : public ManagedObject
      {
      public:
         typedef ManagedAutoPointer<SamplerFactory> P;
         virtual Sampler::P createInstance( Mesh::P mesh ) = 0;
      };
   }
}

#endif // __SAMPLER_H__