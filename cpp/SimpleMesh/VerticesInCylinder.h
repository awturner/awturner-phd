#ifndef __VERTICESINCYLINDER_H__
#define __VERTICESINCYLINDER_H__

#include "Mesh.h"

#include "SearchAgent.h"
#include "SearchFilter.h"

#include <vector>

namespace AWT
{
   namespace SimpleMesh
   {
      class VerticesInCylinder : public SearchAgent
      {
      public:
         typedef ManagedAutoPointer<VerticesInCylinder> P;

      protected:
         VerticesInCylinder( );
         virtual ~VerticesInCylinder( );

      public:
         static P getInstance( );
         virtual std::string getClassName( ) const;

         virtual void calculateBoxDistanceBounds2( OEKDTreeBranch<double,3>* in_Branch, SqDistBounds<double>& bounds ) const;
         virtual bool shouldCheck( AWT::OEKDTree::OEKDTreeBranch<double,3>* in_Branch, const SqDistBounds<double>& bounds ) const;

         virtual void checkObject( const int in_Index );

         virtual void setCentre( const Point in_TestPoint  );
         virtual Point getCentre( ) const;

         virtual void setAxis( const Point in_TestPoint  );
         virtual Point getAxis( ) const;

         virtual void setRadius( const double outer, const double inner );
         virtual void getRadius( double& outer, double& inner ) const;

         virtual void setEnds( const double upper, const double lower );
         virtual void getEnds( double& upper, double& lower ) const;

         virtual std::vector<Index>& getVertices( );

         virtual void reset( );

         virtual void setData( AWT::OEKDTree::OEKDTreeData<double,3>::P data );

      protected:
         struct D;
         D* m_D;

      };
   }
}

#endif // __VERTICESINCYLINDER_H__