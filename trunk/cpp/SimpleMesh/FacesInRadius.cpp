#include "FacesInRadius.h"

#include "TreeData.h"
#include "Mesh.h"
#include "MeshFunctions.h"

#include "Useful/ArrayVectorFunctions.h"
#include "MeshSearching/BoxQueries.h"
#include "MeshSearching/FaceQueries.h"

using namespace AWT;
using namespace AWT::SimpleMesh;
using namespace AWT::OEKDTree;

struct AWT::SimpleMesh::FacesInRadius::D
{
   Point testPoint;
   double testRadius2;

   bool stopAfterFirst;

   std::vector<PointIndexWeights> foundPoints;
};

AWT::SimpleMesh::FacesInRadius::FacesInRadius( )
{
   m_D = new D;

   m_D->stopAfterFirst = false;

   reset( );
}

AWT::SimpleMesh::FacesInRadius::~FacesInRadius( )
{
   delete m_D;
}

AWT::SimpleMesh::FacesInRadius::P AWT::SimpleMesh::FacesInRadius::getInstance( )
{
   AUTOGETINSTANCE( AWT::SimpleMesh::FacesInRadius, ( ) );
}

GETNAMEMACRO( AWT::SimpleMesh::FacesInRadius );

void AWT::SimpleMesh::FacesInRadius::calculateBoxDistanceBounds2( OEKDTreeBranch<double,3>* in_Branch, SqDistBounds<double>& bounds ) const
{
   boxChecked();

   bounds.lower = BoxQueries<double>::getClosestDistance2( m_D->testPoint.data_block(), in_Branch, m_D->testRadius2 );
   bounds.upper = FAR_FAR_AWAY;
}

bool AWT::SimpleMesh::FacesInRadius::shouldCheck( AWT::OEKDTree::OEKDTreeBranch<double,3>* in_Branch, const SqDistBounds<double>& bounds ) const
{
   if ( m_D->stopAfterFirst && !m_D->foundPoints.empty() )
      return false;

   return bounds.lower < m_D->testRadius2;
}

void AWT::SimpleMesh::FacesInRadius::checkObject( const int in_Index )
{
   objectChecked();

   Point vs[3];
   getFaceVertices( mesh, in_Index, vs );

   Point p;
   FaceQueries<double>::getNearestPointOnTriangle( m_D->testPoint.data_block(), vs[0].data_block(), vs[1].data_block(), vs[2].data_block(), p.data_block() );
   p(3) = 1;

   const double thisDistance2 = deltaNormSquared( m_D->testPoint.data_block(), p.data_block(), 3 );

   // This is a bit minging because this is essentially another search filter... oh well
   if ( thisDistance2 < m_D->testRadius2 )
   {
      // Make sure
      p[3] = 1;

      PointIndexWeights piw;
      piw.p = p;
      piw.i = in_Index;
      piw.w = calculateFaceWeights( mesh, in_Index, p );

      m_D->foundPoints.push_back( piw );
   }
}

void AWT::SimpleMesh::FacesInRadius::setTestPoint( const Point in_TestPoint  )
{
   m_D->testPoint = in_TestPoint;
}

Point AWT::SimpleMesh::FacesInRadius::getTestPoint( ) const
{
   return m_D->testPoint;
}

void AWT::SimpleMesh::FacesInRadius::setTestRadius( const double v )
{
   m_D->testRadius2 = v * v;
}

double AWT::SimpleMesh::FacesInRadius::getTestRadius( ) const
{
   return sqrt(m_D->testRadius2);
}

void AWT::SimpleMesh::FacesInRadius::setStopAfterFirst( const bool v )
{
   m_D->stopAfterFirst = v;
}

bool AWT::SimpleMesh::FacesInRadius::getStopAfterFirst( ) const
{
   return m_D->stopAfterFirst;
}

void AWT::SimpleMesh::FacesInRadius::reset( )
{
   SearchAgent::reset();
   m_D->foundPoints.clear();
}

std::vector<PointIndexWeights>& AWT::SimpleMesh::FacesInRadius::getFaces( )
{
   return m_D->foundPoints;
}