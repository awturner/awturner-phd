//#ifndef __NEARESTPOINTSSEARCH_H__
//#define __NEARESTPOINTSSEARCH_H__
//
//#include "EKDTree/EKDSearcher.h"
//#include "EKDTree/PointDataElement.h"
//#include "EKDTree/TriangleDataElement.h"
//#include "EKDTree/EKDTreeBranch.h"
//
//#include <climits>
//#include <vector>
//
//template <class T, unsigned char K>
//class NearestVerticesSearch : public EKDSearcher<T,K>
//{
//public:
//   NearestVerticesSearch( T* in_TestPoint = 0, T in_SearchRadius = std::numeric_limits<T>::max( ) );
//
//   virtual T calculateBoxDistanceBounds2( EKDTreeBranch<T,K>* in_Branch );
//   virtual bool shouldCheck( T in_LowerBound );
//
//   virtual bool checkPointOfInterest( KDTreeDataElement<T,K>* in_Element, int in_Index );
//
//   virtual void setTestPoint( T* in_TestPoint  );
//   virtual void getTestPoint( T* out_TestPoint ) const;
//
//   virtual std::vector<int> getNearestPoints( ) const;
//
//   virtual T    getMaxSearchDistanceSquared( ) const;
//
//   virtual void setSearchRadius( T in_SearchRadius );
//   virtual T    getSearchRadius( );
//
//   virtual int  getNumberOfChecks( ) const;
//   virtual int  getNumberOfBoxChecks( ) const;
//   virtual void reset( );
//
//protected:
//   virtual void handlePointOfInterest( T* in_PointOfInterest, int in_Index );
//
//   virtual bool checkPointOfInterest( PointDataElement<T,K>* in_Element, T* out_Poi );
//   virtual bool checkPointOfInterest( TriangleDataElement<T>* in_Element, T* out_Poi );
//
//   std::vector<int> m_NearestPoints;
//
//   T m_SearchRadius2;
//   T m_TestPoint[K];
//
//   int m_NumberOfChecks;
//
//   int m_NumberOfBoxChecks;
//};
//
//template <class T, unsigned char K>
//NearestVerticesSearch<T,K>::NearestVerticesSearch( T* in_TestPoint, T in_MaxDist )
//{
//   m_NumberOfChecks = 0;
//   m_SearchRadius2 = std::numeric_limits<T>::max( );
//
//   if ( in_TestPoint != 0 )
//      setTestPoint( in_TestPoint );
//
//   setSearchRadius( in_MaxDist );
//}
//
//template <class T, unsigned char K>
//void NearestVerticesSearch<T,K>::setTestPoint( T* in_TestPoint )
//{
//   m_NearestPoints.clear( );
//
//   for ( int i = 0; i < K; i++ )
//      m_TestPoint[i] = in_TestPoint[i];
//}
//
//template <class T, unsigned char K>
//void NearestVerticesSearch<T,K>::getTestPoint( T* out_TestPoint ) const
//{
//   for ( int i = 0; i < K; i++ )
//      out_TestPoint[i] = m_TestPoint[i];
//}
//
//template <class T, unsigned char K>
//std::vector<int> NearestVerticesSearch<T,K>::getNearestPoints( ) const
//{
//   return m_NearestPoints;
//}
//
//template <class T, unsigned char K>
//void NearestVerticesSearch<T,K>::setSearchRadius( T in_SearchRadius )
//{
//   if ( in_SearchRadius > sqrt( std::numeric_limits<T>::max( ) ) )
//      m_SearchRadius2 = std::numeric_limits<T>::max( );
//   else
//      m_SearchRadius2 = in_SearchRadius * in_SearchRadius;
//}
//
//template <class T, unsigned char K>
//T NearestVerticesSearch<T,K>::getSearchRadius( )
//{
//   return sqrt( m_SearchRadius2 );
//}
//
//template <class T, unsigned char K>
//T NearestVerticesSearch<T,K>::getMaxSearchDistanceSquared( ) const
//{
//   return m_SearchRadius2;
//}
//
//template <class T, unsigned char K>
//T NearestVerticesSearch<T,K>::calculateBoxDistanceBounds2( EKDTreeBranch<T,K>* in_Branch )
//{
//   ++m_NumberOfBoxChecks;
//   T ret = 0;
//   T tmp;
//
//   for ( int i = 0; i < K; i++ )
//   {
//      tmp = std::min( std::max( m_TestPoint[i], in_Branch->getMinimumBound( i ) ), in_Branch->getMaximumBound( i ) );
//
//      tmp -= m_TestPoint[i];
//
//      ret += tmp*tmp;
//
//      if ( ret > m_SearchRadius2 )
//         break;
//   }
//
//   return ret;
//}
//
//template <class T, unsigned char K>
//bool NearestVerticesSearch<T,K>::shouldCheck( T in_LowerBound )
//{
//   return in_LowerBound < m_SearchRadius2;
//}
//
//template <class T, unsigned char K>
//void NearestVerticesSearch<T,K>::handlePointOfInterest(T *in_PointOfInterest, int in_Index )
//{
//   T dist2 = 0;
//   T tmp;
//
//   for ( int i = 0; i < K; i++ )
//   {
//      tmp = in_PointOfInterest[i] - m_TestPoint[i];
//      dist2 += tmp*tmp;
//
//      if ( dist2 >= m_SearchRadius2 )
//         return;
//   }
//
//   m_NearestPoints.push_back( in_Index );
//}
//
//#include "EKDTree/SimpleVectorOps.h"
//
//template <class T, unsigned char K>
//bool NearestVerticesSearch<T,K>::checkPointOfInterest( PointDataElement<T,K>* in_Element, T* out_Poi )
//{
//   for ( int i = 0; i < K; i++ )
//      out_Poi[i] = in_Element->GetValue( i );
//
//   return true;
//}
//
//template <class T, unsigned char K>
//bool NearestVerticesSearch<T,K>::checkPointOfInterest( TriangleDataElement<T>* in_Element, T* out_NearestPoint )
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
//   T p[3] = {
//      m_TestPoint[0]-offset[0], 
//      m_TestPoint[1]-offset[1],
//      m_TestPoint[2]-offset[2],
//   };
//
//   T nml[3];
//   cross( &point1[0], &point2[0], &nml[0] );
//   normalize( nml, 3 );
//
//   T nml_dot_p = dot( &nml[0], &p[0], 3 );
//   T np[3] = {
//      p[0] - nml_dot_p * nml[0],
//      p[1] - nml_dot_p * nml[1],
//      p[2] - nml_dot_p * nml[2]
//   };
//
//   T vec0[3] = { point1[0], point1[1], point1[2] };
//   T vec1[3] = { point2[0], point2[1], point2[2] };
//   T vec2[3] = { point2[0]-point1[0], point2[1]-point1[1], point2[2]-point1[2] };
//
//   T nsqvec[3] = {
//      dot( &vec0[0], &vec0[0], 3 ),
//      dot( &vec1[0], &vec1[0], 3 ),
//      dot( &vec2[0], &vec2[0], 3 )
//   };
//
//   T lams[3] = {
//      dot( &vec0[0], &np[0], 3 ),
//      dot( &vec1[0], &np[0], 3 ),
//      dot( &vec2[0], &np[0], 3 ) - dot( &vec2[0], &vec0[0], 3 )
//   };
//
//   //int resultCase;
//
//   if ( lams[0] < 0 && lams[1] < 0 )
//   {
//      out_NearestPoint[0] = 0;
//      out_NearestPoint[1] = 0;
//      out_NearestPoint[2] = 0;
//      //resultCase = 1;
//   }
//   else if ( lams[0] > nsqvec[0] && lams[2] < 0 )
//   {
//      out_NearestPoint[0] = point1[0];
//      out_NearestPoint[1] = point1[1];
//      out_NearestPoint[2] = point1[2];
//      //resultCase = 2;
//   }
//   else if ( lams[1] > nsqvec[1] && lams[2] > nsqvec[2] )
//   {
//      out_NearestPoint[0] = point2[0];
//      out_NearestPoint[1] = point2[1];
//      out_NearestPoint[2] = point2[2];
//      //resultCase = 3;
//   }
//   else
//   {
//      // This is the determinant of [ vec0 vec1 nml ]
//      T det =   vec0[0] * ( vec1[1]*nml[2] - nml[1]*vec1[2] )
//              - vec0[1] * ( vec1[0]*nml[2] - nml[0]*vec1[2] )
//              + vec0[2] * ( vec1[0]*nml[1] - nml[0]*vec1[1] );
//
//      // This is the adjoint of [ vec0 vec1 nml ]
//      T mat[3][3] = {
//         {  (vec1[1]*nml[2]-nml[1]*vec1[2]),   -(vec1[0]*nml[2]-nml[0]*vec1[2]),    (vec1[0]*nml[1]-nml[0]*vec1[1])   },
//         { -(vec0[1]*nml[2]-nml[1]*vec0[2]),    (vec0[0]*nml[2]-nml[0]*vec0[2]),   -(vec0[0]*nml[1]-nml[0]*vec0[1])   },
//         {  (vec0[1]*vec1[2]-vec1[1]*vec0[2]), -(vec0[0]*vec1[2]-vec1[0]*vec0[2]),  (vec0[0]*vec1[1]-vec1[0]*vec0[1]) }
//      };
//
//      T proj[3] = {
//         ( mat[0][0]*np[0] + mat[0][1]*np[1] + mat[0][2]*np[2] ) / det,
//         ( mat[1][0]*np[0] + mat[1][1]*np[1] + mat[1][2]*np[2] ) / det,
//         ( mat[2][0]*np[0] + mat[2][1]*np[1] + mat[2][2]*np[2] ) / det,
//      };
//
//      if ( lams[0] >= 0 && lams[1] <= nsqvec[0] && proj[1] < 0 )
//      {
//         T mult = lams[0] / nsqvec[0];
//
//         out_NearestPoint[0] = 0 + mult * vec0[0];
//         out_NearestPoint[1] = 0 + mult * vec0[1];
//         out_NearestPoint[2] = 0 + mult * vec0[2];
//         //resultCase = 4;
//      }
//      else if ( lams[1] >= 0 && lams[1] <= nsqvec[1] && proj[0] < 0 )
//      {
//         T mult = lams[1] / nsqvec[1];
//
//         out_NearestPoint[0] = 0 + mult * vec1[0];
//         out_NearestPoint[1] = 0 + mult * vec1[1];
//         out_NearestPoint[2] = 0 + mult * vec1[2];
//         //resultCase = 5;
//      }
//      else if ( proj[0]+proj[1] > 1 )
//      {
//         T mult = lams[2] / nsqvec[2];
//
//         out_NearestPoint[0] = point1[0] + mult * vec2[0];
//         out_NearestPoint[1] = point1[1] + mult * vec2[1];
//         out_NearestPoint[2] = point1[2] + mult * vec2[2];
//         //resultCase = 6;
//      }
//      else
//      {
//         out_NearestPoint[0] = np[0];
//         out_NearestPoint[1] = np[1];
//         out_NearestPoint[2] = np[2];
//         //resultCase = 0;
//      }
//   }
//
//   p[0] -= out_NearestPoint[0];
//   p[1] -= out_NearestPoint[1];
//   p[2] -= out_NearestPoint[2];
//
//   out_NearestPoint[0] += offset[0];
//   out_NearestPoint[1] += offset[1];
//   out_NearestPoint[2] += offset[2];
//
//   return true;
//}
//
//template <class T, unsigned char K>
//bool NearestVerticesSearch<T,K>::checkPointOfInterest( KDTreeDataElement<T,K>* in_Element, int in_Index )
//{
//   ++m_NumberOfChecks;
//
//   bool ret;
//   T    out_Poi[K];
//
//   TriangleDataElement<T>* tri = dynamic_cast<TriangleDataElement<T>*>( in_Element );
//   if ( tri != 0 )
//   {
//      if ( ret = checkPointOfInterest( tri, out_Poi ) )
//         handlePointOfInterest( out_Poi, in_Index );
//
//      return ret;
//   }
//
//   PointDataElement<T,K>* pde = dynamic_cast<PointDataElement<T,K>*>( in_Element );
//   if ( pde != 0 )
//   {
//      if ( ret = checkPointOfInterest( pde, out_Poi ) )
//         handlePointOfInterest( out_Poi, in_Index );
//
//      return ret;
//   }
//
//   std::cerr << "Warning: this geometry type is not recognised." << std::endl;
//   
//   return false;
//}
//
//template <class T, unsigned char K>
//int NearestVerticesSearch<T,K>::getNumberOfBoxChecks( ) const
//{
//   return m_NumberOfBoxChecks;
//}
//
//template <class T, unsigned char K>
//int NearestVerticesSearch<T,K>::getNumberOfChecks( ) const
//{
//   return m_NumberOfChecks;
//}
//
//template <class T, unsigned char K>
//void NearestVerticesSearch<T,K>::reset( )
//{
//   m_NumberOfBoxChecks = 0;
//   m_NumberOfChecks = 0;
//}
//
//#endif // __NEARESTPOINTSSEARCH_H__