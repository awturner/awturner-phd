#include "NearestSearch.h"

#include "TreeData.h"
#include "Mesh.h"
#include "MeshFunctions.h"

#include "Useful/ArrayVectorFunctions.h"
#include "MeshSearching/BoxQueries.h"
#include "MeshSearching/FaceQueries.h"

using namespace AWT;
using namespace AWT::SimpleMesh;
using namespace AWT::OEKDTree;

struct AWT::SimpleMesh::NearestSearch::D
{
   SearchFilter::P filter;

   Point testPoint;

   double maxDistance2;

   PointIndexWeights nearestPoint;

   bool found;
};

AWT::SimpleMesh::NearestSearch::NearestSearch( )
{
   m_D = new D;

   reset( );
}

AWT::SimpleMesh::NearestSearch::~NearestSearch( )
{
   delete m_D;
}

AWT::SimpleMesh::NearestSearch::P AWT::SimpleMesh::NearestSearch::getInstance( )
{
   AUTOGETINSTANCE( AWT::SimpleMesh::NearestSearch, ( ) );
}

GETNAMEMACRO( AWT::SimpleMesh::NearestSearch );

void AWT::SimpleMesh::NearestSearch::calculateBoxDistanceBounds2( OEKDTreeBranch<double,3>* in_Branch, AWT::OEKDTree::SqDistBounds<double>& bounds ) const
{
   boxChecked();

   bounds.lower = BoxQueries<double>::getClosestDistance2( m_D->testPoint.data_block(), in_Branch, m_D->maxDistance2 );
   bounds.upper = FAR_FAR_AWAY;
}

void AWT::SimpleMesh::NearestSearch::checkObject( const int in_Index )
{
   objectChecked();

   Point vs[3];
   getFaceVertices( mesh, in_Index, vs );

   Point p;
   FaceQueries<double>::getNearestPointOnTriangle( m_D->testPoint.data_block(), vs[0].data_block(), vs[1].data_block(), vs[2].data_block(), p.data_block() );
   p(3) = 1;

   const double thisDistance2 = deltaNormSquared( m_D->testPoint.data_block(), p.data_block(), 3 );

   // This is a bit minging because this is essentially another search filter... oh well
   if ( thisDistance2 < m_D->maxDistance2 )
   {
      // Accept if the filter says OK
      if ( m_D->filter->handle( p, in_Index ) )
      {
         // Make sure
         p[3] = 1;

         m_D->maxDistance2 = thisDistance2;
         m_D->nearestPoint.p = p;
         m_D->nearestPoint.i = in_Index;
         m_D->nearestPoint.w = calculateFaceWeights( mesh, in_Index, p );

         m_D->found = true;
      }
   }
}

bool AWT::SimpleMesh::NearestSearch::shouldCheck( AWT::OEKDTree::OEKDTreeBranch<double,3>* in_Branch, const AWT::OEKDTree::SqDistBounds<double>& bounds ) const
{
   return bounds.lower < m_D->maxDistance2;
}

void AWT::SimpleMesh::NearestSearch::setTestPoint( const Point in_TestPoint  )
{
   m_D->testPoint = in_TestPoint;
}

Point AWT::SimpleMesh::NearestSearch::getTestPoint( ) const
{
   return m_D->testPoint;
}

bool AWT::SimpleMesh::NearestSearch::isPointValid( ) const
{
   return m_D->found;
}

PointIndexWeights AWT::SimpleMesh::NearestSearch::getNearestPoint( ) const
{
   if ( !m_D->found )
      throw "No point was found!";

   return m_D->nearestPoint;
}

void AWT::SimpleMesh::NearestSearch::reset( )
{
   SearchAgent::reset();
   m_D->maxDistance2 = FAR_FAR_AWAY;

   m_D->nearestPoint = PointIndexWeights();

   m_D->found = false;
}

AWT::SimpleMesh::SearchFilter::P AWT::SimpleMesh::NearestSearch::getSearchFilter( )
{
   return m_D->filter;
}

void AWT::SimpleMesh::NearestSearch::setSearchFilter( SearchFilter::P v )
{
   if ( v != m_D->filter )
   {
      m_D->filter = v;
      modified( );
   }
}

void AWT::SimpleMesh::NearestSearch::setData( AWT::OEKDTree::OEKDTreeData<double,3>::P data )
{
   DEBUGMACRO( "Not doing anything with data!" );
}