#ifndef __MESHAPPROXGEODESIC_H__
#define __MESHAPPROXGEODESIC_H__

#include "ObjectManagement/ManagedObject.h"
#include "Mesh/Mesh.h"

namespace AWT
{
   template <class T>
   class MeshApproxGeodesic : public ManagedObject
   {
   public:
      typedef ManagedAutoPointer<MeshApproxGeodesic<T>> P;

   protected:
      MeshApproxGeodesic( typename Mesh<T>::P mesh );
      virtual ~MeshApproxGeodesic( );

   public:
      static P getInstance( typename Mesh<T>::P mesh );
      virtual std::string getClassName( ) const;

      void setOrigin( const T vtxStart[3], const T distMax );
      void setOrigin( const T vtxStart[3], const MeshIndex cell, const T distMax );

      MeshIndex getNumberOfFoundCells( ) const;
      MeshIndex getFoundCell( const MeshIndex i ) const;

      T distanceTo( const T vtxEnd[3] ) const;
      T distanceTo( const T vtxEnd[3], const MeshIndex cell ) const;

      void directionTo( const T vtxEnd[3], T dir[3], const bool debug = false ) const;
      void directionTo( const T vtxEnd[3], const MeshIndex cell, T dir[3], const bool debug = false ) const;

   protected:
      struct D;
      D* m_D;

   };
}

#endif // __MESHAPPROXGEODESIC_H__