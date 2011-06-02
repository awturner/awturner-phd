#ifndef __FACESRAYALLINTERSECTIONSSEARCH_H__
#define __FACESRAYALLINTERSECTIONSSEARCH_H__

#include "FacesSearch.h"

#include "MeshSearching/FacesTreeData.h"
#include "MeshSearching/FaceQueries.h"
#include "MeshSearching/PointQueries.h"
#include "Useful/ArrayVectorFunctions.h"

#include <limits>

namespace AWT
{
   template <class T>
   class FacesRayAllIntersectionsSearch : public FacesSearch<T>
   {
      template <class T, unsigned char K = 3>
      friend class OEKDTree::OEKDTree;

      template <class T, unsigned char K = 3>
      friend class OEKDTree::OEKDTreeBranch;

   public:
      typedef ManagedAutoPointer<FacesRayAllIntersectionsSearch<T>> P;

      static P getInstance( );

   protected:
      FacesRayAllIntersectionsSearch( const T in_MaxDistance = std::numeric_limits<T>::infinity( ) );
      virtual ~FacesRayAllIntersectionsSearch( );

   public:
      virtual T calculateBoxDistanceBounds2( typename OEKDTree::OEKDTreeBranch<T,3>::P in_Branch );
      virtual bool shouldCheck( T in_LowerBound2 );

      virtual void checkObject( const typename OEKDTree::OEKDTreeData<T,3>::P in_Data, const int in_Index );

      virtual void setTestPoint( const T* in_TestPoint  );
      virtual void getTestPoint( T* out_TestPoint ) const;

      virtual void setTestNormal( const T* in_TestNormal );
      virtual void getTestNormal( T* out_TestNormal ) const;

      virtual int  getNearestPoint( T* out_NearestPoint = 0 ) const;

      virtual void reset( );

      virtual bool isAllowedBackwards( ) const;
      virtual void setAllowedBackwards( const bool v );

      virtual bool isEnforceNormalCompatibility( ) const;
      virtual void setEnforceNormalCompatibility( const bool v );

      virtual std::string getClassName( ) const;
   protected:

      int m_NearestIndex;
      T   m_NearestPoint[3];

      T m_InitMaxDistance2;
      T m_MaxDistance2;

      T m_TestPoint[3];
      T m_TestNormal[3];

      bool m_AllowedBackwards;
      bool m_EnforceNormalCompatibility;
   };
}

#include "Useful/ArrayVectorFunctions.h"

template <class T>
GETNAMEMACRO( AWT::FacesRayAllIntersectionsSearch<T> );

template <class T>
typename AWT::FacesRayAllIntersectionsSearch<T>::P AWT::FacesRayAllIntersectionsSearch<T>::getInstance( )
{
   AUTOGETINSTANCE( AWT::FacesRayAllIntersectionsSearch<T>, ( ) );
}

template <class T>
AWT::FacesRayAllIntersectionsSearch<T>::FacesRayAllIntersectionsSearch( const T in_MaxDistance )
{
   m_InitMaxDistance2 = in_MaxDistance*in_MaxDistance;

   m_AllowedBackwards = false;
   m_EnforceNormalCompatibility = false;

   reset( );
}

template <class T>
AWT::FacesRayAllIntersectionsSearch<T>::~FacesRayAllIntersectionsSearch( )
{

}

template <class T>
T AWT::FacesRayAllIntersectionsSearch<T>::calculateBoxDistanceBounds2( typename AWT::OEKDTree::OEKDTreeBranch<T,3>::P in_Branch )
{
   boxChecked();

   int intersectionsFound = 0;
   T projected;

   T bounds[6];

   T  bestDistances[2];

   T lambda;

   int ax, i, j;

   bool inside = true;

   for ( ax = 0; ax < 3; ax++ )
   {
      bounds[2*ax+0] = in_Branch->getMinimumBound( ax );
      bounds[2*ax+1] = in_Branch->getMaximumBound( ax );

      if ( m_TestPoint[ax] < bounds[2*ax+0] || m_TestPoint[ax] > bounds[2*ax+1] )
         inside = false;
   }

   if ( inside )
      return 0;

   for ( ax = 0; ax < 3 && intersectionsFound < 2; ax++ )
   {
      if ( m_TestNormal[ax] == 0 )
         continue;

      for ( i = 0; i < 2 && intersectionsFound < 2; i++ )
      {
         lambda = ( bounds[2*ax+i] - m_TestPoint[ax] ) / m_TestNormal[ax];

         bestDistances[intersectionsFound] = lambda*lambda;

         for ( j = 0; j < 3; j++ )
         {
            // Project the point onto the plane
            projected = lambda * m_TestNormal[j];

            //bestDistances[intersectionsFound] += lambda*lambda;

            // Fast break on distance

            if ( bestDistances[intersectionsFound] > m_MaxDistance2 )
               break;

            // Don't want to check on the axis in question in case of round off errors
            if ( j != ax )
            {
               projected += m_TestPoint[j];

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
      return bestDistances[0];
   else
      return std::numeric_limits<T>::infinity( );
}

template <class T>
bool AWT::FacesRayAllIntersectionsSearch<T>::shouldCheck( T in_LowerBound2 )
{
   return in_LowerBound2 < m_MaxDistance2;
}

#include "Mesh/TriangleProjection.h"

template <class T>
void AWT::FacesRayAllIntersectionsSearch<T>::checkObject( const typename AWT::OEKDTree::OEKDTreeData<T,3>::P in_Data, const int in_Index )
{
   objectChecked();
   const FacesTreeData<T>* data = reinterpret_cast<const FacesTreeData<T>*>( in_Data.getDataConst( ) );

   T offset[3];
   T point1[3];
   T point2[3];

   data->getVertex( in_Index, 0, offset );
   data->getVertex( in_Index, 1, point1 );
   data->getVertex( in_Index, 2, point2 );

   T ws[2];
   TriangleProjection<T>::projectOntoTriangle( offset, point1, point2, m_TestPoint, ws );

   if ( ws[0] < 0 || ws[1] < 0 || ws[0]+ws[1] > 1 )
      return;

   T intPoint[3];
   TriangleProjection<T>::projectFromTriangle( offset, point1, point2, ws, intPoint );

   const T norm2 = deltaNormSquared( intPoint, m_TestPoint, 3 );
   if ( norm2 < m_MaxDistance2 )
   {
      m_NearestPoint[0] = intPoint[0];
      m_NearestPoint[1] = intPoint[1];
      m_NearestPoint[2] = intPoint[2];

      m_MaxDistance2 = norm2;
      m_NearestIndex = in_Index;
   }

   return;

   for ( int i = 0; i < 3; i++ )
   {
      point1[i] -= offset[i];
      point2[i] -= offset[i];
   }

   T nml[3];
   cross( point1, point2, nml );

   // If ||nml|| == 0, then the case is degenerate
   if ( dot<T>( nml, nml, 3 ) == 0 )
   {
      return;
   }

   // Check for normal compatibility
   if ( m_EnforceNormalCompatibility && dot<T>( nml, m_TestNormal, 3 ) < 0 )
      return;

   normalize<T>( nml, 3 );

   T denom = dot<T>( nml, m_TestNormal, 3 );

   // denom = 0 => line is perp to triangle normal so doesn't intersect
   if ( denom == 0 )
   {
      return;
   }

   T delta[3];
   delta[0] = offset[0] - m_TestPoint[0];
   delta[1] = offset[1] - m_TestPoint[1];
   delta[2] = offset[2] - m_TestPoint[2];

   const T lambda = dot<T>( delta, nml, 3 ) / denom;
   if ( !m_AllowedBackwards && lambda < 0 )
      return;

   // Can check the distance here, since lambda is constant
   const T dist2 = lambda*lambda;
   if ( dist2 > m_MaxDistance2 )
      return;

   // Now we need to see if the point is inside the triangle

   T np[3];
   np[0] = m_TestPoint[0] + lambda * m_TestNormal[0];
   np[1] = m_TestPoint[1] + lambda * m_TestNormal[1];
   np[2] = m_TestPoint[2] + lambda * m_TestNormal[2];

   // OK: out_IntersectionPoint now lies in the plane of the triangle.
   // the question is, does it lie within the boundary of the triangle...

   // Define these to make the code a bit prettier
   T* vec0 = point1;
   T* vec1 = point2;

   // This is the determinant of [ vec0 vec1 nml ]
   T det =   vec0[0] * ( vec1[1]*nml[2] - nml[1]*vec1[2] )
      - vec0[1] * ( vec1[0]*nml[2] - nml[0]*vec1[2] )
      + vec0[2] * ( vec1[0]*nml[1] - nml[0]*vec1[1] );

   // This is the adjoint of [ vec0 vec1 nml ]
   T mat[3][3] = {
      {  (vec1[1]*nml[2]-nml[1]*vec1[2]),   -(vec1[0]*nml[2]-nml[0]*vec1[2]),    (vec1[0]*nml[1]-nml[0]*vec1[1])   },
      { -(vec0[1]*nml[2]-nml[1]*vec0[2]),    (vec0[0]*nml[2]-nml[0]*vec0[2]),   -(vec0[0]*nml[1]-nml[0]*vec0[1])   },
      {  (vec0[1]*vec1[2]-vec1[1]*vec0[2]), -(vec0[0]*vec1[2]-vec1[0]*vec0[2]),  (vec0[0]*vec1[1]-vec1[0]*vec0[1]) }
   };

   T proj[3];

   np[0] -= offset[0];
   np[1] -= offset[1];
   np[2] -= offset[2];

   // This code works, but to make it more efficient, certain things don't need to be calculated
   proj[0] = ( mat[0][0]*np[0] + mat[0][1]*np[1] + mat[0][2]*np[2] ); // / det;
   proj[1] = ( mat[1][0]*np[0] + mat[1][1]*np[1] + mat[1][2]*np[2] ); // / det;
   //proj[2] = ( mat[2][0]*np[0] + mat[2][1]*np[1] + mat[2][2]*np[2] ) / det;

   // Let's add a threshold
   const T lower = static_cast<T>( -1e-5 );
   const T upper = det - lower;
   if ( proj[0] < lower || proj[0] > upper || proj[1] < lower || proj[1] > upper || (proj[0] + proj[1]) > upper )
   {
      return;
   }

   T nearestPoint[3];

   nearestPoint[0] = m_TestPoint[0] + lambda * m_TestNormal[0];
   nearestPoint[1] = m_TestPoint[1] + lambda * m_TestNormal[1];
   nearestPoint[2] = m_TestPoint[2] + lambda * m_TestNormal[2];

   m_NearestIndex = in_Index;

   for ( int i = 0; i < 3; ++i )
      m_NearestPoint[i] = nearestPoint[i];

   m_MaxDistance2 = dist2;
}

template <class T>
void AWT::FacesRayAllIntersectionsSearch<T>::setTestPoint( const T* in_TestPoint  )
{
   for ( int i = 0; i < 3; i++ )
      m_TestPoint[i] = in_TestPoint[i];
}

template <class T>
void AWT::FacesRayAllIntersectionsSearch<T>::getTestPoint( T* out_TestPoint ) const
{
   for ( int i = 0; i < 3; i++ )
      out_TestPoint[i] = m_TestPoint[i];
}

template <class T>
void AWT::FacesRayAllIntersectionsSearch<T>::setTestNormal( const T* in_TestNormal  )
{
   for ( int i = 0; i < 3; i++ )
      m_TestNormal[i] = in_TestNormal[i];
}

template <class T>
void AWT::FacesRayAllIntersectionsSearch<T>::getTestNormal( T* out_TestNormal ) const
{
   for ( int i = 0; i < 3; i++ )
      out_TestNormal[i] = m_TestNormal[i];
}

template <class T>
int  AWT::FacesRayAllIntersectionsSearch<T>::getNearestPoint( T* out_NearestPoint = 0 ) const
{
   if ( m_NearestIndex != -1 && out_NearestPoint != 0 )
   {
      for ( int i = 0; i < 3; i++ )
         out_NearestPoint[i] = m_NearestPoint[i];
   }

   return m_NearestIndex;
}

template <class T>
void AWT::FacesRayAllIntersectionsSearch<T>::reset( )
{
   FacesSearch<T>::reset();

   m_NearestIndex = -1;
   m_MaxDistance2 = m_InitMaxDistance2;
}

template <class T>
bool AWT::FacesRayAllIntersectionsSearch<T>::isAllowedBackwards( ) const
{
   return m_AllowedBackwards;
}

template <class T>
void AWT::FacesRayAllIntersectionsSearch<T>::setAllowedBackwards( const bool v )
{
   m_AllowedBackwards = v;
}

template <class T>
bool AWT::FacesRayAllIntersectionsSearch<T>::isEnforceNormalCompatibility( ) const
{
   return m_EnforceNormalCompatibility;
}

template <class T>
void AWT::FacesRayAllIntersectionsSearch<T>::setEnforceNormalCompatibility( const bool v )
{
   m_EnforceNormalCompatibility = v;
}

#endif // __FACESRAYALLINTERSECTIONSSEARCH_H__