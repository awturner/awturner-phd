#ifndef __PARTICLESYSTEM_H__
#define __PARTICLESYSTEM_H__

#include "ObjectManagement/ManagedObject.h"

#include "ParticlesAndMesh.h"

namespace AWT
{
   template <class T>
   class ParticleSystem : public ManagedObject
   {
   public:
      typedef ManagedAutoPointer<ParticleSystem> P;

   protected:
      ParticleSystem( );
      virtual ~ParticleSystem( );

   public:
      static P getInstance( );
      virtual std::string getClassName( ) const;

      MeshIndex getNumberOfMeshes( );
      MeshIndex getNumberOfParticles( );

      void setSigmaBounds( const T lower, const T upper );

      typename Mesh<T>::P getMesh( MeshIndex i );
      typename Tuples<T>::P getParticles( MeshIndex i );

      void addMesh( typename Mesh<T>::P mesh, typename Tuples<T>::P particles );

      void update( const T alpha );

      void split( );

   protected:
      struct D;
      D* m_D;

   };
}

#endif // __PARTICLESYSTEM_H__