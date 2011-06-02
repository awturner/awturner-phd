#ifndef __NORMALSPACESAMPLER_H__
#define __NORMALSPACESAMPLER_H__

#include "ObjectManagement/ManagedObject.h"

#include "Sampler.h"

namespace AWT
{
   namespace SimpleMesh
   {
      class NormalSpaceSampler : public Sampler
      {
      public:
         typedef ManagedAutoPointer<NormalSpaceSampler> P;

      protected:
         NormalSpaceSampler( Mesh::P mesh, const unsigned int latdivs );
         virtual ~NormalSpaceSampler( );

         virtual void calculateCumAreas( );

      public:
         static P getInstance( Mesh::P mesh, const unsigned int latdivs );
         virtual std::string getClassName( ) const;

      protected:
         const unsigned int latdivs;
      };

      class NormalSpaceSamplerFactory : public SamplerFactory
      {
      protected:
         NormalSpaceSamplerFactory( const unsigned int latdivs );
         virtual ~NormalSpaceSamplerFactory( );

      public:
         typedef ManagedAutoPointer<NormalSpaceSamplerFactory> P;
         static P getInstance( const unsigned int latdivs );
         std::string getClassName( ) const;

         virtual Sampler::P createInstance( Mesh::P mesh );

      protected:
         const unsigned int latdivs;
      };
   }
}

#endif // __NORMALSPACESAMPLER_H__