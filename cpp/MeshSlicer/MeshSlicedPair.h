#ifndef __MESHSLICEDPAIR_H__
#define __MESHSLICEDPAIR_H__

#include "ObjectManagement/ManagedObject.h"

namespace AWT
{
   template <class T>
   class Mesh;

   template <class T>
   class MeshSlicedPair : public ManagedObject
   {
   public:
      typedef ManagedAutoPointer<MeshSlicedPair<T>> P;

   protected:
      MeshSlicedPair( typename Mesh<T>::P included, typename Mesh<T>::P excluded );
      virtual ~MeshSlicedPair( );

   public:
      static P getInstance( typename Mesh<T>::P included, typename Mesh<T>::P excluded );
      virtual std::string getClassName( ) const;

      typename Mesh<T>::P getIncluded( );

      typename Mesh<T>::P getExcluded( );

   protected:
      struct D;
      D* m_D;

   };
}

#endif // __MESHSLICEDPAIR_H__