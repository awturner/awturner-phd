#ifndef __SAMPLINGGRIDIRREGULAR_H__
#define __SAMPLINGGRIDIRREGULAR_H__

#include "ObjectManagement/ManagedObject.h"
#include "TypeDefs.h"
#include "Maths/BSpline.h"

#include "vnl/vnl_matrix.h"
#include "vnl/vnl_sparse_matrix.h"

#include "MeshBreaker/FlattenMeshPair.h"

namespace AWT
{
   namespace AlignParametric
   {
      class SamplingGridIrregular : public ManagedObject
      {
      protected:
         SamplingGridIrregular( TuplesType::P sampleLocations, const Idx _nk[2] );
         virtual ~SamplingGridIrregular( );

      protected:
         TuplesType::P originalSampleLocations;
         Idx nk[2];

         static T calculateWeight( const T spos[2], const T kpos[2], const T scale[2], const BSpline<T>& bspline );

         KernelWeights weightings;
         mutable ControlValues controlValues[2];
      public:
         typedef ManagedAutoPointer<SamplingGridIrregular> P;
         static P getInstance( TuplesType::P sampleLocations, const Idx _nk[2] );

         std::string getClassName( ) const;

         TuplesType::P getOriginalSampleLocations( );
         void getNKernels( Idx nk[2] ) const;

         void getPositions( Displacements positions[2] ) const;
         void getDisplacements( Displacements positions[2] ) const;
         
         const KernelWeights&  getWeights( ) const;
         ControlValues& getParameters( Idx axis );
      };
   }
}

#endif // __SAMPLINGGRIDIRREGULAR_H__