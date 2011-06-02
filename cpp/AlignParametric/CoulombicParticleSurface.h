#ifndef __CoulombicParticleSurface_H__
#define __CoulombicParticleSurface_H__

#include "ParticleSurface.h"
#include "AlignParametric/CoulombicRegularizer.h"
#include "AlignParametric/TypeDefs.h"
#include "Mesh/Mesh.h"
#include "MeshSearching/FacesNearestPointSearch.h"

#include <iostream>

namespace AWT
{
   namespace AlignParametric
   {
      class CoulombicParticleSurface : public ParticleSurface
      {
         friend class CoulombicRegularizer;

      public:
         typedef ManagedAutoPointer<CoulombicParticleSurface> P;

      protected:
         CoulombicParticleSurface( MeshType::P mesh, const TuplesType::P samples, const Idx ntake );
         virtual ~CoulombicParticleSurface( );

      public:
         static P getInstance( MeshType::P mesh, const TuplesType::P samples, const Idx ntake );
         virtual std::string getClassName( ) const;

         // Distance to the closest neighbour particle
         virtual T getMinDistance( const Idx p );

         // Calculate the regularization cost
         virtual T regularizationCost( );

         // Calculate the regularization update
         virtual void regularizationGradient( MatrixType& reg, const Transformation& trans );

         virtual CoulombicRegularizer::P getRegularizer( );

      protected:
         CoulombicRegularizer::P regularizer;

         virtual int updatePointLocation( const Idx i, const T* vtx, FacesNearestPointSearch<T>::P searcher );
      };
   }
}

#endif // __CoulombicParticleSurface_H__