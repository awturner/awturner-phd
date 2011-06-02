#ifndef __FACESINRADIUS_H__
#define __FACESINRADIUS_H__

#include "Mesh.h"

#include "SearchAgent.h"
#include "SearchFilter.h"

#include <vector>

namespace AWT
{
   namespace SimpleMesh
   {
      class FacesInRadius : public SearchAgent
      {
      public:
         typedef ManagedAutoPointer<FacesInRadius> P;

      protected:
         FacesInRadius( );
         virtual ~FacesInRadius( );

      public:
         static P getInstance( );
         virtual std::string getClassName( ) const;

         virtual void calculateBoxDistanceBounds2( OEKDTreeBranch<double,3>* in_Branch, SqDistBounds<double>& bounds ) const;

         virtual void checkObject( const int in_Index );

         virtual bool shouldCheck( AWT::OEKDTree::OEKDTreeBranch<double,3>* in_Branch, const SqDistBounds<double>& bounds ) const;

         virtual void setTestPoint( const Point in_TestPoint  );
         virtual Point getTestPoint( ) const;

         virtual void setTestRadius( const double r );
         virtual double getTestRadius( ) const;

         virtual void setStopAfterFirst( const bool r );
         virtual bool getStopAfterFirst( ) const;

         virtual std::vector<PointIndexWeights>& getFaces( );

         virtual void reset( );

         virtual void setData( AWT::OEKDTree::OEKDTreeData<double,3>::P data );

      protected:
         struct D;
         D* m_D;

      };
   }
}

#endif // __FACESINRADIUS_H__