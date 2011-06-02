#ifndef __ALIGNOBJECT_H__
#define __ALIGNOBJECT_H__

#include "ObjectManagement/ManagedObject.h"

#include "Mesh.h"
#include "TypeDefs.h"
#include "Search.h"
#include "Sampler.h"

namespace AWT
{
   namespace SimpleMesh
   {
      class AlignObject : public ManagedObject
      {
      protected:
         Mesh::P mesh;
         Vector params;
         Search::P searcher;

         Sampler::P sampler;
         PIWs samples;

         const Index nsamples;

         AlignObject( const Index nsamples );

      public:
         typedef ManagedAutoPointer<AlignObject> P;

         Vector getParameters( ) const;

         void incrementParameters( const Vector& inc );

         virtual void setParameters( const Vector& params );

         virtual Mesh::P getMesh( );

         void resample( );

         Index getMaxNumberOfSamples( );

         PIWs& getSamples( );

         virtual PointIndexWeights search( const Point& pnt, const Point& nml, SearchFilter::P filter ) = 0;

         virtual void calculateJacobian( const PointIndexWeights& p, Matrix& jac ) = 0;
      };
   }
}

#endif // __ALIGNOBJECT_H__