#ifndef __ParticleSurfaceRefinement_H__
#define __ParticleSurfaceRefinement_H__

#include "RefinementStrategy.h"
#include "CoulombicParticleSurface.h"

namespace AWT
{
   namespace AlignParametric
   {
      class ParticleSurfaceRefinement : public RefinementStrategy
      {
      public:
         typedef ManagedAutoPointer<ParticleSurfaceRefinement> P;

      protected:
         ParticleSurfaceRefinement( );
         virtual ~ParticleSurfaceRefinement( );

      public:
         static P getInstance( );
         virtual std::string getClassName( ) const;

         const Idx getMaxParticles( ) const;
         void setMaxParticles( const Idx v );

         const Idx getSpreadBlockTime( ) const;
         void setSpreadBlockTime( const Idx v );

         const Idx getParticleMaturityAge( ) const;
         void setParticleMaturityAge( const Idx v );

         const T getMinSplitSeparation( ) const;
         void setMinSplitSeparation( const T v );

         virtual void addParticleSurface( ParticleSurface::P surf );

         virtual bool refine( AWT::Container::Iterator<Idx>::P iter );

      protected:
         struct D;
         D* m_D;

      };
   }
}

#endif // __ParticleSurfaceRefinement_H__