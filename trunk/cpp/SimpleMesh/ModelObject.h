#ifndef __MODELOBJECT_H__
#define __MODELOBJECT_H__

#include "AlignObject.h"

#include "ShapeModel.h"
#include "Sampler.h"

namespace AWT
{
   namespace SimpleMesh
   {
      class ModelObject : public AlignObject
      {
      public:
         typedef ManagedAutoPointer<ModelObject> P;

      protected:
         ModelObject( ShapeModel::P model, SamplerFactory::P fact, Index nsamples );
         virtual ~ModelObject( );

      public:
         static P getInstance( ShapeModel::P model, SamplerFactory::P fact, Index nsamples );

         std::string getClassName( ) const;

         ShapeModel::P getModel( );

         void setParameters( const Vector& params );

         void activateAllModes( );
         void setNumberOfModes( const Index i );

         Index getNumberOfModes( ) const;

         virtual PointIndexWeights search( const Point& pnt, const Point& nml, SearchFilter::P filter );

         virtual void calculateJacobian( const PointIndexWeights& p, Matrix& jac );

      protected:
         struct D;
         D* m_D;
      };
   }
}

#endif // __MODELOBJECT_H__