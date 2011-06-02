#ifndef __UNIFORMSAMPLER_H__
#define __UNIFORMSAMPLER_H__

#include "ObjectManagement/ManagedObject.h"

#include "Sampler.h"

namespace AWT
{
   namespace SimpleMesh
   {
      class UniformSampler : public Sampler
      {
      public:
         typedef ManagedAutoPointer<UniformSampler> P;

      protected:
         UniformSampler( Mesh::P mesh );
         virtual ~UniformSampler( );

         virtual void calculateCumAreas( );

      public:
         static P getInstance( Mesh::P mesh );
         virtual std::string getClassName( ) const;
      };

      class UniformSamplerFactory : public SamplerFactory
      {
      protected:
         UniformSamplerFactory( );
         virtual ~UniformSamplerFactory( );

      public:
         typedef ManagedAutoPointer<UniformSamplerFactory> P;
         static P getInstance( );
         std::string getClassName( ) const;

         virtual Sampler::P createInstance( Mesh::P mesh );
      };
   }
}

#endif // __UNIFORMSAMPLER_H__