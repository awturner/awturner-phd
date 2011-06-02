#ifndef __MESHSLICER_H__
#define __MESHSLICER_H__

#include "ObjectManagement/ManagedObject.h"

#include "MeshSlicedPair.h"

#include "CuttingFunction.h"

namespace AWT
{
   template <class T>
   class CuttingFunction;

   template <class T>
   class Mesh;

   template <class T>
   class MeshSlicer : public ManagedObject
   {
   public:
      typedef ManagedAutoPointer<MeshSlicer<T>> P;

   protected:
      MeshSlicer( );
      virtual ~MeshSlicer( );

   public:
      static P getInstance( );
      virtual std::string getClassName( ) const;

      virtual void setCuttingFunction( typename CuttingFunction<T>::P cutF );
      virtual typename CuttingFunction<T>::P getCuttingFunction( );

      virtual typename MeshSlicedPair<T>::P sliceMesh( typename Mesh<T>::P mesh );

   protected:
      struct D;
      D* m_D;

   };
}

#endif // __MESHSLICER_H__