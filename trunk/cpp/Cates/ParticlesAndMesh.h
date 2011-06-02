#ifndef __PARTICLESANDMESH_H__
#define __PARTICLESANDMESH_H__

#include "ObjectManagement/ManagedObject.h"

#include "Mesh/Mesh.h"
#include "Mesh/Tuples.h"
#include "Useful/ValueRange.h"

namespace AWT
{
   template <class T>
   class ParticlesAndMesh : public ManagedObject
   {
   public:
      typedef ManagedAutoPointer<ParticlesAndMesh<T>> P;

   protected:
      ParticlesAndMesh( typename Mesh<T>::P mesh, typename Tuples<T>::P particles );
      virtual ~ParticlesAndMesh( );

   public:
      static P getInstance( typename Mesh<T>::P mesh, typename Tuples<T>::P particles );
      virtual std::string getClassName( ) const;

      typename Mesh<T>::P getMesh( );
      typename Tuples<T>::P getParticles( );
      typename Tuples<T>::P getSigma( );

      // Add the change on to the particles, and then reproject the points back onto the surface
      void updateParticles( const typename Tuples<T>::P delta, T mult, typename Tuples<T>::P actualDelta );

      void setSigmaRange( const ValueRange<T>& range );

      void splitParticles( );
      void splitParticle( const MeshIndex i );

      T getMinSigma( ) const;

      // Calculate the force which tries to push these particles apart
      T calculateInternalForceUpdate( const ValueRange<T>& sigmaRange, typename Tuples<T>::P delta, T& maxTimestep );

      void debugSigmas( const ValueRange<T>& sigmaRange );

   protected:
      struct D;
      D* m_D;

   };
}

#endif // __PARTICLESANDMESH_H__