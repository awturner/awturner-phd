#ifndef __CatesParticleSurface_H__
#define __CatesParticleSurface_H__

#include "ParticleSurface.h"
#include "CatesRegularizer.h"
#include "AlignParametric/SampledSurface.h"
#include "AlignParametric/TypeDefs.h"
#include "Mesh/Mesh.h"
#include "MeshSearching/FacesNearestPointSearch.h"

#include <iostream>

namespace AWT
{
   namespace AlignParametric
   {
      class CatesParticleSurface : public ParticleSurface
      {
         friend class CatesRegularizer;

      public:
         typedef ManagedAutoPointer<CatesParticleSurface> P;

      protected:
         CatesParticleSurface( MeshType::P mesh, const TuplesType::P samples, const Idx ntake, const ValueRange<T> _sigmaRange );
         virtual ~CatesParticleSurface( );

      public:
         static P getInstance( MeshType::P mesh, const TuplesType::P samples, const Idx ntake, const ValueRange<T> _sigmaRange );
         virtual std::string getClassName( ) const;

         // Distance to the closest neighbour particle
         virtual T getMinDistance( const Idx p );

         // Calculate the regularization cost
         virtual T regularizationCost( );

         // Calculate the regularization update
         virtual void regularizationGradient( MatrixType& reg, const Transformation& trans );

         virtual CatesRegularizer::P getRegularizer( );

         void fixSigma(const bool state);

      protected:
         CatesRegularizer::P regularizer;

         virtual int updatePointLocation( const Idx i, const T* vtx, FacesNearestPointSearch<T>::P searcher );
      };
   }
}

#endif // __CatesParticleSurface_H__