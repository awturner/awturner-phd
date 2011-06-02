//#ifndef __FIRSTRAYINTERSECTIONSEARCH_H__
//#define __FIRSTRAYINTERSECTIONSEARCH_H__
//
//#include "EKDTree/EKDSearcher.h"
//
//#include <climits>
//
//#include "EKDTree/TriangleDataElement.h"
//#include "EKDTree/PointDataElement.h"
//
//#include "Useful/PrintMacros.h"
//
//namespace AWT
//{
//   namespace KDTree
//   {
//      template <class T, unsigned char K>
//      class FirstRayIntersectionSearch : public EKDSearcher<T,K>
//      {
//      public:
//         FirstRayIntersectionSearch( T* in_RayOrigin = 0, T* in_RayDirection = 0, T in_MaxDistance2 = std::numeric_limits<T>::max( ) );
//
//         virtual void setRayOrigin( T* in_RayOrigin );
//         virtual void getRayOrigin( T* out_RayOrigin ) const;
//
//         virtual void setRayDirection( T* in_RayDirection );
//         virtual void getRayDirection( T* out_RayDirection ) const;
//
//         virtual void setMaximumDistance( T in_MaxDistance );
//         virtual T    getMaximumDistance( ) const;
//
//         virtual T    getMaxSearchDistanceSquared( ) const;
//
//         virtual int  getFirstIntersection( T* out_NearestPoint = 0 ) const;
//
//         virtual T calculateBoxDistanceBounds2( EKDTreeBranch<T,K>* in_Branch );
//         virtual bool shouldCheck( T in_LowerBound );
//
//         virtual bool checkPointOfInterest( const KDTreeDataElement<T,K>* in_Element, const int in_Index );
//
//         virtual int  getNumberOfChecks( ) const;
//         virtual int  getNumberOfBoxChecks( ) const;
//         virtual void reset( );
//
//      protected:
//         virtual void handlePointOfInterest( const T* in_PointOfInterest, const int in_Index );
//
//         virtual bool checkPointOfInterest( const TriangleDataElement<T>* in_Element, T* out_PointOfInterest );
//         virtual bool checkPointOfInterest( const PointDataElement<T,K>* in_Element, T* out_PointOfInterest );
//
//         int m_NearestIndex;
//         T   m_NearestPoint[K];
//
//         T m_RayOrigin[K];
//         T m_RayDirection[K];
//
//         T m_InitMaxDistance2;
//         T m_MaxDistance2;
//
//         int m_NumberOfChecks;
//         int m_NumberOfBoxChecks;
//      };
//   }
//}
//
//namespace AWT { namespace KDTree {
//
//template <class T, unsigned char K>
//FirstRayIntersectionSearch<T,K>::FirstRayIntersectionSearch( T* in_RayOrigin, T* in_RayDirection, T in_MaxDistance2 )
//{
//   m_NearestIndex      = -1;
//   m_NumberOfChecks    = 0;
//   m_NumberOfBoxChecks = 0;
//
//   if ( in_RayOrigin != 0 )
//      setRayOrigin( in_RayOrigin );
//
//   if ( in_RayDirection != 0 )
//      setRayDirection( in_RayDirection );
//
//   m_MaxDistance2 = in_MaxDistance2;
//   m_InitMaxDistance2 = m_MaxDistance2;
//}
//
//template <class T, unsigned char K>
//void FirstRayIntersectionSearch<T,K>::setRayOrigin( T* in_RayOrigin )
//{
//   for ( int i = 0; i < K; i++ )
//      m_RayOrigin[i] = in_RayOrigin[i];
//}
//
//template <class T, unsigned char K>
//void FirstRayIntersectionSearch<T,K>::getRayOrigin( T* out_RayOrigin ) const
//{
//   for ( int i = 0; i < K; i++ )
//      out_RayOrigin[i] = m_RayOrigin[i];
//}
//
//template <class T, unsigned char K>
//void FirstRayIntersectionSearch<T,K>::setRayDirection( T* in_RayDirection )
//{
//   for ( int i = 0; i < K; i++ )
//      m_RayDirection[i] = in_RayDirection[i];
//}
//
//template <class T, unsigned char K>
//void FirstRayIntersectionSearch<T,K>::getRayDirection( T* out_RayDirection ) const
//{
//   for ( int i = 0; i < K; i++ )
//      out_RayDirection[i] = m_RayDirection[i];
//}
//
//template <class T, unsigned char K>
//bool FirstRayIntersectionSearch<T,K>::shouldCheck( T in_LowerBound )
//{
//   return in_LowerBound < m_MaxDistance2;
//}
//
//template <class T, unsigned char K>
//void FirstRayIntersectionSearch<T,K>::setMaximumDistance( T in_MaximumDistance )
//{
//   m_MaxDistance2 = in_MaximumDistance*in_MaximumDistance;
//   m_NearestIndex = -1;
//}
//
//template <class T, unsigned char K>
//T FirstRayIntersectionSearch<T,K>::getMaximumDistance( ) const
//{
//   return sqrt( m_MaxDistance2 );
//}
//
//template <class T, unsigned char K>
//T FirstRayIntersectionSearch<T,K>::getMaxSearchDistanceSquared( ) const
//{
//   return m_MaxDistance2;
//}
//
//template <class T, unsigned char K>
//int FirstRayIntersectionSearch<T,K>::getFirstIntersection( T* out_Point ) const
//{
//   if ( out_Point != 0 )
//   {
//      for ( int i = 0; i < K; ++i )
//         out_Point[i] = m_NearestPoint[i];
//   }
//
//   return m_NearestIndex;
//}
//
//template <class T, unsigned char K>
//T FirstRayIntersectionSearch<T,K>::calculateBoxDistanceBounds2( EKDTreeBranch<T,K>* in_Branch )
//{
//   ++m_NumberOfBoxChecks;
//
//   int intersectionsFound = 0;
//   T projected;
//
//   T bounds[2*K];
//
//   T  bestDistances[2];
//
//   T lambda;
//
//   int ax, i, j;
//
//   for ( ax = 0; ax < K; ax++ )
//   {
//      bounds[2*ax+0] = in_Branch->getMinimumBound( ax );
//      bounds[2*ax+1] = in_Branch->getMaximumBound( ax );
//   }
//
//   for ( ax = 0; ax < K; ax++ )
//   {
//      if ( m_RayDirection[ax] == 0 )
//         continue;
//
//      for ( i = 0; i < 2; i++ )
//      {
//         lambda = ( bounds[2*ax+i] - m_RayOrigin[ax] ) / m_RayDirection[ax];
//
//         if ( lambda < 0 )
//            continue;
//
//         bestDistances[intersectionsFound] = 0;
//
//         for ( j = 0; j < K; j++ )
//         {
//            // Project the point onto the plane
//            projected = lambda * m_RayDirection[j];
//            
//            bestDistances[intersectionsFound] += projected*projected;
//
//            // Fast break on distance
//
//            if ( bestDistances[intersectionsFound] > m_MaxDistance2 )
//               break;
//
//            // Don't want to check on the axis in question in case of round off errors
//            if ( j != ax )
//            {
//               projected += m_RayOrigin[j];
//
//               if ( projected < bounds[2*j+0] || projected > bounds[2*j+1] )
//                  break;
//            }
//         }
//
//         // Unless J==K, we broke out of the last loop, so the point isn't in the bounds
//         if ( j == K )
//         {
//            ++intersectionsFound;
//
//            // There can only be two intersections of a line with the hypercuboid
//            if ( intersectionsFound == 2 )
//            {
//               if ( bestDistances[1] < bestDistances[0] )
//                  bestDistances[0] = bestDistances[1];
//
//               i = 2;
//               ax = K;
//            }
//         }
//      }
//   }
//
//   if ( intersectionsFound > 0 )
//      return bestDistances[0];
//   else
//      return std::numeric_limits<T>::infinity( );
//}
//
//template <class T, unsigned char K>
//bool FirstRayIntersectionSearch<T,K>::checkPointOfInterest( const KDTreeDataElement<T,K>* in_Element, const int in_Index )
//{
//   ++m_NumberOfChecks;
//
//   bool ret;
//   T pointOfInterest[K];
//
//   {
//      const TriangleDataElement<T>* ty = dynamic_cast<const TriangleDataElement<T>*>( in_Element );
//      if ( ret = checkPointOfInterest( ty, pointOfInterest ) )
//         handlePointOfInterest( pointOfInterest, in_Index );
//      return ret;
//   }
//
//   {
//      const PointDataElement<T,K>* ty = dynamic_cast<const PointDataElement<T,K>*>( in_Element );
//      if ( ret = checkPointOfInterest( ty, pointOfInterest ) )
//         handlePointOfInterest( pointOfInterest, in_Index );
//      return ret;
//   }
//
//   std::cerr << "Warning: this geometry type is not recognised." << std::endl;
//   
//   return false;
//}
//
//template <class T, unsigned char K>
//void FirstRayIntersectionSearch<T,K>::handlePointOfInterest(const T *in_PointOfInterest, const int in_Index )
//{
//   T dist2 = 0;
//   T tmp;
//
//   for ( int i = 0; i < K; i++ )
//   {
//      tmp = in_PointOfInterest[i] - m_RayOrigin[i];
//      dist2 += tmp*tmp;
//
//      if ( dist2 >= m_MaxDistance2 )
//         return;
//   }
//
//   // If we have got this far, we don't need to
//   // compare to m_MaxDistance2
//
//   m_NearestIndex = in_Index;
//
//   for ( int i = 0; i < K; i++ )
//      m_NearestPoint[i] = in_PointOfInterest[i];
//
//   m_MaxDistance2 = dist2;
//}
//
//#include "EKDTree/SimpleVectorOps.h"
//
//template <class T, unsigned char K>
//bool FirstRayIntersectionSearch<T,K>::checkPointOfInterest( const PointDataElement<T,K>* /*in_Element*/, T* /*out_Poi*/ )
//{
//   return false;
//}
//
//template <class T, unsigned char K>
//bool FirstRayIntersectionSearch<T,K>::checkPointOfInterest( const TriangleDataElement<T>* in_Element, T* out_NearestPoint )
//{
//   if ( K != 3 )
//      AWTEXCEPTIONTHROW("This is only defined for 3-dimensional triangles!");
//
//   T offset[3];
//   T point1[3];
//   T point2[3];
//
//   for ( int i = 0; i < 3; i++ )
//   {
//      offset[i] = in_Element->GetCoordinate( 0, i );
//
//      point1[i] = in_Element->GetCoordinate( 1, i ) - offset[i];
//      point2[i] = in_Element->GetCoordinate( 2, i ) - offset[i];
//   }
//
//   T nml[3];
//   cross( point1, point2, nml );
//
//   // If ||nml|| == 0, then the case is degenerate
//   if ( dot<double,3>( nml, nml ) == 0 )
//   {
//      return false;
//   }
//
//   normalize<double,3>( nml );
//
//   T denom = dot<double,3>( nml, m_RayDirection );
//
//   // denom = 0 => line is perp to triangle normal so doesn't intersect
//   if ( denom == 0 )
//      return false;
//
//   T delta[3];
//   delta[0] = offset[0] - m_RayOrigin[0];
//   delta[1] = offset[1] - m_RayOrigin[1];
//   delta[2] = offset[2] - m_RayOrigin[2];
//
//   const T lambda = dot<double,3>( delta, nml ) / denom;
//   /*if ( lambda < 0 )
//      return false;*/
//
//   T np[3];
//   np[0] = m_RayOrigin[0] + lambda * m_RayDirection[0];
//   np[1] = m_RayOrigin[1] + lambda * m_RayDirection[1];
//   np[2] = m_RayOrigin[2] + lambda * m_RayDirection[2];
//
//   // OK: out_IntersectionPoint now lies in the plane of the triangle.
//   // the question is, does it lie within the boundary of the triangle...
//
//   // Define these to make the code a bit prettier
//   T* vec0 = point1;
//   T* vec1 = point2;
//
//   // This is the determinant of [ vec0 vec1 nml ]
//   T det =   vec0[0] * ( vec1[1]*nml[2] - nml[1]*vec1[2] )
//           - vec0[1] * ( vec1[0]*nml[2] - nml[0]*vec1[2] )
//           + vec0[2] * ( vec1[0]*nml[1] - nml[0]*vec1[1] );
//
//   // This is the adjoint of [ vec0 vec1 nml ]
//   T mat[3][3] = {
//      {  (vec1[1]*nml[2]-nml[1]*vec1[2]),   -(vec1[0]*nml[2]-nml[0]*vec1[2]),    (vec1[0]*nml[1]-nml[0]*vec1[1])   },
//      { -(vec0[1]*nml[2]-nml[1]*vec0[2]),    (vec0[0]*nml[2]-nml[0]*vec0[2]),   -(vec0[0]*nml[1]-nml[0]*vec0[1])   },
//      {  (vec0[1]*vec1[2]-vec1[1]*vec0[2]), -(vec0[0]*vec1[2]-vec1[0]*vec0[2]),  (vec0[0]*vec1[1]-vec1[0]*vec0[1]) }
//   };
//
//   T proj[3];
//   
//   np[0] -= offset[0];
//   np[1] -= offset[1];
//   np[2] -= offset[2];
//
//   // This code works, but to make it more efficient, certain things don't need to be calculated
//   proj[0] = ( mat[0][0]*np[0] + mat[0][1]*np[1] + mat[0][2]*np[2] ); // / det;
//   proj[1] = ( mat[1][0]*np[0] + mat[1][1]*np[1] + mat[1][2]*np[2] ); // / det;
//   //proj[2] = ( mat[2][0]*np[0] + mat[2][1]*np[1] + mat[2][2]*np[2] ) / det;
//
//   if ( proj[0] < 0 || proj[0] > det/*1*/ || proj[1] < 0 || proj[1] > det/*1*/ || (proj[0] + proj[1]) > det/*1*/ )
//      return false;
//
//   out_NearestPoint[0] = m_RayOrigin[0] + lambda * m_RayDirection[0];
//   out_NearestPoint[1] = m_RayOrigin[1] + lambda * m_RayDirection[1];
//   out_NearestPoint[2] = m_RayOrigin[2] + lambda * m_RayDirection[2];
//
//   return true;
//}
//
//template <class T, unsigned char K>
//void FirstRayIntersectionSearch<T,K>::reset( )
//{
//   m_NumberOfBoxChecks = m_NumberOfChecks = 0;
//
//   m_MaxDistance2 = m_InitMaxDistance2;
//}
//
//template <class T, unsigned char K>
//int FirstRayIntersectionSearch<T,K>::getNumberOfChecks( ) const
//{
//   return m_NumberOfChecks;
//}
//
//template <class T, unsigned char K>
//int FirstRayIntersectionSearch<T,K>::getNumberOfBoxChecks( ) const
//{
//   return m_NumberOfBoxChecks;
//}
//
//} }
//
//#endif // __FIRSTRAYINTERSECTIONSEARCH_H__