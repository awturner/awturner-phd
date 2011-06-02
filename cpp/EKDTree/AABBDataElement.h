//#ifndef __TriangleDataElement_H__
//#define __TriangleDataElement_H__
//
//#include "EKDTree/KDTreeDataElement.h"
//
//#include <ostream>
//
//template <class T, unsigned char K>
//class AABBDataElement : public KDTreeDataElement<T,K>
//{
//public:
//   AABBDataElement( T* p, T* size );
//   ~AABBDataElement( );
//
//   virtual T GetValue( int in_Axis );
//
//   virtual T GetMinBound( int in_Axis );
//
//   virtual T GetMaxBound( int in_Axis );
//
//protected:
//   T m_Point[K];
//   T m_Size[K];
//};
//
//template <class T>
//AABBDataElement<T>::AABBDataElement( T* p, T* size )
//{
//   for ( int i = 0; i < K; ++i )
//   {
//      m_Point[i] = p[i];
//      m_Size[i] = size[i] / 2;
//   }
//}
//
//template <class T>
//AABBDataElement<T>::~AABBDataElement( )
//{
//}
//
//template <class T>
//T AABBDataElement<T>::GetValue( int in_Axis )
//{
//   return m_Point[in_Axis];
//}
//
//template <class T>
//T AABBDataElement<T>::GetMinBound( int in_Axis )
//{
//   return m_Point[in_Axis] - m_Size[in_Axis];
//}
//
//template <class T>
//T AABBDataElement<T>::GetMaxBound( int in_Axis )
//{
//   return m_Point[in_Axis] + m_Size[in_Axis];
//}
//
//#endif // __TriangleDataElement_H__
