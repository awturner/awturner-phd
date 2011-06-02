//template <class T, unsigned char K>
//class KDTreeBranch;
//
//#include "GeneralKDTree/GeneralKDTree.h"
//#include <limits>
//
//template <class T, unsigned char K>
//class KDTreeBranch
//{
//public:
//   KDTreeBranch( GeneralKDTree<T,K>* tree, int start, int end, int discriminatorAxis, T discriminatorValue, KDTreeBranch<T,K>* left = 0, KDTreeBranch<T,K>* right = 0 );
//   ~KDTreeBranch( );
//
//   T getMinimumBound( const unsigned char in_Axis ) const;
//   T getMaximumBound( const unsigned char in_Axis ) const;
//
//   KDTreeBranch* getLeftChild( ) const;
//   KDTreeBranch* getRightChild( ) const;
//
//   int getSize( ) const;
//
//   T getSignedDistance( const int in_Plane, T* in_Point ) const;
//   void getDiscriminatorPlane( const int in_Plane, T* out_Plane ) const;
//
//   void getPoint( const int in_Index, T* out_Point ) const;
//
//protected:
//   // This is a pointer to the underlying data array
//   GeneralKDTree<T,K>* m_Tree;
//
//   int m_Start;
//   int m_End;
//   int m_Axis;
//   T   m_Disc;
//
//   KDTreeBranch<T,K>* m_Left;
//   KDTreeBranch<T,K>* m_Right;
//};
//
//template <class T, unsigned char K>
//KDTreeBranch<T,K>::KDTreeBranch( GeneralKDTree<T,K>* tree, int start, int end, int discAxis, T disc, KDTreeBranch<T,K>* left, KDTreeBranch<T,K>* right ) : m_Left( left ), m_Right( right )
//{
//   m_Tree  = tree;
//   m_Start = start;
//   m_End   = end;
//   m_Axis  = discAxis;
//   m_Disc  = disc;
//}
//
//template <class T, unsigned char K>
//KDTreeBranch<T,K>::~KDTreeBranch( )
//{
//   if ( m_Left != 0 )
//      delete m_Left;
//
//   if ( m_Right != 0 )
//      delete m_Right;
//}
//
//template <class T, unsigned char K>
//KDTreeBranch<T,K>* KDTreeBranch<T,K>::getLeftChild( ) const
//{
//   return m_Left;
//}
//
//template <class T, unsigned char K>
//KDTreeBranch<T,K>* KDTreeBranch<T,K>::getRightChild( ) const
//{
//   return m_Right;
//}
//
//template <class T, unsigned char K>
//void KDTreeBranch<T,K>::getPoint(const int in_Point, T* out_Point) const
//{
//   int idx = m_Tree->m_Indexes[in_Point+m_Start];
//   T* pnt = m_Tree->m_Points[idx];
//
//   for ( int i = 0; i < K; i++ )
//      out_Point[i] = pnt[i];
//}
//
//template <class T, unsigned char K>
//T KDTreeBranch<T,K>::getMinimumBound(const unsigned char in_Axis) const
//{
//   T point[K];
//   T ret = std::numeric_limits<T>::infinity( );
//
//   for ( int i = 0, imax = getSize( ); i < imax; i++ )
//   {
//      getPoint( i, point );
//      if ( point[in_Axis] < ret )
//         ret = point[in_Axis];
//   }
//
//   return ret;
//}
//
//template <class T, unsigned char K>
//T KDTreeBranch<T,K>::getMaximumBound(const unsigned char in_Axis) const
//{
//   T point[K];
//   T ret = -std::numeric_limits<T>::infinity( );
//
//   for ( int i = 0, imax = getSize( ); i < imax; i++ )
//   {
//      getPoint( i, point );
//      if ( point[in_Axis] > ret )
//         ret = point[in_Axis];
//   }
//
//   return ret;
//}
//
//template <class T, unsigned char K>
//int KDTreeBranch<T,K>::getSize( ) const
//{
//   return m_End - m_Start + 1;
//}
//
//template <class T, unsigned char K>
//void KDTreeBranch<T,K>::getDiscriminatorPlane( const int /*in_Plane*/, T* out_Plane ) const
//{
//   for ( int i = 0; i < K; i++ )
//      out_Plane[i] = 0.0;
//
//   out_Plane[m_Axis] = -1.0;
//   out_Plane[K]      = m_Disc;
//}
//
//template <class T, unsigned char K>
//T KDTreeBranch<T,K>::getSignedDistance( const int in_Plane, T* in_Point ) const
//{
//   T* plane = new T[K+1];
//   getDiscriminatorPlane( in_Plane, plane );
//
//   T norm = 0;
//   for ( int i = 0; i < K; i++ )
//      norm += plane[i]*plane[i];
//
//   norm = static_cast<T>( 1.0 / sqrt( static_cast<double>( norm ) ) );
//
//   T ret = 0;
//
//   for ( int i = 0; i <= K; i++ )
//      ret += plane[i] * in_Point[i] * norm;
//   
//   delete plane;
//
//   return ret;
//}