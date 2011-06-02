#ifndef __VERTICESINRADIUS_H__
#define __VERTICESINRADIUS_H__

#include "Mesh.h"

#include "SearchAgent.h"
#include "SearchFilter.h"

#include <vector>

namespace AWT
{
   namespace SimpleMesh
   {
      class VerticesInRadius : public SearchAgent
      {
      public:
         typedef ManagedAutoPointer<VerticesInRadius> P;

      protected:
         VerticesInRadius( );
         virtual ~VerticesInRadius( );

      public:
         static P getInstance( );
         virtual std::string getClassName( ) const;

         virtual void calculateBoxDistanceBounds2( OEKDTreeBranch<double,3>* in_Branch, SqDistBounds<double>& bounds ) const;
         virtual bool shouldCheck( AWT::OEKDTree::OEKDTreeBranch<double,3>* in_Branch, const SqDistBounds<double>& bounds ) const;

         virtual void checkObject( const int in_Index );

         virtual void setTestPoint( const Point in_TestPoint  );
         virtual Point getTestPoint( ) const;

         virtual void setOuterRadius( const double r );
         virtual double getOuterRadius( ) const;

         virtual void setInnerRadius( const double r );
         virtual double getInnerRadius( ) const;

         virtual std::vector<Index>& getVertices( );

         virtual void reset( );

         virtual void setData( AWT::OEKDTree::OEKDTreeData<double,3>::P data );
      protected:
         struct D;
         D* m_D;

      };
   }
}

#endif // __VERTICESINRADIUS_H__