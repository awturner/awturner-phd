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
///***********************************************************************************************
// * File: $RCSfile: EKDTree.h,v $
// * Module path: $Source: /CodePrototypes/EKDTree/include/EKDTree/EKDTree.h,v $
// * Revision: $Revision: 177 $
// * Date: $Date: 2010-01-19 07:59:25 +0000 (Tue, 19 Jan 2010) $
// * Author: $Author: aturner $
// *
// * Created By: Andy Turner 16/05/2007
// *
// * This software is distributed WITHOUT ANY WARRANTY; without even
// * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
// * PURPOSE.
// *
// *----------------------------------------------------------------------------------------------
// *
// * REVISION HISTORY:
// * $Log: EKDTree.h,v $
// * Revision 1.3  2007/06/01 11:31:04  Andy
// * Lots of updates.  Not committed for a long time, so can't remember.  Oops.
// *
// * Revision 1.2  2007/05/16 16:04:21  Andy
// * Added FindNearestPoints method where you pass in the container for the results
// *
// * Revision 1.1  2007/05/16 14:33:44  Andy
// * Initial Checkin
// *
// *
// *
// ***********************************************************************************************/
//
//#ifndef __EKDTree_H__
//#define __EKDTree_H__
//
//
//#include <limits>
//#include <vector>
//#include "Useful/Exception.h"
//
//namespace AWT
//{
//   namespace KDTree
//   {
//      template <class T, unsigned char K>
//      class KDTreeDataElement;
//
//      template <class T, unsigned char K>
//      class EKDSearcher;
//
//      /*!
//       * @class EKDTree
//       * @date 16/05/2007
//       *
//       * @brief Implementation of K-d tree data structure for efficient spatial searching.
//       *
//       * This is the header for the class KDTree, which implements
//       * an optimised k-d tree based on the paper "An Algorithm for
//       * Finding Best Matches in Logarithmic Expected Time,"
//       * Jerome H. Friedman, ACM Transactions on Mathematical
//       * Software, Vol. 3, NO. 3, September 1977, Pages 209-226.
//       *
//       * It is templated on the type of data it is storing (T), and on the number of
//       * spatial dimensions (K).  It reuses some code from the existing implementation
//       * by FMRYB, but it doesn't use a separate node class.  Because of the templating,
//       * all methods are specified in this file.
//       */
//
//      template <class T, unsigned char K>
//      class EKDTree
//      {
//         template <class T, unsigned char K>
//         friend class EKDTreeBranch;
//
//      //! @name Construction
//      //@{
//      public:
//         /*! Class constructor used to build the tree.
//         *
//         * @param *in_data A pointer to an array of data elements
//         * @param in_npoints Size of the array, i.e. the number of points.
//         * @param in_MinSize Minimum size of subtree over which to use tree-based searching - below this level, search exhaustively (more efficient for small trees)
//         *
//         */
//         EKDTree(KDTreeDataElement<T,K>** in_data, const int in_npoints, const int in_MinSize = 10);
//         
//         //! Destructor
//         ~EKDTree();
//      //@}
//
//      //! @name Queries
//      //@{
//      public:
//         /*!
//         * Returns the number of spatial dimensions
//         *
//         * @return Number of spatial dims.
//         */
//         const int getDimensionality();
//      //@}
//
//      ////////////////////////////////////////////////////////////////////////////////
//      // Member functions
//      public:
//         enum SearchType { EXHAUSTIVE, RECURSIVE };
//
//         /*!
//         * Starts the process of building the tree.  Called automatically upon construction, 
//         * but could be called again e.g. if the data changes
//         */
//         void BuildTree();
//
//         EKDTreeBranch<T,K>* getRootBranch();
//
//         int getOriginalIndex(const int in_Index) const;
//
//         KDTreeDataElement<T,K>* getDataElement(const int in_Index) const;
//
//         KDTreeDataElement<T,K>* getOriginalDataElement(const int in_Index) const;
//
//         const int getMinimumSize();
//
//         void search(EKDSearcher<T,K>* in_Search, SearchType in_SearchType = RECURSIVE);
//
//         void search(EKDSearcher<T,K>* in_Search, int& searchCount, SearchType in_SearchType = RECURSIVE);
//
//      protected:
//         void searchRecursive (EKDSearcher<T,K>* in_Search, EKDTreeBranch<T,K>* in_Branch, int& branchesSearched);
//
//         void searchExhaustive(EKDSearcher<T,K>* in_Search, EKDTreeBranch<T,K>* in_Branch);
//
//         /*!
//         * Handles the recursive calls to partition the array into [ {<= median} median {>= median} ]
//         *
//         * @param in_Low First index in the array to check
//         * @param in_High Last index in the array to check
//         * @param in_Dim Current discriminator dimension
//         * @param out_Bounds Bounding box of the current points [dim1_min dim1_max dim2_min dim2_max ... ]
//         */
//         void recursivelyPartition(int in_Low, int in_High, int in_Dim, T* out_Bounds);
//
//	      /*!
//         * Does the meat of partitioning the array into [ {<= median} median {>= median} ]
//         *
//         * @param in_Low First index in the array to check
//         * @param in_High Last index in the array to check
//         * @param in_Dim Current discriminator dimension
//         */
//         void partitionArray(int in_Low, int in_High, int in_Dim);
//
//         void branchCalculateChildBounds(int in_ChildIndex, int in_Low, int in_High, int& out_Low, int& out_High, T& out_Bound);
//
//         bool checkInvariant(int in_Low, int in_High, int in_Dim);
//
//      ////////////////////////////////////////////////////////////////////////////////
//      // Member variables
//
//         //! Number of points to be searched over
//         const int  m_NPoints;
//
//         //! Minimum size of tree (below which just do exhaustive searching)
//         const int  m_MinSize;
//
//         //! Pointer to the data array
//         KDTreeDataElement<T,K>**  m_Points;
//         //std::vector<KDTreeDataElement<T,K>*> m_Points;
//
//         //! Pointer to the index array (this gets rearranged, rather than the data array)
//         int* m_Indexes;
//
//         //! Tree of orthogonal distances to cutting planes
//         T* m_OrthoPositions[2];
//
//         //! Overall bounds of the tree
//         T m_Bounds[2*K];
//
//         //! Root branch of the tree (used in searching)
//         EKDTreeBranch<T,K>* m_Root;
//      };
//   }
//}
//
//#include <iostream>
//#include "EKDTree/EKDTreeBranch.h"
//#include "EKDTree/EKDSearcher.h"
//#include "../Useful/PrintMacros.h"
//
//
//namespace AWT { namespace KDTree {
//
//// These definitions are used in several places throughout; it is convenient to have them
//// as a define to avoid having to hunt and change
//// This has been modified to make this into a K-d trie (i.e. points only stored at leaves, 
//// not at branches)
//#define INITPARTITIONS(low,high) \
//   const int imedian = low + (high-low)/2; \
//   const int lstart = low; \
//   const int lend   = imedian; \
//   const int rstart = imedian + 1; \
//   const int rend   = high;
//
//template <class T, unsigned char K>
//void EKDTree<T,K>::branchCalculateChildBounds(int in_ChildIndex, int in_Low, int in_High, int& out_Low, int& out_High, T& out_Bound)
//{
//   INITPARTITIONS(in_Low, in_High);
//   
//   switch (in_ChildIndex)
//   {
//   case 0:
//      out_Low  = lstart;
//      out_High = lend;
//      break;
//   case 1:
//      out_Low  = rstart;
//      out_High = rend;
//      break;
//   default:
//      AWTEXCEPTIONTHROW("Invalid Child Index!");
//   }
//
//   out_Bound = m_OrthoPositions[in_ChildIndex][imedian];
//}
//
//template <class T, unsigned char K>
//EKDTreeBranch<T,K>* EKDTree<T,K>::getRootBranch()
//{
//   if (m_Root == 0)
//      m_Root = new EKDTreeBranch<T,K>(this, 0, m_NPoints-1, 0, m_Bounds);
//   else
//      m_Root->init(this, 0, m_NPoints-1, 0, m_Bounds);
//
//   return m_Root;
//}
//
//template <class T, unsigned char K>
//const int EKDTree<T,K>::getMinimumSize()
//{
//   return m_MinSize;
//}
//
//// Constructor
//template <class T, unsigned char K>
//EKDTree<T,K>::EKDTree(KDTreeDataElement<T,K>** in_data, const int in_npoints, const int in_MinSize)
//   : m_MinSize(in_MinSize), m_NPoints(in_npoints)
//{
//   if (in_MinSize < 1)
//      AWTEXCEPTIONTHROW("in_MinSize should be >= 1");
//
//   this->m_Root              = 0;
//
//   this->m_Points            = in_data;
//   this->m_Indexes           = new int[m_NPoints];
//
//   this->m_OrthoPositions[0] = new T[m_NPoints];
//   this->m_OrthoPositions[1] = new T[m_NPoints];
//
//   BuildTree();
//}
//
//// Destructor
//template <class T, unsigned char K>
//EKDTree<T,K>::~EKDTree()
//{
//   if (m_Root != 0)
//      delete m_Root;
//
//   delete m_Indexes;
//   delete m_OrthoPositions[0];
//   delete m_OrthoPositions[1];
//}
//
//// Queries
//template <class T, unsigned char K>
//const int EKDTree<T,K>::getDimensionality()
//{
//   return K;
//}
//
//template <class T, unsigned char K>
//int EKDTree<T,K>::getOriginalIndex(const int in_Index) const
//{
//   return m_Indexes[in_Index];
//}
//
//template <class T, unsigned char K>
//KDTreeDataElement<T,K>* EKDTree<T,K>::getDataElement(const int in_Index) const
//{
//   return getOriginalDataElement(getOriginalIndex(in_Index));
//}
//
//template <class T, unsigned char K>
//KDTreeDataElement<T,K>* EKDTree<T,K>::getOriginalDataElement(const int in_Index) const
//{
//   return m_Points[ in_Index ];
//}
//
//template <class T, unsigned char K>
//void EKDTree<T,K>::BuildTree()
//{
//   // Fill the array of indexes - these will be [0..in_npoints-1] to start with,
//   // but they'll be rearranged when the tree is built
//   for (int i = 0; i < m_NPoints; i++)
//   {
//      // Make sure that the point is up-to-date
//      m_Points[i]->update();
//      this->m_Indexes[i] = i;
//   }
//
//	recursivelyPartition(0, m_NPoints-1, 0, m_Bounds);
//}
//
//template <class T, unsigned char K>
//void EKDTree<T,K>::recursivelyPartition(int low, int high, int dim, T* out_Bounds)
//{
//   //std::cerr << low << "\t" << high << std::endl;
//
//	if (high - low < m_MinSize)
//	{
//      int d, i;
//      
//      for (d = 0; d < K; d++)
//      {
//         out_Bounds[2*d + 0] =  std::numeric_limits<T>::max();
//         out_Bounds[2*d + 1] = -out_Bounds[2*d + 0];
//      }
//
//      for (i = low; i <= high; i++)
//      {
//         KDTreeDataElement<T,K>* del = m_Points[m_Indexes[i]];
//
//         for (d = 0; d < K; d++)
//         {
//            out_Bounds[2*d + 0] = std::min(out_Bounds[2*d + 0], del->GetMinBound(d));
//            out_Bounds[2*d + 1] = std::max(out_Bounds[2*d + 1], del->GetMaxBound(d));
//         }
//      }
//
//		return;
//	}
//
//   // Do the partitioning of the current array
//   partitionArray(low, high, dim);
//
//   INITPARTITIONS(low,high);
//
//   // Create arrays to record the bounds of the left and right sub-trees
//   T boundsL[6];
//   T boundsR[6];
//
//   // And now make the recursive calls to do the leq and geq subtrees
//	int dimm = (dim+1) % K;
//	recursivelyPartition(lstart, lend, dimm, boundsL);
//	recursivelyPartition(rstart, rend, dimm, boundsR);
//
//   m_OrthoPositions[0][imedian] = boundsL[2*dim + 1];
//   m_OrthoPositions[1][imedian] = boundsR[2*dim + 0];
//
//   // Work out the bounding box which surrounds the discriminator point and the two sub trees
//   for (int d = 0; d < K; d++)
//   {
//      // Take the minimum of the minimum bounds
//      out_Bounds[2*d + 0] = std::min(m_Points[m_Indexes[imedian]]->GetMinBound(d), std::min(boundsL[2*d + 0], boundsR[2*d + 0]));
//
//      // and the maximum of the maximum bounds
//      out_Bounds[2*d + 1] = std::max(m_Points[m_Indexes[imedian]]->GetMaxBound(d), std::max(boundsL[2*d + 1], boundsR[2*d + 1]));
//   }
//
//   /*
//   if (!checkInvariant(low, high, dim))
//   {
//      std::cerr << "Invariant Error at (low=" << low << ", high=" << high << ", dim=" << dim << ")" << std::endl;
//   }
//   */
//}
//
//template <class T, unsigned char K>
//bool EKDTree<T,K>::checkInvariant(int in_Low, int in_High, int in_Dim)
//{
//   INITPARTITIONS(in_Low, in_High);
//
//   for (int i = lstart; i <= lend; i++)
//   {
//      if (m_Points[m_Indexes[i]]->GetValue(in_Dim) > m_OrthoPositions[0][imedian])
//         return false;
//   }
//
//   for (int i = rstart; i <= rend; i++)
//   {
//      if (m_Points[m_Indexes[i]]->GetValue(in_Dim) < m_OrthoPositions[1][imedian])
//         return false;
//   }
//
//   return true;
//}
//
//template <class T, unsigned char K>
//void EKDTree<T,K>::partitionArray(int in_Low, int in_High, int in_Dim)
//{
//   // This quick and efficient implementation is based on the paper "Fast Median Search: an
//   // ANSI C implementation," Nicolas Devillard - ndevilla AT free DOT fr, 1998.
//
//   /*
//   * This Quickselect routine is based on the algorithm described in
//   * "Numerical recipes in C", Second Edition,
//   * Cambridge University Press, 1992, Section 8.5, ISBN 0-521-43108-5
//   * This code by Nicolas Devillard - 1998. Public domain.
//   */
//
//   // This was modified by AWT a little.
//
//   // These exist purely to make the code easier to read
//   register int tmp;
//#define swapElements(a,b) { tmp = m_Indexes[a];m_Indexes[a] = m_Indexes[b]; m_Indexes[b] = tmp; }
//
//	int median;
//	int middle, ll, hh;
//
//	median = (in_Low + in_High) / 2;
//
//	while (true) {
//		if (in_High <= in_Low) /* One element only */
//			return;
//
//		if (in_High == in_Low + 1) { /* Two elements only */
//			if (m_Points[m_Indexes[in_Low]]->GetValue(in_Dim) > m_Points[m_Indexes[in_High]]->GetValue(in_Dim))
//				swapElements(in_Low, in_High);
//
//			break;
//		}
//
//		/*
//		 * Find median of in_Low, middle and in_High items; swap into position in_Low
//		 */
//		middle = (in_Low + in_High) / 2;
//		if (m_Points[m_Indexes[middle]]->GetValue(in_Dim) > m_Points[m_Indexes[in_High]]->GetValue(in_Dim))
//			swapElements(middle, in_High);
//		if (m_Points[m_Indexes[in_Low]]->GetValue(in_Dim) > m_Points[m_Indexes[in_High]]->GetValue(in_Dim))
//			swapElements(in_Low, in_High);
//		if (m_Points[m_Indexes[middle]]->GetValue(in_Dim) > m_Points[m_Indexes[in_Low]]->GetValue(in_Dim))
//			swapElements(middle, in_Low);
//
//		/* Swap in_Low item (now in position middle) into position (in_Low+1) */
//		swapElements(middle, in_Low + 1);
//
//		/*
//		 * Nibble from each end towards middle, swapping items when stuck
//		 */
//		ll = in_Low + 1;
//		hh = in_High;
//		while (true) {
//			while (m_Points[m_Indexes[in_Low]]->GetValue(in_Dim) > m_Points[m_Indexes[  ++ll]]->GetValue(in_Dim));
//			while (m_Points[m_Indexes[  --hh]]->GetValue(in_Dim) > m_Points[m_Indexes[in_Low]]->GetValue(in_Dim));
//			
//			if (hh < ll)
//				break;
//
//			swapElements(ll, hh);
//		}
//
//		/* Swap middle item (in position in_Low) back into correct position */
//		swapElements(in_Low, hh);
//
//		/* Re-set active partition */
//		if (hh <= median)
//			in_Low = ll;
//
//		if (hh >= median)
//			in_High = hh - 1;
//	}
//#undef swapElements
//}
//
//#undef INITPARTITIONS
//
//template <class T, unsigned char K>
//void EKDTree<T,K>::search(EKDSearcher<T,K>* in_Search, SearchType in_SearchType)
//{
//   int searchCount = 0;
//   this->search(in_Search, searchCount, in_SearchType);
//}
//
//template <class T, unsigned char K>
//void EKDTree<T,K>::search(EKDSearcher<T,K>* in_Search, int& searchCount, SearchType in_SearchType)
//{
//   in_Search->reset();
//   searchCount = 0;
//
//   switch (in_SearchType)
//   {
//   case EXHAUSTIVE:
//      searchExhaustive(in_Search, getRootBranch());
//      break;
//   case RECURSIVE:
//      searchRecursive(in_Search, getRootBranch(), searchCount);
//      break;
//   default:
//      AWTEXCEPTIONTHROW("Illegal Search Type!");
//   }
//}
//
//template <class T, unsigned char K>
//void EKDTree<T,K>::searchExhaustive(EKDSearcher<T,K>* in_Search, EKDTreeBranch<T,K>* in_Branch)
//{
//   KDTreeDataElement<T,K>* dataElement;
//   int origIndex;
//
//   for (int i = 0, imax = in_Branch->getSize(); i < imax; i++)
//   {
//      origIndex = in_Branch->getOriginalIndex(i);
//      dataElement = in_Branch->getOriginalDataElement(origIndex);
//
//      in_Search->checkPointOfInterest(dataElement, origIndex);
//   }
//}
//
//template <class T, unsigned char K>
//void EKDTree<T,K>::searchRecursive(EKDSearcher<T,K>* in_Search, 
//                                            EKDTreeBranch<T,K>* in_Branch, int& branchesSearched)
//{
//   if (in_Branch == 0)
//      return;
//
//   EKDTreeBranch<T,K>* children[2];
//
//   children[0] = in_Branch->getChild(0);
//   children[1] = in_Branch->getChild(1);
//
//   if (children[0] == 0 && children[1] == 0)
//   {
//      // Check this branch exhaustively
//      searchExhaustive(in_Search, in_Branch);
//      ++branchesSearched;
//
//      return;
//   }
//   else
//   {
//      T   lowerBounds[2];
//
//      int oi;
//      int swap;
//
//      for (int i = 0; i < 2; i++)
//      {
//         if (children[i] == 0)
//            lowerBounds[i] = std::numeric_limits<T>::max();
//         else
//            lowerBounds[i] = in_Search->calculateBoxDistanceBounds2(children[i]);
//      }
//
//      swap = (lowerBounds[1] < lowerBounds[0]) ? 1 : 0;
//
//      for (int i = 0; i < 2; i++)
//      {
//         oi = (i+swap) % 2;
//
//         if (children[oi] != 0 && in_Search->shouldCheck(lowerBounds[oi]))
//         {
//            searchRecursive(in_Search, children[oi], branchesSearched);
//         }
//
//         in_Branch->finishedWithChild(oi);
//      }
//   }
//}
//
//} }
//
//#endif // __EKDTree_H__
