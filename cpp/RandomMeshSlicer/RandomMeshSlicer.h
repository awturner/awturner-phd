#ifndef __RANDOMMESHSLICER_H__
#define __RANDOMMESHSLICER_H__

#include "ObjectManagement/ManagedObject.h"

#include "Mesh/MeshTypeDefs.h"

namespace AWT
{
   template <class T>
   class RandomMeshSlicer : public ManagedObject
   {
   public:
      typedef ManagedAutoPointer<RandomMeshSlicer> P;

   protected:
      RandomMeshSlicer( typename Mesh<T>::P mesh, unsigned int nplanes );
      virtual ~RandomMeshSlicer( );

   public:
      static P getInstance( typename Mesh<T>::P mesh, const unsigned int nplanes );
      virtual std::string getClassName( ) const;

      unsigned int getNumberOfFragments( ) const;

      typename Mesh<T>::P getFragment( const unsigned int f );

   protected:
      struct D;
      D* m_D;

   };
}

#endif // __RANDOMMESHSLICER_H__