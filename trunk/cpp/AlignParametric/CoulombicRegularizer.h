#ifndef __COULOMBICREGULARIZER_H__
#define __COULOMBICREGULARIZER_H__

#include "ObjectManagement/ManagedObject.h"

#include "Mesh/Mesh.h"
#include "Mesh/Tuples.h"
#include "Useful/ValueRange.h"
#include "TypeDefs.h"

#include "Cates/ParticlesAndMesh.h"

namespace AWT
{
   namespace AlignParametric
   {
      class CoulombicParticleSurface;

      class CoulombicRegularizer : public ManagedObject
      {
      public:
         typedef ManagedAutoPointer<CoulombicRegularizer> P;

      protected:
         CoulombicRegularizer( CoulombicParticleSurface* surf );
         virtual ~CoulombicRegularizer( );

      public:
         static P getInstance( CoulombicParticleSurface* surf );
         virtual std::string getClassName( ) const;

         T getCutoffDistance( ) const;
         void setCutoffDistance( const T v );

         T getThreshold( ) const;
         void setThreshold( const T v );

         virtual T getMinDistance( const Idx p );

         // Calculate the force which tries to push these particles apart
         void calculateUpdate( MatrixType& reg );

         T calculateCost( const bool debug = true );

         void associateSampleToFace( const MeshIndex i, const MeshIndex f );

      protected:
         struct D;
         D* m_D;

      };
   }
}

#endif // __COULOMBICREGULARIZER_H__