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
#ifndef __OEKDNEARESTSEARCHER_H__
#define __OEKDNEARESTSEARCHER_H__

#include "Mesh/MeshTypeDefs.h"

#include "OEKDSearcher.h"
#include "OEKDTreeBranch.h"
#include <queue>
#include <limits>

namespace AWT
{
   namespace OEKDTree
   {
      template <class T, unsigned char K>
      class OEKDTreeBranch;

      template <class T, unsigned char K>
      class OEKDTreeData;

      template <class T>
      struct SqDistBounds
      {
         T lower;
         T upper;
         
         bool operator<(const SqDistBounds<T>& other) const { return lower < other.lower; }
      };

      template <class T, unsigned char K>
      class OEKDNearestSearcher : public OEKDSearcher<T,K>, public virtual ManagedObject
      {
      public:
         typedef ManagedAutoPointer< OEKDNearestSearcher<T,K> > P;

      protected:
         OEKDNearestSearcher( );

         virtual ~OEKDNearestSearcher( );

      public:

         virtual void calculateBoxDistanceBounds2( typename OEKDTreeBranch<T,K>* in_Branch, SqDistBounds<T>& bounds  ) const = 0;

         virtual bool shouldCheck( typename AWT::OEKDTree::OEKDTreeBranch<T,K>* in_Branch, const SqDistBounds<T>& bounds ) const = 0;

         virtual void checkObject( const int in_Index ) = 0;

         virtual void setSearchType( const AWT::MeshSearchType s );

         virtual AWT::MeshSearchType getSearchType( ) const;

         virtual void searchTree( typename OEKDTree<T,K>* in_Tree );

         virtual void checkExhaustive( typename AWT::OEKDTree::OEKDTreeBranch<T,K>* in_Branch );

         virtual void check( typename AWT::OEKDTree::OEKDTreeBranch<T,K>* in_Branch );

         static bool debug;

      protected:
         AWT::MeshSearchType m_SearchType;

         typedef AWT::OEKDTree::OEKDTreeBranch<T,K> BranchType;

         typedef std::pair<SqDistBounds<T>,BranchType*> QueuePair;
         typedef std::priority_queue<QueuePair,std::vector<QueuePair>,std::greater<QueuePair>> SearchQueue;
         
         virtual void check( typename SearchQueue& queue );
      };
   }
}

template <class T, unsigned char K>
bool AWT::OEKDTree::OEKDNearestSearcher<T,K>::debug = false;

template <class T, unsigned char K>
AWT::OEKDTree::OEKDNearestSearcher<T,K>::OEKDNearestSearcher( )
{
   m_SearchType = AWT::SEARCH_RECURSIVE;
}

template <class T, unsigned char K>
AWT::OEKDTree::OEKDNearestSearcher<T,K>::~OEKDNearestSearcher( )
{
}

template <class T, unsigned char K>
AWT::MeshSearchType AWT::OEKDTree::OEKDNearestSearcher<T,K>::getSearchType( ) const
{
   return m_SearchType;
}

template <class T, unsigned char K>
void AWT::OEKDTree::OEKDNearestSearcher<T,K>::setSearchType( AWT::MeshSearchType s )
{
   m_SearchType = s;
}

template <class T, unsigned char K>
void AWT::OEKDTree::OEKDNearestSearcher<T,K>::searchTree( typename AWT::OEKDTree::OEKDTree<T,K>* in_Tree )
{
   OEKDTreeBranch<T,K>* root = in_Tree->getRootBranch( );

   switch ( m_SearchType )
   {
   case AWT::SEARCH_EXHAUSTIVE:
      checkExhaustive( root );
      break;
   case AWT::SEARCH_RECURSIVE:
      check( root );
      break;
   default:
      AWTEXCEPTIONTHROW( "Invalid search type" );
   }

   OEKDTreeBranch<T,K>::checkIn( root );
}

template <class T, unsigned char K>
void AWT::OEKDTree::OEKDNearestSearcher<T,K>::checkExhaustive( typename AWT::OEKDTree::OEKDTreeBranch<T,K>* in_Branch )
{
   OEKDTreeData<T,K>::P data = in_Branch->getTree( )->getData( );

   for ( int i = 0, imax = in_Branch->getSize( ); i < imax; i++ )
   {
      checkObject( in_Branch->getOriginalIndex( i ) );
   }
}

template <class T, unsigned char K>
void AWT::OEKDTree::OEKDNearestSearcher<T,K>::check( typename AWT::OEKDTree::OEKDTreeBranch<T,K>* in_Branch )
{
   if ( debug )
      PRINTVBL( in_Branch->getSize() );

   BranchType* branch = BranchType::checkOut( );
   branch->init( in_Branch );

   SqDistBounds<T> firstBounds = { 0, std::numeric_limits<T>::max() };

   SearchQueue queue;
   queue.push( QueuePair( firstBounds, branch ) );
   while ( !queue.empty() )
      check( queue );
}


template <class T, unsigned char K>
void AWT::OEKDTree::OEKDNearestSearcher<T,K>::check( typename AWT::OEKDTree::OEKDNearestSearcher<T,K>::SearchQueue& queue )
{
   QueuePair pair = queue.top();
   queue.pop();

   // Extract the data from the pair
   const SqDistBounds<T> currentBounds = pair.first;
   BranchType* branch                  = pair.second;

   if ( debug )
      PRINTVBL( branch->getSize() );

   SqDistBounds<T> bounds;

   // Only need to check this if the lower bound is small enough
   if ( shouldCheck( branch, currentBounds ) )
   {
      MeshIndex childrenFound = 0;
      for ( int i = 0; i < 2; i++ )
      {
         OEKDTreeBranch<T,K>* child = branch->getChild( i );

         if ( child != 0 )
         {
            ++childrenFound;
            calculateBoxDistanceBounds2( child, bounds );
            queue.push( QueuePair( bounds, child ) );
         }
      }

      // If there are no children, treat as a leaf and search all
      if ( childrenFound == 0 )
         checkExhaustive( branch );
   }

   BranchType::checkIn( branch );
}

#endif // __OEKDNEARESTSEARCHER_H__