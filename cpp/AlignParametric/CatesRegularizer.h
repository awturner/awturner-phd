#ifndef __CatesRegularizer_H__
#define __CatesRegularizer_H__

#include "ObjectManagement/ManagedObject.h"

#include "Mesh/Mesh.h"
#include "Mesh/Tuples.h"
#include "Useful/ValueRange.h"
#include "TypeDefs.h"

namespace AWT
{
   namespace AlignParametric
   {
      class CatesParticleSurface;

      class CatesRegularizer : public ManagedObject
      {
      public:
         typedef ManagedAutoPointer<CatesRegularizer> P;

         enum DistanceType { DT_EUCLIDEAN, DT_APPROXGEODESIC };

      protected:
         CatesRegularizer( CatesParticleSurface* surf, const ValueRange<T> sigmaRange );
         virtual ~CatesRegularizer( );

      public:
         static P getInstance( CatesParticleSurface* surf, const ValueRange<T> sigmaRange );
         virtual std::string getClassName( ) const;

         TuplesType::P getSigma( );

         DistanceType getDistanceType( ) const;
         void setDistanceType( const DistanceType dt );

         T getSigma( MeshIndex i ) const;
         void setSigma( MeshIndex i, const T v );

         // Calculate the force which tries to push these particles apart
         void calculateUpdate( MatrixType& reg );

         T calculateCost( const bool debug = true );
         T calculateNegativeEntropy( );

         T getMinSigma( ) const;

         const ValueRange<T>& getSigmaRange( ) const;
         void setSigmaRange( const ValueRange<T>& sigmaRange );

         void associateSampleToFace( const MeshIndex i, const MeshIndex f );

         void fixSigma(const bool state);

      protected:
         struct D;
         D* m_D;

      };
   }
}

#endif // __CatesRegularizer_H__