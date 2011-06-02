#ifndef __RAYSEARCH_H__
#define __RAYSEARCH_H__

#include "Mesh.h"
#include "SearchAgent.h"

#include "SearchFilter.h"

#include <limits>

namespace AWT
{
   namespace SimpleMesh
   {
      class RaySearch : public SearchAgent
      {
      public:
         typedef ManagedAutoPointer<RaySearch> P;

      protected:
         RaySearch( const double in_MaxDistance = std::numeric_limits<double>::infinity( ) );
         virtual ~RaySearch( );

      public:
         static P getInstance( const double in_MaxDistance = std::numeric_limits<double>::infinity( ) );
         virtual std::string getClassName( ) const;

         virtual void calculateBoxDistanceBounds2( OEKDTree::OEKDTreeBranch<double,3>* in_Branch, OEKDTree::SqDistBounds<double>& bounds ) const;
         virtual bool shouldCheck( AWT::OEKDTree::OEKDTreeBranch<double,3>* in_Branch, const OEKDTree::SqDistBounds<double>& bounds ) const;

         virtual void checkObject( const int in_Index );

         virtual SearchFilter::P getSearchFilter( );
         virtual void setSearchFilter( SearchFilter::P v );

         virtual void setTestPoint( const Point in_TestPoint  );
         virtual Point getTestPoint( ) const;

         virtual void setTestNormal( const Point in_TestNormal );
         virtual Point getTestNormal( ) const;

         virtual bool isPointValid( ) const;
         virtual PointIndexWeights getNearestPoint( ) const;

         virtual void reset( );

         virtual void setData( AWT::OEKDTree::OEKDTreeData<double,3>::P data );
      protected:
         struct D;
         D* m_D;
      };
   }
}

#endif // __RAYSEARCH_H__