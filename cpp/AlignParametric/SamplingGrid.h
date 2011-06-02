#ifndef __SAMPLINGGRID_H__
#define __SAMPLINGGRID_H__

#include "ObjectManagement/ManagedObject.h"
#include "TypeDefs.h"

#include "vnl/vnl_matrix.h"
#include "vnl/vnl_sparse_matrix.h"

#include "MeshBreaker/FlattenMeshPair.h"

namespace AWT
{
   namespace AlignParametric
   {
      class SamplingGrid : public ManagedObject
      {
      protected:
         SamplingGrid( const Idx _ns[2], const Idx _nk[2] );
         virtual ~SamplingGrid( );

      protected:
         Idx ns[2];
         Idx nk[2];

         KernelWeights weightings;
         ControlValues controlValues[2];
      public:
         typedef ManagedAutoPointer<SamplingGrid> P;
         static P getInstance( const Idx _ns[2], const Idx _nk[2] );

         std::string getClassName( ) const;

         void getNSamples( Idx ns[2] ) const;
         void getNKernels( Idx nk[2] ) const;

         const KernelWeights&  getWeights( ) const;
         ControlValues& getParameters( Idx axis );

         void getPositions( Displacements positions[2] ) const;
         void getDisplacements( Displacements positions[2] ) const;

         // Work out the control values necessary to yield the same displacements
         void calculateFromOther( const SamplingGrid& other );

      protected:
         static void getPointPosition( const Idx ns[2], const Idx i, double pos[2] );
      };
   }
}

#endif // __SAMPLINGGRID_H__