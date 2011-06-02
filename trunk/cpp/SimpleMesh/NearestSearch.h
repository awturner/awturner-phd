#ifndef __SIMPLEMESHNEARESTSEARCH_H__
#define __SIMPLEMESHNEARESTSEARCH_H__

#include "Mesh.h"
#include "SearchAgent.h"

#include "SearchFilter.h"

namespace AWT
{
   namespace SimpleMesh
   {
      class NearestSearch : public SearchAgent
      {
      public:
         typedef ManagedAutoPointer<NearestSearch> P;

      protected:
         NearestSearch( );
         virtual ~NearestSearch( );

      public:
         static P getInstance( );
         virtual std::string getClassName( ) const;
      
         virtual void calculateBoxDistanceBounds2( OEKDTree::OEKDTreeBranch<double,3>* in_Branch, OEKDTree::SqDistBounds<double>& bounds ) const;
         virtual bool shouldCheck( AWT::OEKDTree::OEKDTreeBranch<double,3>* in_Branch, const OEKDTree::SqDistBounds<double>& bounds ) const;
         
         virtual void checkObject( const int in_Index );

         virtual void setTestPoint( const Point in_TestPoint  );
         virtual Point getTestPoint( ) const;

         virtual bool isPointValid( ) const;
         virtual PointIndexWeights getNearestPoint( ) const;

         virtual SearchFilter::P getSearchFilter( );
         virtual void setSearchFilter( SearchFilter::P v );

         virtual void reset( );

         virtual void setData( AWT::OEKDTree::OEKDTreeData<double,3>::P data );

      protected:
         struct D;
         D* m_D;

      };
   }
}

#endif // __SIMPLEMESHNEARESTSEARCH_H__