/*
* ################################################################################
* ### MIT License
* ################################################################################
* 
* Copyright (c) 2006-2011 Andy Turner
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/
//#ifndef __TRIANGLESINTERSECTINGPLANESEARCH_H__
//#define __TRIANGLESINTERSECTINGPLANESEARCH_H__
//
//#include "EKDTree/EKDSearcher.h"
//#include "EKDTree/TriangleDataElement.h"
//#include "EKDTree/EKDTreeBranch.h"
//
//#include <climits>
//#include <vector>
//
//namespace AWT
//{
//   namespace KDTree
//   {
//      template <class T>
//      class TrianglesIntersectingPlaneSearch : public EKDSearcher<T,3>
//      {
//         static const unsigned char K = 3;
//
//      public:
//         TrianglesIntersectingPlaneSearch( T* in_TestPoint = 0 );
//
//         virtual T calculateBoxDistanceBounds2( AWT::KDTree::EKDTreeBranch<T,3>* in_Branch );
//         virtual bool shouldCheck( T in_LowerBound );
//
//         virtual bool checkPointOfInterest( AWT::KDTree::KDTreeDataElement<T,3>* in_Element, int in_Index );
//
//         virtual void setTestPlane( T* in_TestPlane );
//         virtual void getTestPlane( T* out_TestPlane ) const;
//
//         virtual T getMaxSearchDistanceSquared( ) const { return 0; }
//
//         virtual std::vector<int> getIntersectedTriangles( ) const;
//
//         virtual int  getNumberOfChecks( ) const;
//         virtual int  getNumberOfBoxChecks( ) const;
//         virtual void reset( );
//
//      protected:
//         virtual void handlePointOfInterest( T* in_PointOfInterest, int in_Index );
//
//         virtual bool checkPointOfInterest( AWT::KDTree::TriangleDataElement<T>* in_Element, T* out_Poi );
//
//         virtual bool checkSideOfPlane( T* in_Vertex ) const;
//
//         std::vector<int> m_IntersectedTriangles;
//
//         T m_TestPlane[K+1];
//
//         int m_NumberOfChecks;
//         int m_NumberOfBoxChecks;
//      };
//   }
//}
//
//namespace AWT { namespace KDTree {
//
//template <class T>
//TrianglesIntersectingPlaneSearch<T>::TrianglesIntersectingPlaneSearch( T* in_TestPlane )
//{
//   if ( K != 3 )
//      AWTEXCEPTIONTHROW("This is only defined for 3-dimensional triangles!");
//
//   m_NumberOfChecks = 0;
//
//   if ( in_TestPlane != 0 )
//      setTestPlane( in_TestPlane );
//}
//
//template <class T>
//void TrianglesIntersectingPlaneSearch<T>::setTestPlane( T* in_TestPlane )
//{
//   m_IntersectedTriangles.clear( );
//
//   for ( int i = 0; i <= K; i++ )
//      m_TestPlane[i] = in_TestPlane[i];
//
//   // Put the plane into normal form - just makes distance calcs meaningful
//   T mag = 0, tmp;
//   for ( int i = 0; i < K; ++i )
//   {
//      mag += m_TestPlane[i]*m_TestPlane[i];
//   }
//
//   if ( mag != 1 )
//   {
//      mag = sqrt( mag );
//
//      for ( int i = 0; i <= K; i++ )
//         m_TestPlane[i] /= mag;
//   }
//}
//
//template <class T>
//void TrianglesIntersectingPlaneSearch<T>::getTestPlane( T* out_TestPlane ) const
//{
//   for ( int i = 0; i < K; i++ )
//      out_TestPlane[i] = m_TestPlane[i];
//}
//
//template <class T>
//std::vector<int> TrianglesIntersectingPlaneSearch<T>::getIntersectedTriangles( ) const
//{
//   return m_IntersectedTriangles;
//}
//
//template <class T>
//bool TrianglesIntersectingPlaneSearch<T>::checkSideOfPlane( T* in_Vertex ) const
//{
//   // Assume the implicit homogeneous one
//   T side = -m_TestPlane[K];
//
//   for ( int d = 0; d < K; ++d )
//      side += in_Vertex[d]*m_TestPlane[d];
//
//   return side >= 0;
//}
//
//template <class T>
//T TrianglesIntersectingPlaneSearch<T>::calculateBoxDistanceBounds2( AWT::KDTree::EKDTreeBranch<T,3>* in_Branch )
//{
//   ++m_NumberOfBoxChecks;
//   T ret = 0;
//   T tmp;
//
//   // Calculate the plane-to-corner distance for each of the vertices
//   const int nvertices = 2 << K;
//
//   // Hold the vertices
//   T vertices[K];
//
//   unsigned int posCount = 0;
//
//   for ( int i = 0; i < nvertices; ++i )
//   {
//      for ( int d = 0; d < K; ++d )
//         vertices[d] = (i & ( 1 << d )) != 0 ? in_Branch->getMinimumBound( d ) : in_Branch->getMaximumBound( d );
//
//      if ( checkSideOfPlane( &vertices[0] ) )
//         ++posCount;
//   }
//
//   if ( posCount != 0 && posCount != nvertices )
//      return 0;
//   else
//      return std::numeric_limits<T>::infinity( );
//}
//
//template <class T>
//bool TrianglesIntersectingPlaneSearch<T>::shouldCheck( T in_LowerBound )
//{
//   return in_LowerBound == 0;
//}
//
//template <class T>
//void TrianglesIntersectingPlaneSearch<T>::handlePointOfInterest(T *in_PointOfInterest, int in_Index )
//{
//   m_IntersectedTriangles.push_back( in_Index );
//}
//
//#include "EKDTree/SimpleVectorOps.h"
//
//
//template <class T>
//bool TrianglesIntersectingPlaneSearch<T>::checkPointOfInterest( TriangleDataElement<T>* in_Element, T* out_NearestPoint )
//{
//   T point[3];
//   unsigned int posCount = 0;
//
//   for ( int v = 0; v < 3; ++v )
//   {
//      for ( int i = 0; i < 3; ++i )
//         point[i] = in_Element->GetCoordinate( v, i );
//
//      if ( checkSideOfPlane( point ) )
//         ++posCount;
//   }
//
//   return ( posCount != 0 && posCount != 3 );
//}
//
//template <class T>
//bool TrianglesIntersectingPlaneSearch<T>::checkPointOfInterest( KDTreeDataElement<T,3>* in_Element, int in_Index )
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
//   std::cerr << "Warning: this geometry type is not recognised." << std::endl;
//   
//   return false;
//}
//
//template <class T>
//int TrianglesIntersectingPlaneSearch<T>::getNumberOfBoxChecks( ) const
//{
//   return m_NumberOfBoxChecks;
//}
//
//template <class T>
//int TrianglesIntersectingPlaneSearch<T>::getNumberOfChecks( ) const
//{
//   return m_NumberOfChecks;
//}
//
//template <class T>
//void TrianglesIntersectingPlaneSearch<T>::reset( )
//{
//   m_NumberOfBoxChecks = 0;
//   m_NumberOfChecks = 0;
//}
//
//} }
//
//#endif // __NEARESTPOINTSSEARCH_H__