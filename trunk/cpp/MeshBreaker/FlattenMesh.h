#ifndef __FLATTENMESH_H__
#define __FLATTENMESH_H__

#include "ObjectManagement/ManagedObject.h"

#include "FlattenMeshPair.h"
#include "GeneratorProvider.h"

namespace AWT
{
   template <class T>
   class FlattenMesh : public ManagedObject
   {
   public:
      typedef ManagedAutoPointer<FlattenMesh> P;

      enum WeightType
      {
         WEIGHT_TUTTE,
         WEIGHT_FLOATER,
         WEIGHT_HARMONIC,
         WEIGHT_WACHSPRESS,
      };

      enum SpreadType
      {
         SPREAD_NONE,
         SPREAD_AWT,
         SPREAD_YOSHIZAWA,
      };

      static WeightType getWeightType( const char* str );
      static SpreadType getSpreadType( const char* str );

   protected:
      FlattenMesh( );
      virtual ~FlattenMesh( );

   public:
      static typename FlattenMeshPair<T>::P flattenToPair( typename GeneratorProvider<T>::P fg, const WeightType wt, const SpreadType st );
      static typename Mesh<T>::P flatten( typename GeneratorProvider<T>::P fg, const WeightType wt, const SpreadType st );

   protected:
      struct D;
   };
}

#endif // __FLATTENMESH_H__