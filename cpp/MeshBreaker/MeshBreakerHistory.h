#ifndef __MESHBREAKERHISTORY_H__
#define __MESHBREAKERHISTORY_H__

#include "ObjectManagement/ManagedObject.h"

#include "MeshBreaker.h"

namespace AWT
{
   template <class T>
   class MeshBreakerHistory : public ManagedObject
   {
   public:
      typedef ManagedAutoPointer<MeshBreakerHistory> P;

   protected:
      MeshBreakerHistory( const typename MeshBreaker<T>::Symbol sym, const MeshIndex f, const MeshIndex g, const MeshIndex distVert, const MeshIndex lneigh, const MeshIndex rneigh  );
      virtual ~MeshBreakerHistory( );

   public:
      static P getInstance( const typename MeshBreaker<T>::Symbol sym, const MeshIndex f, const MeshIndex g, const MeshIndex distVert, const MeshIndex lneigh, const MeshIndex rneigh );
      virtual std::string getClassName( ) const;

      virtual typename MeshBreaker<T>::Symbol getSymbol( ) const;

      virtual MeshIndex getF( ) const;
      virtual MeshIndex getG( ) const;

      virtual MeshIndex getDistalVertex( ) const;

      virtual MeshIndex getLeftNeighbour( ) const;
      virtual MeshIndex getRightNeighbour( ) const;
         
   protected:
      struct D;
      D* m_D;

   };
}

#endif // __MESHBREAKERHISTORY_H__