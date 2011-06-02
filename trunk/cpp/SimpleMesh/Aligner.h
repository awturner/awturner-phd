#ifndef __ALIGNER_H__
#define __ALIGNER_H__

#include "ObjectManagement/ManagedObject.h"

#include "TypeDefs.h"
#include "ShapeModel.h"
#include "Mesh.h"

#include "AlignObject.h"
#include "FragmentObject.h"
#include "ModelObject.h"
#include "AllSearchFilters.h"

namespace AWT
{
   namespace SimpleMesh
   {
      class Aligner : public ManagedObject
      {
      public:
         typedef ManagedAutoPointer<Aligner> P;
         enum CostFunctionType { CF_POINTTOPOINT, CF_POINTTOPLANE };

      protected:
         Aligner( CostFunctionType cft );
         virtual ~Aligner( );

      public:
         static P getInstance( CostFunctionType cft );
         virtual std::string getClassName( ) const;

         CostFunctionType getCostFunctionType( ) const;
         void setCostFunctionType( const CostFunctionType& v );

         void setModel( ShapeModel::P model, SamplerFactory::P fact, const Index nsamples, Vector params );
         void addFragment( Mesh::P frag, Sampler::P samp, const Index nsamples, Vector params );

         void setFragmentNoise( const double v );
         double getFragmentNoise( ) const;

         const Vector getModelParameters( ) const;
         void setModelParameters( const Vector& vec );

         Index getNumberOfFragments( ) const;

         const Vector getFragmentPose( const Index i ) const;
         void setFragmentPose( const Index i, const Vector& vec );

         const Transformation getFragmentTransformation( const Index i ) const;

         const Vector getTargetRegistrationError( const Index i, const Index j ) const;

         LameckerMeasures getLameckerMeasures( );

         void resample( );

         void writeModels( const std::string& saveFormat );

         Index findCorrespondences( );

         double stepUpdate( );

         void setNumberOfModes( const Index i );
         void activateAllModes( );

         Index getNumberOfModes( ) const;

         double getTrimFactor() const;
         void setTrimFactor( const double v );

      protected:
         struct D;
         D* m_D;

      };
   }
}

#endif // __ALIGNER_H__