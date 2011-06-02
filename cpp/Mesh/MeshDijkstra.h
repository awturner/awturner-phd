#ifndef __MESHDIJKSTRA_H__
#define __MESHDIJKSTRA_H__

#include "ObjectManagement/ManagedObject.h"

#include "MeshTypeDefs.h"

#include "Mesh.h"
#include "Tuples.h"

namespace AWT
{
   template <class T>
   class MeshDijkstra : public ManagedObject
   {
   public:
      typedef ManagedAutoPointer<MeshDijkstra<T>> P;

   protected:
      MeshDijkstra( typename Mesh<T>::P mesh, typename Tuples<T>::P initialData );
      virtual ~MeshDijkstra( );

   public:
      static P getInstance( typename Mesh<T>::P mesh, typename Tuples<T>::P initialData );
      virtual std::string getClassName( ) const;

      virtual typename Tuples<T>::P getPointData( );

   protected:
      struct D;
      D* m_D;

   };
}

#endif // __MESHDIJKSTRA_H__