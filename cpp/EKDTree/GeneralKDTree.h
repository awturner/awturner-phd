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
// * File: $RCSfile: GeneralKDTree.h,v $
// * Module path: $Source: /CodePrototypes/GeneralKDTree/include/GeneralKDTree/GeneralKDTree.h,v $
// * Revision: $Revision: 62 $
// * Date: $Date: 2008-08-28 17:29:47 +0100 (Thu, 28 Aug 2008) $
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
// * $Log: GeneralKDTree.h,v $
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
//#ifndef __GENERALKDTREE_H__
//#define __GENERALKDTREE_H__
//
//
//#include <limits>
//#include <vector>
//
///*!
// * @class GeneralKDTree
// * @date 16/05/2007
// * @bug None
// * @todo Nothing
// *
// * @brief This class provides the interface for a k-d tree structure to be used
// * in order to search for a closest point through a large number of points
// * in minimum time.
// *
// * This is the header for the class KDTree, which implements
// * an optimised k-d tree based on the paper "An Algorithm for
// * Finding Best Matches in Logarithmic Expected Time,"
// * Jerome H. Friedman, ACM Transactions on Mathematical
// * Software, Vol. 3, NO. 3, September 1977, Pages 209-226.
// *
// * It is templated on the type of data it is storing (T), and on the number of
// * spatial dimensions (K).  It reuses some code from the existing implementation
// * by FMRYB, but it doesn't use a separate node class.  Because of the templating,
// * all methods are specified in this file.
// */
//template <class T, unsigned char K>
//class GeneralKDTree
//{
//   friend class KDTreeBranch<T,K>;
////! @name Construction
////@{
//public:
//   /*! Class constructor used to build the tree.
//   *
//   * @param *in_data A pointer to an array containing the point list.
//   * @param in_nPoints Size of the array, i.e. the number of points.
//   * @param in_MinSize Minimum size of subtree over which to use tree-based searching - below this level, search exhaustively (more efficient for small trees)
//   *
//   */
//   GeneralKDTree(T** in_data, const int in_npoints, const int in_MinSize = 10);
//   
//   //! Destructor
//   ~GeneralKDTree();
////@}
//
////! @name Queries
////@{
//public:
//   /*!
//   * Returns the number of spatial dimensions
//   *
//   * @return Number of spatial dims.
//   */
//   const int getDimensionality();
////@}
//
//////////////////////////////////////////////////////////////////////////////////
//// Member functions
//public:
//   /*!
//   * Starts the process of building the tree.  Called automatically upon construction, 
//   * but could be called again e.g. if the data changes
//   */
//   void BuildTree();
//
//   /*!
//   * Function used to search for the closest point to the query point.
//   *
//   * @param in_SearchPoint Query point.
//   * @return Closest point index.
//   */
//   const int FindNearestPoint(T* in_SearchPoint);
//
//   /*!
//   * Function used to search for the closest points to the query point
//   * in a sphere of radius "inSearchRadius" centred at the query point.
//   *
//   * @param in_TestPoint Query point.
//   * @param in_Radius Radius of the sphere to be searched.
//   * @return Vector list of closest indices.
//   */
//   std::vector<int> FindNearestPoints(T* in_TestPoint, T in_Radius);
//
//   /*!
//   * Function used to search for the closest points to the query point
//   * in a sphere of radius "inSearchRadius" centred at the query point,
//   * but putting the results into the specified vector
//   *
//   * @param in_TestPoint Query point.
//   * @param in_Radius Radius of the sphere to be searched.
//   * @param out_Results Vector list of closest indices.
//   */
//   void FindNearestPoints(T* in_TestPoint, T in_Radius, std::vector<int>& out_Results);
//
//   /*!
//   * Function used to search for the closest point to the query point.
//   * This method does not use the tree structure, but instead searches
//   * the entire list (present for testing purposes)
//   *
//   * @param in_SearchPoint Query point.
//   * @return Closest point index.
//   */
//   const int FindNearestPointExhaustive(T* p);
//
//   /*!
//   * Function used to search for the closest points to the query point
//   * in a sphere of radius "inSearchRadius" centred at the query point.
//   * This method does not use the tree structure, but instead searches
//   * the entire list (present for testing purposes)
//   *
//   * @param in_TestPoint Query point.
//   * @param in_Radius Radius of the sphere to be searched.
//   * @return Vector list of closest indices.
//   */
//   std::vector<int> FindNearestPointsExhaustive(T* p, T in_Radius);
//
//protected:
//   /*!
//   * Function used to find the square of the distance between two points
//   * in space, i.e. | in_p1-in_p2 |^2.
//   *
//   * @param in_p1 First point.
//   * @param in_p2 Second point.
//   * @return Squared Euclidean distance between.
//   */
//public:
//   const T distanceSquaredBetweenPoints(T* in_p1, T* in_p2);
//protected:
//   /*!
//   * Recursive function for finding all points within a certain radius.
//   * The public function of the same name is just a simpler call to this
//   * function
//   *
//   * @param in_TestPoint Test point.
//   * @param in_RadiusSquared Square of the hypersphere radius to find points within.
//   * @param in_Low First index in the array to check
//   * @param in_High Last index in the array to check
//   * @param in_Dim Current discriminator dimension
//   * @param io_Points Vector to hold found point indices
//   */
//   const void FindNearestPoints(T* in_TestPoint, T in_RadiusSquared, const int in_Low, const int in_High, 
//      int in_Dim, std::vector<int>& io_Points);
//   
//   /*!
//   * Recursive function for finding the closest point in the specified subtree.
//   *
//   * @param in_TestPoint Test point.
//   * @param in_Low First index in the array to check
//   * @param in_High Last index in the array to check
//   * @param in_Dim Current discriminator dimension
//   * @param out_minD Distance squared corresponding to closest point
//   * @param in_Bounds Bounding box of current tree
//   * @return Nearest point index
//   */
//   const int FindNearestPoint(T* in_TestPoint, const int in_Low, const int in_High, int in_Dim, T& out_minD, T* in_Bounds);
//
//   /*!
//   * Function used to search for the closest point to the query point.
//   * This method does not use the tree structure, but instead searches
//   * the entire list (present for testing purposes)
//   *
//   * @param in_SearchPoint Query point.
//   * @param in_Low Start Index
//   * @param in_High End Index
//   * @param out_BestDistance Minimum squared distance
//   * @return Closest point index.
//   */
//   const int FindNearestPointExhaustive(T* p, const int in_Low, const int in_High, T& out_BestDistance);
//
//   /*!
//   * Function used to search for the closest points to the query point
//   * in a sphere of radius "inSearchRadius" centred at the query point.
//   * This method does not use the tree structure, but instead searches
//   * the entire list (present for testing purposes)
//   *
//   * @param in_TestPoint Query point.
//   * @param in_RadiusSquared Square of radius of the sphere to be searched.
//   * @param in_Low Start Index
//   * @param in_High End Index
//   * @param io_Points Vector list of closest indices.
//   */
//   void FindNearestPointsExhaustive(T* p, T in_RadiusSquared, const int in_Low, const int in_High, 
//      std::vector<int>& io_Points);
//
//   /*!
//   * Handles the recursive calls to partition the array into [ {<= median} median {>= median} ]
//   *
//   * @param in_Low First index in the array to check
//   * @param in_High Last index in the array to check
//   * @param in_Dim Current discriminator dimension
//   */
//   KDTreeBranch<T,K>* recursivelyPartition(int in_Low, int in_High, int in_Dim, T* out_Bounds);
//
//	/*!
//   * Does the meat of partitioning the array into [ {<= median} median {>= median} ]
//   *
//   * @param in_Low First index in the array to check
//   * @param in_High Last index in the array to check
//   * @param in_Dim Current discriminator dimension
//   */
//   void partitionArray(int in_Low, int in_High, int in_Dim);
//
//////////////////////////////////////////////////////////////////////////////////
//// Member variables
//public:
//   static int QueryCount;
//protected:
//   //! Number of points to be searched over
//   const int  m_NPoints;
//
//   //! Minimum size of tree (below which just do exhaustive searching)
//   const int  m_MinSize;
//
//   //! Pointer to the data array
//   T**  m_Points;
//
//   //! Pointer to the index array (this gets rearranged, rather than the data array)
//   int* m_Indexes;
//
//   T*   m_Bounds;
//};
//
//template <class T, unsigned char K>
//int GeneralKDTree<T,K>::QueryCount = 0;
//
//// These definitions are used in several places throughout; it is convenient to have them
//// as a define to avoid having to hunt and change
//#define INITPARTITIONS(low,high) \
//   int imedian = low + (high-low)/2; \
//   int lstart = low; \
//   int lend   = imedian - 1; \
//   int rstart = imedian + 1; \
//   int rend   = high;
//
//// Constructor
//template <class T, unsigned char K>
//GeneralKDTree<T,K>::GeneralKDTree(T** in_data, const int in_npoints, const int in_MinSize)
//   : m_MinSize(in_MinSize), m_NPoints(in_npoints)
//{
//   if (in_MinSize < 0)
//      throw "in_MinSize >= 0";
//
//   this->m_Points  = in_data;
//   this->m_Indexes = new int[m_NPoints];
//
//   this->m_Bounds = new T[2*K];
//
//   BuildTree();
//}
//
//// Destructor
//template <class T, unsigned char K>
//GeneralKDTree<T,K>::~GeneralKDTree()
//{
//   delete m_Indexes;
//   delete m_Bounds;
//}
//
//// Queries
//template <class T, unsigned char K>
//const int GeneralKDTree<T,K>::getDimensionality()
//{
//   return K;
//}
//
//// Methods
//template <class T, unsigned char K>
//const int GeneralKDTree<T,K>::FindNearestPoint(T* p)
//{
//   // Start the recursion
//   T minD = std::numeric_limits<T>::infinity();
//
//   T* bounds = new T[2*K];
//
//   for (int i = 0; i < 2*K; i++)
//   {
//      bounds[i] = m_Bounds[i];
//   }
//
//   int np = FindNearestPoint(p, 0, m_NPoints-1, 0, minD, bounds);
//
//   delete bounds;
//
//   return np;
//}
//
//template <class T, unsigned char K>
//std::vector<int> GeneralKDTree<T,K>::FindNearestPoints(T* in_TestPoint, T in_Radius)
//{
//   // Start the recursion
//   std::vector<int> ret;
//
//   FindNearestPoints(in_TestPoint, in_Radius*in_Radius, ret);
//   return ret;
//}
//
//template <class T, unsigned char K>
//void GeneralKDTree<T,K>::FindNearestPoints(T* in_TestPoint, T in_Radius, std::vector<int>& out_Results)
//{
//   // Start the recursion
//   FindNearestPoints(in_TestPoint, in_Radius*in_Radius, 0, m_NPoints-1, 0, out_Results);
//}
//
//template <class T, unsigned char K>
//const int GeneralKDTree<T,K>::FindNearestPointExhaustive(T* p)
//{
//   T bestDistance = std::numeric_limits<T>::infinity();
//   return FindNearestPointExhaustive(p, 0, m_NPoints - 1, bestDistance);
//}
//
//template <class T, unsigned char K>
//const int GeneralKDTree<T,K>::FindNearestPointExhaustive(T* p, const int in_Low, const int in_High, T& out_BestDistance)
//{
//   // If none of the points are better than out_BestDistance, then we should return "no point"
//   int bestIndex    = -1;
//   T testDistance;
//   
//	for (int i = in_Low; i <= in_High; i++)
//	{
//		testDistance = distanceSquaredBetweenPoints(p, m_Points[ m_Indexes[i] ]);
//		if (testDistance < out_BestDistance)
//		{
//			out_BestDistance = testDistance;
//			bestIndex        = m_Indexes[i];
//		}
//	}
//	
//	return bestIndex;
//}
//
//template <class T, unsigned char K>
//std::vector<int> GeneralKDTree<T,K>::FindNearestPointsExhaustive(T* p, T in_Radius)
//{
//   std::vector<int> points;
//   FindNearestPointsExhaustive(p, in_Radius*in_Radius, 0, m_NPoints-1, points);
//
//   return points;
//}
//
//template <class T, unsigned char K>
//void GeneralKDTree<T,K>::FindNearestPointsExhaustive(T* p, T in_RadiusSquared, const int in_Low, const int in_High,
//                                                     std::vector<int>& io_Points)
//{
//   // This is the no-brainer way of finding the nearest points -
//   // just go over the whole data set
//   T testDistance;
//   
//   for (int i = in_Low; i <= in_High; i++)
//   {
//      testDistance = distanceSquaredBetweenPoints(p, m_Points[ m_Indexes[i] ]);
//      if (testDistance < in_RadiusSquared)
//      {
//         io_Points.push_back(m_Indexes[i]);
//      }
//   }
//}
//
//template <class T, unsigned char K>
//const T GeneralKDTree<T,K>::distanceSquaredBetweenPoints(T* ps, T* qs)
//{
//   ++QueryCount;
//   // Calculate Euclidean norm
//	T ret = 0;
//	T tmp;
//   
//	for (int i = 0; i < K; i++)
//	{
//      tmp  = ps[i] - qs[i];
//		ret += tmp * tmp;
//	}
//   // but don't square root it (x^2 is monotonic in x)
//
//   mfs << "scatter2x([";
//   for (int i = 0; i < 2 && i < K; i++)
//   {
//      mfs << qs[i] << ";";
//   }
//   mfs << "], 'ro');" << std::endl;
//   
//   if (QueryCount < 20)
//      mfs << "text(" << qs[0] << "," << qs[1] << ", '" << QueryCount << "');" << std::endl;
//      
//   
//   
//	return ret;
//}
//
//template <class T, unsigned char K>
//const void GeneralKDTree<T,K>::FindNearestPoints(T* in_TestPoint, T in_RadiusSquared, const int low, 
//                                                  const int high, int dim, std::vector<int>& io_Points)
//{
//   double distanceSquared;
//
//	if (high - low < m_MinSize)
//	{
//		// If the partition m_Indexes small, it's more efficient to do exhaustive search!
//      FindNearestPointsExhaustive(in_TestPoint, in_RadiusSquared, low, high, io_Points);
//	}
//	else
//	{
//      int dimm = (dim+1) % K;
//      
//      INITPARTITIONS(low, high);
//
//      // Firstly, test the discriminator point
//      distanceSquared = distanceSquaredBetweenPoints(in_TestPoint, m_Points[ m_Indexes[low] ]);
//      if (distanceSquared < in_RadiusSquared)
//      {
//         io_Points.push_back(m_Indexes[low]);
//      }
//
//      // This is the minimum distance from the test point to the discriminator hyperplane
//		T orthoDistance = m_Points[m_Indexes[low]][dim] - in_TestPoint[dim]; 
//		
//		int chstart, chend;
//		
//      // Depending on which side of the boundary the test point lies, we want to test the leq or geq
//      // sub-tree first
//		if (orthoDistance > 0)
//		{
//			chstart = lstart;
//			chend   = lend;
//		}
//		else
//		{
//			chstart = rstart;
//			chend   = rend;
//		}
//		
//      FindNearestPoints(in_TestPoint, in_RadiusSquared, chstart, chend, dimm, io_Points);
//		
//      // If the boundary is further away than the radius of the sphere, there's no point in checking the
//      // other sub-tree
//		if (orthoDistance*orthoDistance < in_RadiusSquared)
//		{
//			chstart = lstart + rstart - chstart;
//			chend   = lend   + rend   - chend;
//		
//         FindNearestPoints(in_TestPoint, in_RadiusSquared, chstart, chend, dimm, io_Points);
//		}
//	}
//}
//
//#include <cmath>
//
//template <class T, unsigned char K>
//const int GeneralKDTree<T,K>::FindNearestPoint(T* p, const int low, const int high, int dim, T& minD, T* bounds)
//{
//	if (high - low < m_MinSize)
//	{
//      return FindNearestPointExhaustive(p, low, high, minD);
//	}
//	else
//	{
//      int dimm = (dim+1) % K;
//		int tmpIdx;
//
//      INITPARTITIONS(low,high);
//
//      // Finds the nearest point on tree bounding box
//      // - fast breaks once the cumulative distance exceeds minD
//      {
//         T tmpDelta;
//         T testDistance = 0;
//
//         for (int i = 0; i < K; i++)
//         {
//            tmpDelta = ::min(::max(p[i], bounds[2*i+0]), bounds[2*i+1]) - p[i];
//            testDistance += tmpDelta * tmpDelta;
//
//            if (testDistance > minD)
//               return -1;
//         }
//      }
//
//      if (K >= 2)
//      {
//         T x = bounds[0];
//         T y = bounds[2];
//         T r = bounds[1];
//         T b = bounds[3];
//
//         T w = r-x;
//         T h = b-y;
//
//         if (w > 0 && h > 0)
//            mfs << "rectangle('Position', [" << x << " " << y << " " << w << " " << h << "]);" << std::endl;
//      }
//
//      // This is the minimum distance from the test point to the discriminator hyperplane
//      T orthoDistance = m_Points[m_Indexes[imedian]][dim] - p[dim];
//
//      int chstart, chend;
//		
//      // Depending on which side of the boundary the test point lies, we want to test the leq or geq
//      // sub-tree first
//		if (orthoDistance > 0)
//		{
//			chstart = lstart; chend   = lend;
//		}
//		else
//		{
//			chstart = rstart; chend   = rend;
//		}
//
//      T testDistance;
//      int bestIndex = -1;
//
//      T minBound = bounds[2*dim+0];
//      T maxBound = bounds[2*dim+1];
//
//      for (int i = 0; i < 2; i++)
//      {
//         if (chstart == lstart)
//         {
//            bounds[2*dim+1] = m_Points[m_Indexes[imedian]][dim];
//         }
//         else
//         {
//            bounds[2*dim+0] = m_Points[m_Indexes[imedian]][dim];
//         }
//
//         // Now check the subtree which contains the test point
//         tmpIdx = FindNearestPoint(p, chstart, chend, dimm, testDistance = minD, bounds);
//
//         if (tmpIdx != -1)
//         {
//            bestIndex = tmpIdx;
//            minD = testDistance;
//         }
//
//         bounds[2*dim+0] = minBound;
//         bounds[2*dim+1] = maxBound;
//
//         
//         if (i == 0)
//         {
//            if (orthoDistance*orthoDistance >= minD)
//               break;
//
//            chstart = lstart + rstart - chstart;
//            chend   = lend   + rend   - chend;
//
//            testDistance = distanceSquaredBetweenPoints(p, m_Points[ m_Indexes[imedian] ]);
//            if (testDistance < minD)
//            {
//               minD = testDistance;
//               bestIndex = m_Indexes[imedian];
//            }
//         }
//      }
//
//		return bestIndex;
//	}
//}
//
//int NSTYLES = 8;
//const char* styles[] = { "r.", "g.", "b.", "m.", "r+", "g+", "b+", "m+" };
//
//template <class T, unsigned char K>
//void printPoints(KDTreeBranch<T,K>* /*branch*/, int /*depth*/)
//{
//   /*
//   if (branch == 0)
//      return;
//
//   T point[K];
//
//   mfs << "scatter2x([";
//
//   for (int i = 0, imax = branch->getSize(); i < imax; i++)
//   {
//      branch->getPoint(i, point);
//
//      for (int d = 0; d < K; d++)
//         mfs << point[d] << " ";
//   
//      mfs << ";" << std::endl;
//   }
//
//   mfs << "]', '" << styles[depth % NSTYLES] << "');" << std::endl;
//   */
//}
//
//template <class T, unsigned char K>
//void copyBounds(T* bounds, T* ret)
//{
//   for (int i = 0; i < 2*K; i++)
//      ret[i] = bounds[i];
//}
//
//template <class T, unsigned char K>
//int findPlaneDirection(T* plane)
//{
//   for (int i = 0; i < K; i++)
//      if (plane[i] != 0.0)
//         return i;
//
//   throw "Degenerate plane!";
//}
//
//template <class T, unsigned char K>
//void printBranch(KDTreeBranch<T,K>* branch, int depth, T* bounds)
//{
//   return;
//
//   double* plane = new double[K+1];
//   T* bs = new T[2*K];
//
//   if (branch->getLeftChild() == 0 || branch->getRightChild() == 0)
//   {
//      printPoints(branch, depth);
//   }
//   else
//   {
//      branch->getDiscriminatorPlane(0, plane);
//      int ax = findPlaneDirection<T,K>(plane);
//
//      copyBounds<T,K>(bounds, bs);
//      bs[2*ax+1] = min(bs[2*ax+1], plane[2]);
//
//      printBranch(branch->getLeftChild(), depth+1, bs);
//      
//      copyBounds<T,K>(bounds, bs);
//      bs[2*ax+0] = max(bs[2*ax+0], plane[2]);
//
//      printBranch(branch->getRightChild(), depth+1, bs);
//   }
//
//   copyBounds<T,K>(bounds, bs);
//   branch->getDiscriminatorPlane(0, plane);
//   int ax = findPlaneDirection<T,K>(plane);
//
//   bs[2*ax+0] = ((plane[ax] < 0) ? 1 : -1) * plane[2];
//   bs[2*ax+1] = ((plane[ax] < 0) ? 1 : -1) * plane[2];
//
//   /*
//   if (depth < 10)
//   {
//      mfs << "hnd = plot2x([" << bs[0] << " " << bs[2] << "; " << bs[1] << " " << bs[3] << "]', 'c');";
//      mfs << "set(hnd, 'LineWidth', " << max(10-2*depth,1) << "); pause; %depth=" << depth << std::endl;
//   }
//   */
//
//   delete bs;
//   delete plane;
//}
//
//template <class T, unsigned char K>
//void printBranch(KDTreeBranch<T,K>* branch)
//{
//   T* bounds = new T[2*K];
//
//   for (int d = 0; d < K; d++)
//   {
//      bounds[2*d+0] = branch->getMinimumBound(d);
//      bounds[2*d+1] = branch->getMaximumBound(d);
//   }
//
//   printBranch(branch, 0, bounds);
//
//   delete bounds;
//}
//
//template <class T, unsigned char K>
//void GeneralKDTree<T,K>::BuildTree()
//{
//   // Fill the array of indexes - these will be [0..in_npoints-1] to start with,
//   // but they'll be rearranged when the tree is built
//   for (int i = 0; i < m_NPoints; i++)
//      this->m_Indexes[i] = i;
//
//   //mfs << "BuildTree()" << std::endl;
//	KDTreeBranch<T,K>* branches = recursivelyPartition(0, m_NPoints-1, 0, m_Bounds);
//
//   std::cerr << "General Tree: Bounds = " << std::endl;
//   for (int i = 0; i < K; i++)
//      std::cerr << m_Bounds[2*i+0] << "->" << m_Bounds[2*i+1] << std::endl;
//
//   printBranch(branches);
//
//   delete branches;
//}
//
//template <class T, unsigned char K>
//KDTreeBranch<T,K>* GeneralKDTree<T,K>::recursivelyPartition(int low, int high, int dim, T* out_Bounds)
//{
//   T data;
//
//   if (high - low < m_MinSize)
//	{
//      int ax;
//      for (int i = low; i <= high; i++)
//      {
//         for (ax = 0; ax < K; ++ax)
//         {
//            data = m_Points[m_Indexes[i]][ax];
//            out_Bounds[2*ax+0] = ::min(out_Bounds[2*ax+0], data);
//            out_Bounds[2*ax+1] = ::max(out_Bounds[2*ax+1], data);
//         }
//      }
//
//      return new KDTreeBranch<T,K>(this, low, high, dim, 0, 0, 0);
//	}
//
//   // Do the partitioning of the current array
//   partitionArray(low, high, dim);
//
//   INITPARTITIONS(low,high);
//
//   // And now make the recursive calls to do the leq and geq subtrees
//	int dimm = (dim+1) % K;
//
//   T* boundsL = new T[2*K];
//   T* boundsR = new T[2*K];
//
//	KDTreeBranch<T,K>* left  = recursivelyPartition(lstart, lend, dimm, boundsL);
//	KDTreeBranch<T,K>* right = recursivelyPartition(rstart, rend, dimm, boundsR);
//
//   for (int ax = 0; ax < K; ++ax)
//   {
//      data = m_Points[m_Indexes[imedian]][ax];
//
//      out_Bounds[2*ax+0] = ::min(::min(boundsL[2*ax+0], boundsR[2*ax+0]), data);
//      out_Bounds[2*ax+1] = ::max(::max(boundsL[2*ax+1], boundsR[2*ax+1]), data);
//   }
//
//   delete boundsL;
//   delete boundsR;
//
//   return new KDTreeBranch<T,K>(this, low, high, dim, m_Points[m_Indexes[imedian]][dim], left, right);
//}
//
//template <class T, unsigned char K>
//void GeneralKDTree<T,K>::partitionArray(int in_Low, int in_High, int in_Dim)
//{
//   
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
//			if (m_Points[m_Indexes[in_Low]][in_Dim] > m_Points[m_Indexes[in_High]][in_Dim])
//				swapElements(in_Low, in_High);
//
//			return;
//		}
//
//		/*
//		 * Find median of in_Low, middle and in_High items; swap into position in_Low
//		 */
//		middle = (in_Low + in_High) / 2;
//		if (m_Points[m_Indexes[middle]][in_Dim] > m_Points[m_Indexes[in_High]][in_Dim])
//			swapElements(middle, in_High);
//		if (m_Points[m_Indexes[in_Low]][in_Dim] > m_Points[m_Indexes[in_High]][in_Dim])
//			swapElements(in_Low, in_High);
//		if (m_Points[m_Indexes[middle]][in_Dim] > m_Points[m_Indexes[in_Low]][in_Dim])
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
//			while (m_Points[m_Indexes[in_Low]][in_Dim] > m_Points[m_Indexes[  ++ll]][in_Dim]);
//			while (m_Points[m_Indexes[  --hh]][in_Dim] > m_Points[m_Indexes[in_Low]][in_Dim]);
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
//#endif // __GENERALKDTREE_H__
