#include "RaySearch.h"

#include "TreeData.h"
#include "Mesh.h"
#include "MeshFunctions.h"

#include "Useful/ArrayVectorFunctions.h"

struct AWT::SimpleMesh::RaySearch::D
{
   SearchFilter::P filter;

   PointIndexWeights nearestPoint;
   bool found;

   double initMaxDistance2;
   double maxDistance2;

   Point testPoint;
   Point testNormal;
};

AWT::SimpleMesh::RaySearch::P AWT::SimpleMesh::RaySearch::getInstance( const double in_MaxDistance )
{
   AUTOGETINSTANCE( AWT::SimpleMesh::RaySearch, ( in_MaxDistance ) );
}

AWT::SimpleMesh::RaySearch::RaySearch( const double in_MaxDistance )
{
   m_D = new D;

   m_D->initMaxDistance2 = in_MaxDistance*in_MaxDistance;

   reset( );
}

AWT::SimpleMesh::RaySearch::~RaySearch( )
{
   delete m_D;
}

GETNAMEMACRO( AWT::SimpleMesh::RaySearch );

void AWT::SimpleMesh::RaySearch::calculateBoxDistanceBounds2( AWT::OEKDTree::OEKDTreeBranch<double,3>* in_Branch, AWT::OEKDTree::SqDistBounds<double>& rbounds ) const
{
   boxChecked();

   int intersectionsFound = 0;
   double projected;

   double bounds[6];

   double  bestDistances[2];

   double lambda;

   int ax, i, j;

   bool inside = true;

   rbounds.lower = rbounds.upper = FAR_FAR_AWAY;

   for ( ax = 0; ax < 3; ax++ )
   {
      bounds[2*ax+0] = in_Branch->getMinimumBound( ax );
      bounds[2*ax+1] = in_Branch->getMaximumBound( ax );

      if ( m_D->testPoint[ax] < bounds[2*ax+0] || m_D->testPoint[ax] > bounds[2*ax+1] )
         inside = false;
   }

   if ( inside )
      return;

   for ( ax = 0; ax < 3 && intersectionsFound < 2; ax++ )
   {
      if ( m_D->testNormal[ax] == 0 )
         continue;

      for ( i = 0; i < 2 && intersectionsFound < 2; i++ )
      {
         lambda = ( bounds[2*ax+i] - m_D->testPoint[ax] ) / m_D->testNormal[ax];

         bestDistances[intersectionsFound] = lambda*lambda;

         for ( j = 0; j < 3; j++ )
         {
            // Project the point onto the plane
            projected = lambda * m_D->testNormal[j];

            //bestDistances[intersectionsFound] += lambda*lambda;

            // Fast break on distance

            if ( bestDistances[intersectionsFound] > m_D->maxDistance2 )
               break;

            // Don't want to check on the axis in question in case of round off errors
            if ( j != ax )
            {
               projected += m_D->testPoint[j];

               if ( projected < bounds[2*j+0] || projected > bounds[2*j+1] )
                  break;
            }
         }

         // Unless J==K, we broke out of the last loop, so the point isn't in the bounds
         if ( j == 3 )
         {
            ++intersectionsFound;

            // There can only be two intersections of a line with the hypercuboid
            if ( intersectionsFound == 2 )
            {
               if ( bestDistances[1] < bestDistances[0] )
                  bestDistances[0] = bestDistances[1];
            }
         }
      }
   }

   if ( intersectionsFound > 0 )
      rbounds.lower = bestDistances[0];
   else
      rbounds.lower = FAR_FAR_AWAY;

   rbounds.upper = FAR_FAR_AWAY;
}

bool AWT::SimpleMesh::RaySearch::shouldCheck( AWT::OEKDTree::OEKDTreeBranch<double,3>* in_Branch, const AWT::OEKDTree::SqDistBounds<double>& bounds ) const
{
   return bounds.lower < m_D->maxDistance2;
}

#include "Mesh/TriangleProjection.h"

void AWT::SimpleMesh::RaySearch::checkObject( const int in_Index )
{
   objectChecked();

   Point vs[3];
   getFaceVertices( mesh, in_Index, vs );

   // Calculate the face normal
   const Point faceNormal = calculateFaceNormal( mesh, in_Index );

   // Calculate the lambda value - assume no intersection if orthogonal
   const double den = inner_product<double>( m_D->testNormal, faceNormal );
   if ( den == 0 )
      return;

   const double lambda = inner_product<double>( vs[0] - m_D->testPoint, faceNormal ) / den;
   if ( lambda < 0 )
   {
      return;
   }

   // Calculate where this point is on the face and the weights for this
   const Point intPoint = m_D->testPoint + lambda * m_D->testNormal;

   // A quick debugging check - make sure this point is in the same plane as the triangle vertices
   const double planeCheck = inner_product<double>( vs[0], faceNormal ) - inner_product<double>( intPoint, faceNormal );
   if ( abs(planeCheck) > 1e-8 )
   {
      DEBUGLINE;
      PRINTVBL( planeCheck );
      throw "Calculated intersection point is not on plane";
   }

   const ConvexWeights ws = calculateFaceWeights( mesh, in_Index, intPoint );
   
   // If the point is outside the triangle, skip it
   if ( ws[0] < 0 || ws[1] < 0 || ws[0]+ws[1] > 1 )
      return;

   const double norm2 = ( m_D->testPoint - intPoint ).squared_magnitude();
   if ( norm2 < m_D->maxDistance2 && m_D->filter->handle( intPoint, in_Index ) )
   {
      m_D->nearestPoint.p = intPoint;
      m_D->nearestPoint.i = in_Index;
      m_D->nearestPoint.w = ws;

      m_D->maxDistance2 = norm2;
      m_D->found = true;
   }
}

void AWT::SimpleMesh::RaySearch::setTestPoint( const Point in_TestPoint  )
{
   m_D->testPoint = in_TestPoint;
}

AWT::SimpleMesh::Point AWT::SimpleMesh::RaySearch::getTestPoint( ) const
{
   return m_D->testPoint;
}

void AWT::SimpleMesh::RaySearch::setTestNormal( const Point in_TestNormal  )
{
   m_D->testNormal = in_TestNormal;
}

AWT::SimpleMesh::Point AWT::SimpleMesh::RaySearch::getTestNormal( ) const
{
   return m_D->testNormal;
}

bool AWT::SimpleMesh::RaySearch::isPointValid( ) const
{
   return m_D->found;
}

AWT::SimpleMesh::PointIndexWeights AWT::SimpleMesh::RaySearch::getNearestPoint( ) const
{
   if ( !m_D->found )
      throw "No point was found!";

   return m_D->nearestPoint;
}

void AWT::SimpleMesh::RaySearch::reset( )
{
   SearchAgent::reset();

   m_D->maxDistance2 = m_D->initMaxDistance2;
   m_D->found = false;
}

AWT::SimpleMesh::SearchFilter::P AWT::SimpleMesh::RaySearch::getSearchFilter( )
{
   return m_D->filter;
}

void AWT::SimpleMesh::RaySearch::setSearchFilter( SearchFilter::P v )
{
   if ( v != m_D->filter )
   {
      m_D->filter = v;
      modified( );
   }
}
