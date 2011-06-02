#ifndef __MESHSMOOTHING_H__
#define __MESHSMOOTHING_H__

#include "Mesh/Mesh.h"
#include "Mesh/Tuples.h"

namespace AWT
{
   template <class T>
   class MeshSmoothing
   {
   public:
      static typename Mesh<T>::P smooth( typename Mesh<T>::P m, T spatialSigma, T influenceSigma );

   protected:
      static typename Mesh<T>::P smoothImpl( typename Mesh<T>::P m, T spatialSigma, T influenceSigma, bool mollify, typename Tuples<T>::P n = typename Tuples<T>::P( ) );
   };
}

#endif // __MESHSMOOTHING_H__