#ifndef __VERTICESBETWEENPLANES_H__
#define __VERTICESBETWEENPLANES_H__

#include "Mesh.h"

#include "SearchAgent.h"
#include "SearchFilter.h"

#include <vector>

namespace AWT
{
   namespace SimpleMesh
   {
      class VerticesBetweenPlanes : public SearchAgent
      {
      public:
         typedef ManagedAutoPointer<VerticesBetweenPlanes> P;

      protected:
         VerticesBetweenPlanes( const Index nplanes = 2 );
         virtual ~VerticesBetweenPlanes( );

      public:
         static P getInstance( const Index nplanes = 2 );
         virtual std::string getClassName( ) const;

         virtual void calculateBoxDistanceBounds2( OEKDTreeBranch<double,3>* in_Branch, SqDistBounds<double>& bounds ) const;
         virtual bool shouldCheck( AWT::OEKDTree::OEKDTreeBranch<double,3>* in_Branch, const SqDistBounds<double>& bounds ) const;

         virtual void checkObject( const int in_Index );

         virtual void setTestPlane( const Index i, const Point& in_Plane );
         virtual Point getTestPlane( const Index i ) const;

         virtual std::vector<Index>& getVertices( );

         virtual void reset( );

         virtual void setData( AWT::OEKDTree::OEKDTreeData<double,3>::P data );
      protected:
         struct D;
         D* m_D;

      };
   }
}

#endif // __VERTICESBETWEENPLANES_H__