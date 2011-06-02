#ifndef __TRIANGLEPROJECTION_H__
#define __TRIANGLEPROJECTION_H__

#include "ObjectManagement/ManagedObject.h"

#include "Mesh.h"

namespace AWT
{
   template <class T>
   class TriangleProjection : public ManagedObject
   {
   public:
      typedef ManagedAutoPointer<TriangleProjection> P;

   protected:
      TriangleProjection( );
      virtual ~TriangleProjection( );

   public:
      virtual std::string getClassName( ) const;

      static void projectFromTriangle( const typename Mesh<T>::P in_Mesh, const MeshIndex in_Index, const T* vertex2d, T* vertex3d );
      static void projectFromTriangle( const T* vtxOrigin, const T* vtxX, const T* vtxY, const T* vertex2d, T* vertex3d );

      static void projectOntoTriangle( const typename Mesh<T>::P in_Mesh, const MeshIndex in_Index, const T* vertex3d, T* vertex2d );
      static void projectOntoTriangle( const T* vtxOrigin, const T* vtxX, const T* vtxY, const T* vertex3d, T* vertex2d );

   protected:
      struct D;
      D* m_D;

   };
}

#endif // __TRIANGLEPROJECTION_H__