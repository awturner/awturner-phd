#ifndef __FACESINTERSECTPLANE_H__
#define __FACESINTERSECTPLANE_H__

#include "Mesh.h"
#include "SearchAgent.h"

#include "SearchFilter.h"
#include <vector>

namespace AWT
{
   namespace SimpleMesh
   {
      struct IndexLine
      {
         Index i;
         Point p[2];
      };

      class FacesIntersectPlane : public SearchAgent
      {
      public:
         typedef ManagedAutoPointer<FacesIntersectPlane> P;

      protected:
         FacesIntersectPlane( );
         virtual ~FacesIntersectPlane( );

      public:
         static P getInstance( );
         virtual std::string getClassName( ) const;

         virtual void calculateBoxDistanceBounds2( OEKDTreeBranch<double,3>* in_Branch, SqDistBounds<double>& bounds ) const;

         virtual bool shouldCheck( OEKDTreeBranch<double,3>* in_Branch, const SqDistBounds<double>& bounds ) const;

         virtual void checkObject( const int in_Index );

         virtual void setTestPlane( const Point in_Plane );
         virtual Point getTestPlane( ) const;

         virtual std::vector<IndexLine>& getFaces( );

         virtual void reset( );

         virtual void setData( AWT::OEKDTree::OEKDTreeData<double,3>::P data );

      protected:
         struct D;
         D* m_D;

      };
   }
}

#endif // __FACESINTERSECTPLANE_H__