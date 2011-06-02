#ifndef __OBJECTEKDTREE_H__
#define __OBJECTEKDTREE_H__


#include "ObjectManagement/ManagedObject.h"

namespace AWT
{
   namespace OEKDTree
   {
      template <class T, unsigned char K>
      class OEKDTreeData;

      /*!
       * @class OEKDTree
       * @date 16/05/2007
       *
       * @brief Implementation of K-d tree data structure for efficient spatial searching.
       *
       * This is the header for the class KDTree, which implements
       * an optimised k-d tree based on the paper "An Algorithm for
       * Finding Best Matches in Logarithmic Expected Time,"
       * Jerome H. Friedman, ACM Transactions on Mathematical
       * Software, Vol. 3, NO. 3, September 1977, Pages 209-226.
       *
       * It is templated on the type of data it is storing (T), and on the number of
       * spatial dimensions (K).  It reuses some code from the existing implementation
       * by FMRYB, but it doesn't use a separate node class.  Because of the templating,
       * all methods are specified in this file.
       */

      template <class T, unsigned char K>
      class OEKDSearcher;

      template <class T, unsigned char K>
      class OEKDTreeBranch;

      template <class T, unsigned char K>
      class OEKDTree : public virtual ManagedObject
      {
      public:
         typedef ManagedAutoPointer< OEKDTree<T,K> > P;

      private:
         template <class T, unsigned char K>
         friend class OEKDTreeBranch;

      public:
         typedef ManagedAutoPointer<OEKDTree<T,K>> P;

      //! @name Construction
      //@{
      public:
         static P getInstance( typename OEKDTreeData<T,K>::P in_Data, const int in_MinSize = 10, const bool in_KeepData = false );

         /*! Class constructor used to build the tree.
         *
         * @param *in_data A pointer to an array of data elements
         * @param in_npoints Size of the array, i.e. the number of points.
         * @param in_MinSize Minimum size of subtree over which to use tree-based searching - below this level, search exhaustively (more efficient for small trees)
         *
         */
      protected:
         OEKDTree( typename OEKDTreeData<T,K>::P in_Data, const int in_MinSize, const bool in_KeepData );
         
         //! Destructor
         ~OEKDTree( );
      //@}

      //! @name Queries
      //@{
      public:
         /*!
         * Returns the number of spatial dimensions
         *
         * @return Number of spatial dims.
         */
         const int getDimensionality( );
      //@}

         typename OEKDTreeData<T,K>::P getData( ) const;

         int getOriginalIndex( const int in_Index ) const;

         virtual std::string getClassName( ) const;

      ////////////////////////////////////////////////////////////////////////////////
      // Member functions
      public:
         /*!
         * Starts the process of building the tree.  Called automatically upon construction, 
         * but could be called again e.g. if the data changes
         */
         void build( );

         const int getMinimumSize( );

         //void search( OEKDSearcher<T,K>* in_Search, SearchType in_SearchType = RECURSIVE );

         typename OEKDTreeBranch<T,K>* getRootBranch( );

      protected:
         /*
         void searchRecursive ( OEKDSearcher<T,K>* in_Search, OEKDTreeBranch<T,K>* in_Branch );
         void searchExhaustive( OEKDSearcher<T,K>* in_Search, OEKDTreeBranch<T,K>* in_Branch );
         */

         /*!
         * Handles the recursive calls to partition the array into [ {<= median} median {>= median} ]
         *
         * @param in_Low First index in the array to check
         * @param in_High Last index in the array to check
         * @param in_Dim Current discriminator dimension
         * @param out_Bounds Bounding box of the current points [dim1_min dim1_max dim2_min dim2_max ... ]
         */
         void recursivelyPartition( const int in_Low, const int in_High, const int in_Dim, T* out_Bounds );

	      /*!
         * Does the meat of partitioning the array into [ {<= median} median {>= median} ]
         *
         * @param in_Low First index in the array to check
         * @param in_High Last index in the array to check
         * @param in_Dim Current discriminator dimension
         */
         void partitionArray( int in_Low, int in_High, int in_Dim );

         void branchCalculateChildBounds( const int in_ChildIndex, const int in_Low, const int in_High, int& out_Low, int& out_High, unsigned char& dim, T& out_Bound );

         bool checkInvariant( const int in_Low, const int in_High, const int in_Dim );

      ////////////////////////////////////////////////////////////////////////////////
      // Member variables

         //! Pointer to the data
         typename OEKDTreeData<T,K>::P m_Data;

         //! Minimum size of tree (below which just do exhaustive searching)
         const int  m_MinSize;

         int m_NPoints;

         //! Pointer to the index array (this gets rearranged, rather than the data array)
         int* m_Indexes;

         //! Tree of orthogonal distances to cutting planes
         T* m_OrthoPositions[2];

         //! Overall bounds of the tree
         T m_Bounds[2*K];

         //! Which axis is the branch split on
         unsigned char* m_SplitAxis;

         bool m_KeepData;
      };
   }
}

#include <iostream>
#include "ObjectEKDTree/OEKDTreeBranch.h"
#include "ObjectEKDTree/OEKDTreeData.h"
#include "ObjectEKDTree/OEKDSearcher.h"
#include "Useful/PrintMacros.h"
#include <limits>
#include <vector>
#include "Useful/Exception.h"

using namespace AWT::OEKDTree;

// Constructor
template <class T, unsigned char K>
AWT::OEKDTree::OEKDTree<T,K>::OEKDTree( typename OEKDTreeData<T,K>::P in_data, const int in_MinSize, const bool in_KeepData )
   : m_MinSize( in_MinSize )
{
   if ( in_MinSize < 1 )
      AWTEXCEPTIONTHROW("in_MinSize should be >= 1");

   this->m_KeepData = in_KeepData;

   if ( in_KeepData )
   {
      in_data->keep( );
   }

   this->m_Data = in_data;

   // These arrays will be created when the tree is built
   this->m_Indexes           = 0;
   this->m_OrthoPositions[0] = 0;
   this->m_OrthoPositions[1] = 0;
   this->m_NPoints           = 0;
   this->m_SplitAxis         = 0;

   build( );
}

// Destructor
template <class T, unsigned char K>
AWT::OEKDTree::OEKDTree<T,K>::~OEKDTree( )
{
   if ( this->m_KeepData )
      m_Data->release( );

   if ( m_Indexes != 0 )
      delete m_Indexes;

   if ( m_OrthoPositions[0] != 0 )
      delete m_OrthoPositions[0];

   if ( m_OrthoPositions[1] != 0 )
      delete m_OrthoPositions[1];

   if ( m_SplitAxis != 0 )
      delete m_SplitAxis;
}

// These definitions are used in several places throughout; it is convenient to have them
// as a define to avoid having to hunt and change
// This has been modified to make this into a K-d trie (i.e. points only stored at leaves, 
// not at branches)
#define INITPARTITIONS(low,high) \
   const int imedian = low + (high-low)/2; \
   const int lstart = low; \
   const int lend   = imedian; \
   const int rstart = imedian + 1; \
   const int rend   = high;

template <class T, unsigned char K>
std::string AWT::OEKDTree::OEKDTree<T,K>::getClassName( ) const
{
   return "AWT::OEKDTree::OEKDTree<T,K>";
}

template <class T, unsigned char K>
void AWT::OEKDTree::OEKDTree<T,K>::branchCalculateChildBounds( const int in_ChildIndex, const int in_Low, const int in_High, int& out_Low, int& out_High, unsigned char& out_Dim, T& out_Bound )
{
   INITPARTITIONS( in_Low, in_High );
   
   switch ( in_ChildIndex )
   {
   case 0:
      out_Low  = lstart;
      out_High = lend;
      break;
   case 1:
      out_Low  = rstart;
      out_High = rend;
      break;
   default:
      AWTEXCEPTIONTHROW("Invalid Child Index!");
   }

   out_Dim   = m_SplitAxis[imedian];
   out_Bound = m_OrthoPositions[in_ChildIndex][imedian];
}

template <class T, unsigned char K>
const int AWT::OEKDTree::OEKDTree<T,K>::getMinimumSize( )
{
   return m_MinSize;
}

template <class T, unsigned char K>
typename AWT::OEKDTree::OEKDTree<T,K>::P AWT::OEKDTree::OEKDTree<T,K>::getInstance( typename OEKDTreeData<T,K>::P in_data, const int in_MinSize, const bool in_KeepData )
{
   OEKDTree<T,K>* inst = new OEKDTree<T,K>( in_data, in_MinSize, in_KeepData );
   inst->countMe( );

   OEKDTree<T,K>::P ret( inst );
   inst->release( );

   return ret;
}

// Queries
template <class T, unsigned char K>
const int AWT::OEKDTree::OEKDTree<T,K>::getDimensionality( )
{
   return K;
}

template <class T, unsigned char K>
typename AWT::OEKDTree::OEKDTreeData<T,K>::P AWT::OEKDTree::OEKDTree<T,K>::getData( ) const
{
   return m_Data;
}

template <class T, unsigned char K>
void AWT::OEKDTree::OEKDTree<T,K>::build( )
{
   // Fill the array of indexes - these will be [0..in_npoints-1] to start with,
   // but they'll be rearranged when the tree is built

   if ( this->m_Indexes == 0 || this->m_OrthoPositions[0] == 0 || this->m_OrthoPositions[1] == 0 ||
      m_NPoints != m_Data->getNumberOfObjects( ) )
   {
      m_NPoints = m_Data->getNumberOfObjects( );

      if ( this->m_Indexes != 0 )
         delete [] this->m_Indexes;

      if ( this->m_OrthoPositions[0] )
         delete [] this->m_OrthoPositions[0];

      if ( this->m_OrthoPositions[1] )
         delete [] this->m_OrthoPositions[0];

      if ( this->m_SplitAxis )
         delete [] this->m_SplitAxis;

      this->m_Indexes           = new int[m_NPoints];
      this->m_OrthoPositions[0] = new T[m_NPoints];
      this->m_OrthoPositions[1] = new T[m_NPoints];
      this->m_SplitAxis         = new unsigned char[m_NPoints];
   }

   for ( int i = 0; i < m_NPoints; i++ )
   {
      this->m_Indexes[i] = i;
   }

	recursivelyPartition( 0, m_NPoints-1, 0, m_Bounds );
   
   this->modified( );
}

template <class T, unsigned char K>
void AWT::OEKDTree::OEKDTree<T,K>::recursivelyPartition( const int low, const int high, const int dim, T* out_Bounds )
{
   //std::cerr << low << "\t" << high << std::endl;

	if ( high - low < m_MinSize )
	{
      int d, i;
      
      for ( d = 0; d < K; d++ )
      {
         out_Bounds[2*d + 0] =  std::numeric_limits<T>::max( );
         out_Bounds[2*d + 1] = -out_Bounds[2*d + 0];
      }

      for ( i = low; i <= high; i++ )
      {
         for ( d = 0; d < K; d++ )
         {
            out_Bounds[2*d + 0] = std::min( out_Bounds[2*d + 0], m_Data->getMinimumBound( m_Indexes[i], d ) );
            out_Bounds[2*d + 1] = std::max( out_Bounds[2*d + 1], m_Data->getMaximumBound( m_Indexes[i], d ) );
         }
      }

		return;
	}

   // Do the partitioning of the current array
   partitionArray( low, high, dim );
   
   // Calculate the direction of most variance
   T variance[K], sums[K];
   for (int i = 0; i < K; ++i)
      variance[i] = sums[i] = 0;

   T pos[K];
   for (int i = low; i <= high; ++i)
   {
      m_Data->getPosition( m_Indexes[i], pos );
      for (unsigned char d = 0; d < K; ++d)
      {
         variance[d] += pos[d]*pos[d];
         sums[d] += pos[d];
      }
   }

   unsigned char bestDim = 0;

   for (unsigned char d = 0; d < K; ++d)
   {
      variance[d] -= sums[d]*sums[d] / (high-low+1);

      if (variance[d] > variance[bestDim])
         bestDim = d;
   }

   INITPARTITIONS(low,high);

   // Create arrays to record the bounds of the left and right sub-trees
   T boundsL[6];
   T boundsR[6];

   // And now make the recursive calls to do the leq and geq subtrees
	const int dimm = bestDim; //(dim+1) % K;
   
   //DEBUGMACRO( dimm << "\t" << (1*bestDim) );

	recursivelyPartition( lstart, lend, dimm, boundsL );
	recursivelyPartition( rstart, rend, dimm, boundsR );
   
   m_OrthoPositions[0][imedian] = boundsL[2*dim + 1];
   m_OrthoPositions[1][imedian] = boundsR[2*dim + 0];
   m_SplitAxis[imedian]         = dimm;

   // Work out the bounding box which surrounds the discriminator point and the two sub trees
   for ( int d = 0; d < K; d++ )
   {
      // Take the minimum of the minimum bounds
      out_Bounds[2*d + 0] = std::min( m_Data->getMinimumBound( m_Indexes[imedian], d ), std::min( boundsL[2*d + 0], boundsR[2*d + 0] ) );

      // and the maximum of the maximum bounds
      out_Bounds[2*d + 1] = std::max( m_Data->getMaximumBound( m_Indexes[imedian], d ), std::max( boundsL[2*d + 1], boundsR[2*d + 1] ) );
   }

   if ( !checkInvariant( low, high, dim ) )
   {
      std::cerr << "Invariant Error at (low=" << low << ", high=" << high << ", dim=" << dim << ")" << std::endl;
   }
}

template <class T, unsigned char K>
bool AWT::OEKDTree::OEKDTree<T,K>::checkInvariant( const int in_Low, const int in_High, const int in_Dim )
{
   INITPARTITIONS( in_Low, in_High );

   for ( int i = lstart; i <= lend; i++ )
   {
      if ( m_Data->getCoordinate( m_Indexes[i], in_Dim ) > m_OrthoPositions[0][imedian] )
         return false;
   }

   for ( int i = rstart; i <= rend; i++ )
   {
      if ( m_Data->getCoordinate( m_Indexes[i], in_Dim ) < m_OrthoPositions[1][imedian] )
         return false;
   }

   return true;
}

template <class T, unsigned char K>
void AWT::OEKDTree::OEKDTree<T,K>::partitionArray( int in_Low, int in_High, int in_Dim )
{
   // This quick and efficient implementation is based on the paper "Fast Median Search: an
   // ANSI C implementation," Nicolas Devillard - ndevilla AT free DOT fr, 1998.

   /*
   * This Quickselect routine is based on the algorithm described in
   * "Numerical recipes in C", Second Edition,
   * Cambridge University Press, 1992, Section 8.5, ISBN 0-521-43108-5
   * This code by Nicolas Devillard - 1998. Public domain.
   */

   // This was modified by AWT a little.

   // These exist purely to make the code easier to read
   register int tmp;
#define swapElements(a,b) { tmp = m_Indexes[a];m_Indexes[a] = m_Indexes[b]; m_Indexes[b] = tmp; }

	int median;
	int middle, ll, hh;

	median = (in_Low + in_High) / 2;

	while (true) {
		if (in_High <= in_Low) /* One element only */
			return;

		if (in_High == in_Low + 1) { /* Two elements only */
			if ( m_Data->getCoordinate( m_Indexes[in_Low], in_Dim ) > m_Data->getCoordinate( m_Indexes[in_High], in_Dim ) )
				swapElements( in_Low, in_High );

			break;
		}

		/*
		 * Find median of in_Low, middle and in_High items; swap into position in_Low
		 */
		middle = (in_Low + in_High) / 2;
		if ( m_Data->getCoordinate( m_Indexes[middle], in_Dim ) > m_Data->getCoordinate( m_Indexes[in_High], in_Dim ) )
			swapElements( middle, in_High );
		if ( m_Data->getCoordinate( m_Indexes[in_Low], in_Dim ) > m_Data->getCoordinate( m_Indexes[in_High], in_Dim ) )
			swapElements( in_Low, in_High );
		if ( m_Data->getCoordinate( m_Indexes[middle], in_Dim ) > m_Data->getCoordinate( m_Indexes[in_Low], in_Dim ) )
			swapElements( middle, in_Low );

		/* Swap in_Low item (now in position middle) into position (in_Low+1) */
		swapElements( middle, in_Low + 1 );

		/*
		 * Nibble from each end towards middle, swapping items when stuck
		 */
		ll = in_Low + 1;
		hh = in_High;
		while (true) {
			while ( m_Data->getCoordinate( m_Indexes[in_Low], in_Dim ) > m_Data->getCoordinate( m_Indexes[++ll], in_Dim ) );
			while ( m_Data->getCoordinate( m_Indexes[--hh], in_Dim ) > m_Data->getCoordinate( m_Indexes[in_Low], in_Dim ) );
			
			if (hh < ll)
				break;

			swapElements( ll, hh );
		}

		/* Swap middle item (in position in_Low) back into correct position */
		swapElements( in_Low, hh );

		/* Re-set active partition */
		if (hh <= median)
			in_Low = ll;

		if (hh >= median)
			in_High = hh - 1;
	}
#undef swapElements
}

#undef INITPARTITIONS


template <class T, unsigned char K>
int AWT::OEKDTree::OEKDTree<T,K>::getOriginalIndex( const int in_Index ) const
{
   return m_Indexes[in_Index];
}

template <class T, unsigned char K>
AWT::OEKDTree::OEKDTreeBranch<T,K>* AWT::OEKDTree::OEKDTree<T,K>::getRootBranch( )
{
   OEKDTreeBranch<T,K>* root = OEKDTreeBranch<T,K>::checkOut();
   root->init( this, 0, m_NPoints-1, 0, m_Bounds );

   return root;
}

#endif // __OBJECTEKDTREE_H__
