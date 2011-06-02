//#ifndef __EKDTREEBRANCH_H__
//#define __EKDTREEBRANCH_H__
//
//#include "EKDTree/EKDTree.h"
//#include <vector>
//
//namespace AWT
//{
//   namespace KDTree
//   {
//      typedef unsigned char Axis;
//      typedef unsigned char ChildIndex;
//
//       /*!
//       * @class EKDTreeBranch
//       * @date ?
//       * @todo Dispose of instances after some event?
//       *
//       * @brief Represents a branching of the K-d tree
//       *
//       * This is a convenience class for handling a branching of the K-d tree.  It enables
//       * you to get information such as the extents of the bounding box, as well as giving
//       * access to the child branches.
//       *
//       * You can't instantiate this class - you only need instances briefly, and so this
//       * class maintains an internal store from which instances are "checked out" when
//       * needed, and checked back in when no longer needed.
//       */
//      template <class T, unsigned char K>
//      class EKDTreeBranch
//      {
//         /*! Allow EKDTree to manipulate me
//         */
//         template <class T, unsigned char K>
//         friend class EKDTree;
//
//      private:
//
//         /*! Private constructor.
//         * Not sure why this one exists...
//         */
//         EKDTreeBranch( ) { ++InstanceCount; }
//
//         /*! Private constructor
//         * @todo Work out if parameter ''end'' is inclusive or exclusive
//         * @param tree   KDTree of which this is a branching
//         * @param start  Starting index within the tree's internal data store
//         * @param end    Ending index within the tree's internal data store
//         * @param bounds Array of bounds in order [ min_dim_0 max_dim_0 min_dim_1 max_dim_1...min_dim_(K-1) max_dim_(K-1) ]
//         */
//         EKDTreeBranch( EKDTree<T,K>* tree, const int start, const int end, const Axis axis, const T* bounds );
//
//      public:
//         ~EKDTreeBranch( );
//
//         /*! Initialize this branch
//         * @param tree   KDTree of which this is a branching
//         * @param start  Starting index within the tree's internal data store
//         * @param end    Ending index within the tree's internal data store
//         * @param axis   Axis along which to discriminate
//         * @param bounds Array of bounds in order [ min_dim_0 max_dim_0 min_dim_1 max_dim_1...min_dim_(K-1) max_dim_(K-1) ]
//         */
//         void init( EKDTree<T,K>* tree, const int start, const int end, const Axis axis, const T* bounds );
//
//         /*! Get the minimum extent along the specified axis
//         * @param in_Axis The desired axis
//         * @return Upper bound on x : p[in_Axis] >= x
//         */
//         T getMinimumBound( const Axis in_Axis ) const;
//         
//         /*! Get the maximum extent along the specified axis
//         * @param in_Axis The desired axis
//         * @return Lower bound on x : p[in_Axis] <= x
//         */
//         T getMaximumBound( const Axis in_Axis ) const;
//
//         /*! Get the child branching of this branch
//         * The parameter in_ChildIndex should be 0 (zero) or 1 (one):
//         * <ul>
//         *   <li>1 - the half of the children closer to the minimum bound in the discriminator axis
//         *   <li>1 - the half of the children closer to the minimum bound in the discriminator axis
//         * </ul>
//         * @param in_ChildIndex Child index
//         * @return Desired child branch
//         */
//         EKDTreeBranch<T,K>* getChild( const ChildIndex in_ChildIndex );
//
//         /*! Report that you have finished with the child so that it can be recycled
//         * @param in_ChildIndex The index of the child that you've finished with
//         */
//         void                finishedWithChild( const ChildIndex in_ChildIndex );
//
//         /*! The total number of elements in this branch
//         * @return The total number of elements in this branch
//         */
//         int  getSize( ) const;
//
//         /*! Convert an index relative to this branching to the index in the parent tree
//         * @return Index within the parent tree
//         */
//         int getOriginalIndex( const int in_Index ) const;
//
//         /*! Get the tree data element, indexed relative to this branch
//         * @param in_Index Element index
//         * @return Corresponding data element
//         */
//         KDTreeDataElement<T,K>* getDataElement( const int in_Index ) const;
//         
//         /*! Get the tree data element, indexed relative to the parent tree
//         * This is just a convenience function
//         * @param in_Index Element index
//         * @return Corresponding data element
//         */
//         KDTreeDataElement<T,K>* getOriginalDataElement( const int in_Index ) const;
//
//         /*
//         * @return Difference between parent indices and this tree's indices
//         */
//         int  getOffset( ) const;
//
//      public:
//         /*! The total number of instances in existence
//         */
//         static int InstanceCount;
//
//         /*! Get an instance of the tree branch
//         */
//         static EKDTreeBranch<T,K>* checkOut( );
//
//         /*! Return this branch, you've finished using it
//         * <b>Do not use the branch after you have called this method!</b>
//         * It might have been handed out to somebody else, or destroyed.
//         */
//         static void checkIn( EKDTreeBranch<T,K>* in_Branch );
//
//      protected:
//         /*! Copy one bounds array to another
//         */
//         static void copyBounds( const T* in_Source, T* out_Dest );
//
//         /*! Internal store of branch instances
//         */
//         static std::vector<EKDTreeBranch<T,K>*>    m_BranchStack;
//         
//         /*! Parent tree
//         */
//         EKDTree<T,K>*              m_Tree;
//
//         /*! Start index in parent tree
//         */
//         int                        m_Start;
//         
//         /*! End index in parent tree
//         */
//         int                        m_End;
//         
//         /*! Discriminator axis
//         */
//         unsigned char              m_Axis;
//
//         T                          m_Bounds[2*K];
//
//         EKDTreeBranch<T,K>*        m_Children[2];
//      };
//   }
//}
//
//#include <iostream>
//
//namespace AWT { namespace KDTree {
//
//template <class T, unsigned char K>
//std::vector<EKDTreeBranch<T,K>*> EKDTreeBranch<T,K>::m_BranchStack;
//
//template <class T, unsigned char K>
//EKDTreeBranch<T,K>* EKDTreeBranch<T,K>::checkOut( )
//{
//   if ( m_BranchStack.empty( ) )
//   {
//      EKDTreeBranch<T,K>* ret = new EKDTreeBranch<T,K>( );
//      ret->countMe( );
//      return ret;
//   }
//   else
//   {
//      EKDTreeBranch<T,K>* ret = m_BranchStack.back( );
//      m_BranchStack.pop_back( );
//      return ret;
//   }
// 
//}
//
//template <class T, unsigned char K>
//void EKDTreeBranch<T,K>::checkIn( EKDTreeBranch<T,K>* in_Branch )
//{
//   m_BranchStack.push_back( in_Branch );
//}  
//
//template <class T, unsigned char K>
//int EKDTreeBranch<T,K>::InstanceCount = 0;
//
//template <class T, unsigned char K>
//EKDTreeBranch<T,K>::EKDTreeBranch( EKDTree<T,K>* tree, const int start, const int end, const Axis axis, const T* bounds )
//{
//   ++InstanceCount;
//   init( tree, start, end, axis, bounds );
//}
//
//template <class T, unsigned char K>
//void EKDTreeBranch<T,K>::init( EKDTree<T,K>* tree, const int start, const int end, const Axis axis, const T* bounds )
//{
//   m_Tree  = tree;
//   m_Start = start;
//   m_End   = end;
//   m_Axis  = axis;
//
//   // Set the children to null initially - we will create them lazily
//   m_Children[0] = 0;
//   m_Children[1] = 0;
//
//   // Copy the passed bounds
//   copyBounds( bounds, m_Bounds );
//}
//
//template <class T, unsigned char K>
//EKDTreeBranch<T,K>::~EKDTreeBranch( )
//{
//   finishedWithChild( 0 );
//   finishedWithChild( 1 );
//}
//
//template <class T, unsigned char K>
//void EKDTreeBranch<T,K>::copyBounds(const T *in_Source, T *out_Dest)
//{
//   for ( int i = 0; i < 2*K; i++ )
//      out_Dest[i] = in_Source[i];
//}
//
//template <class T, unsigned char K>
//int EKDTreeBranch<T,K>::getOffset( ) const
//{
//   return m_Start;
//}
//
//template <class T, unsigned char K>
//void EKDTreeBranch<T,K>::finishedWithChild( const ChildIndex in_ChildIdx )
//{
//   if ( m_Children[in_ChildIdx] != 0 )
//   {
//      checkIn( m_Children[in_ChildIdx] );
//      //delete m_Children[in_ChildIdx];
//   }
//
//   m_Children[in_ChildIdx] = 0;
//}
//
//template <class T, unsigned char K>
//EKDTreeBranch<T,K>* EKDTreeBranch<T,K>::getChild( const ChildIndex in_ChildIndex )
//{
//   if ( getSize( ) <= m_Tree->getMinimumSize( ) )
//      return 0;
//
//   // Lazily evaluate the children
//   if ( m_Children[in_ChildIndex] == 0 )
//   {
//      //T childBounds[2*K];
//      //copyBounds( m_Bounds, childBounds );
//
//      int cstart, cend;
//      T tightenedBound;
//      m_Tree->branchCalculateChildBounds( in_ChildIndex, m_Start, m_End, cstart, cend, tightenedBound );
//
//      //childBounds[2*m_Axis + (1-in_ChildIndex)] = tightenedBound;
//      
//      // Create the branch
//      m_Children[in_ChildIndex] = checkOut( );
//      m_Children[in_ChildIndex]->init( m_Tree, cstart, cend, (m_Axis+1)%K, m_Bounds );
//
//      m_Children[in_ChildIndex]->m_Bounds[2*m_Axis + (1-in_ChildIndex)] = tightenedBound;
//   }
//
//   return m_Children[in_ChildIndex];
//}
//
//template <class T, unsigned char K>
//KDTreeDataElement<T,K>* EKDTreeBranch<T,K>::getDataElement(const int in_Index) const
//{
//   return m_Tree->getDataElement( m_Start + in_Index );
//}
//
//template <class T, unsigned char K>
//KDTreeDataElement<T,K>* EKDTreeBranch<T,K>::getOriginalDataElement(const int in_Index) const
//{
//   return m_Tree->getOriginalDataElement( in_Index );
//}
//
//template <class T, unsigned char K>
//int EKDTreeBranch<T,K>::getOriginalIndex( const int in_Index ) const
//{
//   return m_Tree->getOriginalIndex( m_Start + in_Index );
//}
//
//template <class T, unsigned char K>
//T EKDTreeBranch<T,K>::getMinimumBound(const Axis in_Axis) const
//{
//   return m_Bounds[2*in_Axis+0];
//}
//
//template <class T, unsigned char K>
//T EKDTreeBranch<T,K>::getMaximumBound(const Axis in_Axis) const
//{
//   return m_Bounds[2*in_Axis+1];
//}
//
//template <class T, unsigned char K>
//int EKDTreeBranch<T,K>::getSize( ) const
//{
//   int ret = m_End - m_Start + 1;
//   if ( ret < 0 ) ret = 0;
//   return ret;
//}
//
//} }
//
//#endif // __EKDTREEBRANCH_H__
