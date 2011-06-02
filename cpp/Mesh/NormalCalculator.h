#ifndef __NORMALCALCULATOR_H__
#define __NORMALCALCULATOR_H__

#include "ObjectManagement/ManagedObject.h"

#include "Tuples.h"
#include "Mesh.h"

namespace AWT
{
   template <class T>
   class NormalCalculator : public ManagedObject
   {
   public:
      typedef ManagedAutoPointer<NormalCalculator<T>> P;

   public:
      virtual void calculateNormals( typename Mesh<T>::P mesh, typename Tuples<T>::P vtxNormals, typename Tuples<T>::P meshNormals ) = 0;

      virtual void calculateNormalsAndSet( typename Mesh<T>::P mesh, typename Tuples<T>::P vtxNormals = Tuples<T>::P( ), typename Tuples<T>::P faceNormals = Tuples<T>::P( ) );
   };
}

#endif // __NORMALCALCULATOR_H__