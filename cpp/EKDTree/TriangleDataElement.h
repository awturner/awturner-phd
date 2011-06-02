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
//#ifndef __TriangleDataElement_H__
//#define __TriangleDataElement_H__
//
//#include "EKDTree/KDTreeDataElement.h"
//#include "Useful/Exception.h"
//
//#include <ostream>
//
//namespace AWT
//{
//   namespace KDTree
//   {
//      template <class T>
//      class TriangleDataElement : public KDTreeDataElement<T,3>
//      {
//      public:
//         TriangleDataElement( T* p0, T* p1, T* p2 );
//         virtual ~TriangleDataElement( );
//
//         virtual T GetValue( const int in_Axis ) const;
//
//         virtual T GetMinBound( const int in_Axis ) const;
//
//         virtual T GetMaxBound( const int in_Axis ) const;
//
//         virtual T GetCoordinate( const int in_Vertex, const int in_Axis ) const;
//
//         virtual void update( );
//
//         static friend std::ostream& operator<<( std::ostream& os, TriangleDataElement& tri )
//         {
//            os << "[" << std::endl;
//            os << "\t" << tri.m_Offset[0] << " " << tri.m_Offset[1] << " " << tri.m_Offset[2] << std::endl;
//            os << "\t" << (tri.m_Offset[0]+tri.m_Point1[0]) << " " << (tri.m_Offset[1]+tri.m_Point1[1]) << " " << (tri.m_Offset[2]+tri.m_Point1[2]) << std::endl;
//            os << "\t" << (tri.m_Offset[0]+tri.m_Point2[0]) << " " << (tri.m_Offset[1]+tri.m_Point2[1]) << " " << (tri.m_Offset[2]+tri.m_Point2[2]) << std::endl;
//            os << "]";
//            return os;
//         }
//
//      protected:
//         TriangleDataElement( ) { p0 = p1 = p2 = 0; }
//
//         T m_Offset[3];
//         // m_Point0 is implicitly [0 0 0]
//         T m_Point1[3];
//         T m_Point2[3];
//
//         T p0[3], p1[3], p2[3];
//
//         T m_Centroid[3];
//
//         T m_Bounds[6];
//      };
//   }
//}
//
//#include <iostream>
//
//namespace AWT { namespace KDTree {
//
//template <class T>
//TriangleDataElement<T>::TriangleDataElement( T* p0, T* p1, T* p2 )
//{
//   for ( int d = 0; d < 3; ++d )
//   {
//      this->p0[d] = p0[d];
//      this->p1[d] = p1[d];
//      this->p2[d] = p2[d];
//   }
//
//   update( );
//}
//
//template <class T>
//TriangleDataElement<T>::~TriangleDataElement( )
//{
//}
//
//template <class T>
//void TriangleDataElement<T>::update( )
//{
//   m_Bounds[0] = m_Bounds[1] = 
//   m_Bounds[2] = m_Bounds[3] = 
//   m_Bounds[4] = m_Bounds[5] = 0;
//
//   for ( int i = 0; i < 3; i++ )
//   {
//      m_Offset[i] = p0[i];
//      m_Point1[i] = p1[i] - p0[i];
//      m_Point2[i] = p2[i] - p0[i];
//
//      if ( m_Point1[i] < m_Bounds[2*i+0] )
//         m_Bounds[2*i+0] = m_Point1[i];
//
//      if ( m_Point1[i] > m_Bounds[2*i+1] )
//         m_Bounds[2*i+1] = m_Point1[i];
//
//      if ( m_Point2[i] < m_Bounds[2*i+0] )
//         m_Bounds[2*i+0] = m_Point2[i];
//
//      if ( m_Point2[i] > m_Bounds[2*i+1] )
//         m_Bounds[2*i+1] = m_Point2[i];
//
//      m_Centroid[i] = m_Offset[i] + ( m_Point1[i] + m_Point2[i] ) / static_cast<T>( 3 );
//   }
//}
//
//template <class T>
//T TriangleDataElement<T>::GetValue( const int in_Axis ) const
//{
//   return m_Centroid[in_Axis];
//}
//
//template <class T>
//T TriangleDataElement<T>::GetMinBound( const int in_Axis ) const
//{
//   return m_Offset[in_Axis] + m_Bounds[2*in_Axis+0];
//}
//
//template <class T>
//T TriangleDataElement<T>::GetMaxBound( const int in_Axis ) const
//{
//   return m_Offset[in_Axis] + m_Bounds[2*in_Axis+1];
//}
//
//template <class T>
//T TriangleDataElement<T>::GetCoordinate(const int in_Vertex, const int in_Axis) const
//{
//   switch ( in_Vertex )
//   {
//   case 0:
//      return m_Offset[in_Axis];
//      break;
//   case 1:
//      return m_Offset[in_Axis] + m_Point1[in_Axis];
//      break;
//   case 2:
//      return m_Offset[in_Axis] + m_Point2[in_Axis];
//      break;
//   default:
//      AWTEXCEPTIONTHROW("Illegal Vertex!");
//   }
//}
//
//} }
//
//#endif // __TriangleDataElement_H__
