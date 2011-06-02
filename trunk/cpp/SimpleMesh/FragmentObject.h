#ifndef __FRAGMENTOBJECT_H__
#define __FRAGMENTOBJECT_H__

#include "AlignObject.h"
#include "MeshEdgeSearchFilter.h"

namespace AWT
{
   namespace SimpleMesh
   {
      class FragmentObject : public AlignObject
      {
      public:
         typedef ManagedAutoPointer<FragmentObject> P;

      protected:
         FragmentObject( Mesh::P frag, Sampler::P sampler, Index nsamples );
         virtual ~FragmentObject( );

      public:
         static P getInstance( Mesh::P frag, Sampler::P sampler, Index nsamples );

         std::string getClassName( ) const;

         void setParameters( const Vector& params );

         virtual PointIndexWeights search( const Point& pnt, const Point& nml, SearchFilter::P filter );

         Transformation getTransformation( ) const;

         Transformation getInverseTransformation( ) const;

         SearchFilter::P getEdgeFilter( );

         virtual void calculateJacobian( const PointIndexWeights& p, Matrix& jac );

      protected:
         struct D;
         D* m_D;
      };
   }
}

#endif // __FRAGMENTOBJECT_H__