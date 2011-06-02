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
#ifndef __OEKDTreeBranch_H__
#define __OEKDTreeBranch_H__

#include "ObjectEKDTree/OEKDTree.h"
#include "AxisAlignedBoundingBox.h"
#include <vector>


namespace AWT
{
   namespace OEKDTree
   {
      typedef unsigned char ChildIndex;

       /*!
       * @class OEKDTreeBranch
       * @date ?
       * @todo Dispose of instances after some event?
       *
       * @brief Represents a branching of the K-d tree
       *
       * This is a convenience class for handling a branching of the K-d tree.  It enables
       * you to get information such as the extents of the bounding box, as well as giving
       * access to the child branches.
       *
       * You can't instantiate this class - you only need instances briefly, and so this
       * class maintains an internal store from which instances are "checked out" when
       * needed, and checked back in when no longer needed.
       */
      template <class T, unsigned char K>
      class OEKDTreeBranch : public AxisAlignedBoundingBox<T,K>
      {
      public:
         typedef ManagedAutoPointer< OEKDTreeBranch<T,K> > P;

         /*! Allow OEKDTree to manipulate me
         */
         //template <class T, unsigned char K>
         //friend class OEKDTree;

      private:

         /*! Private constructor.
         * Not sure why this one exists...
         */
         OEKDTreeBranch() { ++InstanceCount; }

         /*! Private constructor
         * @todo Work out if parameter ''end'' is inclusive or exclusive
         * @param tree   KDTree of which this is a branching
         * @param start  Starting index within the tree's internal data store
         * @param end    Ending index within the tree's internal data store
         * @param bounds Array of bounds in order [ min_dim_0 max_dim_0 min_dim_1 max_dim_1...min_dim_(K-1) max_dim_(K-1) ]
         */
         OEKDTreeBranch(OEKDTree<T,K>* tree, const int start, const int end, const Axis axis, const T* bounds);

      public:
         ~OEKDTreeBranch();

         std::string getClassName() const;

         /*! Initialize this branch
         * @param tree   KDTree of which this is a branching
         * @param start  Starting index within the tree's internal data store
         * @param end    Ending index within the tree's internal data store
         * @param axis   Axis along which to discriminate
         * @param bounds Array of bounds in order [ min_dim_0 max_dim_0 min_dim_1 max_dim_1...min_dim_(K-1) max_dim_(K-1) ]
         */
         void init(OEKDTree<T,K>* tree, const int start, const int end, const Axis axis, const T* bounds);

         void init(OEKDTreeBranch<T,K>* other);

         /*! Get the minimum extent along the specified axis
         * @param in_Axis The desired axis
         * @return Upper bound on x : p[in_Axis] >= x
         */
         T getMinimumBound(const Axis in_Axis) const;
         
         /*! Get the maximum extent along the specified axis
         * @param in_Axis The desired axis
         * @return Lower bound on x : p[in_Axis] <= x
         */
         T getMaximumBound(const Axis in_Axis) const;

         /*! Get the child branching of this branch
         * The parameter in_ChildIndex should be 0 (zero) or 1 (one):
         * <ul>
         *   <li>1 - the half of the children closer to the minimum bound in the discriminator axis
         *   <li>1 - the half of the children closer to the minimum bound in the discriminator axis
         * </ul>
         * @param in_ChildIndex Child index
         * @return Desired child branch
         */
         OEKDTreeBranch<T,K>* getChild(const ChildIndex in_ChildIndex);

         /*! Get the tree that this branch is part of
         * @return Tree
         */
         OEKDTree<T,K>* getTree();

         /*! The total number of elements in this branch
         * @return The total number of elements in this branch
         */
         int  getSize() const;

         /*! Convert an index relative to this branching to the index in the parent tree
         * @return Index within the parent tree
         */
         int getOriginalIndex(const int in_Index) const;

         /*
         * @return Difference between parent indices and this tree's indices
         */
         int  getOffset() const;

      public:
         /*! The total number of instances in existence
         */
         static int InstanceCount;

         /*! Get an instance of the tree branch
         */
         static OEKDTreeBranch<T,K>* checkOut();

         /*! Return this branch, you've finished using it
         * <b>Do not use the branch after you have called this method!</b>
         * It might have been handed out to somebody else, or destroyed.
         */
         static void checkIn(OEKDTreeBranch<T,K>* in_Branch);

         /*! Destroy the store of previously used branches
         */
         static void destroyStore();

      protected:
         /*! Copy one bounds array to another
         */
         static void copyBounds(const T* in_Source, T* out_Dest);

         /*! Internal store of branch instances
         */
         static std::vector<OEKDTreeBranch<T,K>*>    m_BranchStack;
         
         /*! Parent tree
         */
         typename OEKDTree<T,K>*          m_Tree;

         /*! Start index in parent tree
         */
         int                              m_Start;
         
         /*! End index in parent tree
         */
         int                              m_End;
         
         /*! Discriminator axis
         */
         unsigned char                    m_Axis;

         T                                m_Bounds[2*K];
      };
   }
}

#include <iostream>

template <class T, unsigned char K>
std::string AWT::OEKDTree::OEKDTreeBranch<T,K>::getClassName() const
{
   return "AWT::OEKDTree::OEKDTreeBranch<T,K>";
}

namespace AWT { namespace OEKDTree {

template <class T, unsigned char K>
std::vector<OEKDTreeBranch<T,K>*> OEKDTreeBranch<T,K>::m_BranchStack;

template <class T, unsigned char K>
OEKDTreeBranch<T,K>* OEKDTreeBranch<T,K>::checkOut()
{
   if (m_BranchStack.empty())
   {
      OEKDTreeBranch<T,K>* ret = new OEKDTreeBranch<T,K>();
      ret->countMe();
      return ret;
   }
   else
   {
      OEKDTreeBranch<T,K>* ret = m_BranchStack.back();
      m_BranchStack.pop_back();
      return ret;
   }
}

template <class T, unsigned char K>
void OEKDTreeBranch<T,K>::checkIn(OEKDTreeBranch<T,K>* in_Branch)
{
   m_BranchStack.push_back(in_Branch);
}  

template <class T, unsigned char K>
void OEKDTreeBranch<T,K>::destroyStore()
{
   m_BranchStack.clear();
}

template <class T, unsigned char K>
int OEKDTreeBranch<T,K>::InstanceCount = 0;

template <class T, unsigned char K>
OEKDTreeBranch<T,K>::OEKDTreeBranch(OEKDTree<T,K>* tree, const int start, const int end, const Axis axis, const T* bounds)
{
   ++InstanceCount;
   init(tree, start, end, axis, bounds);
}

template <class T, unsigned char K>
void OEKDTreeBranch<T,K>::init(OEKDTreeBranch<T,K>* other)
{
   init(other->m_Tree, other->m_Start, other->m_End, other->m_Axis, other->m_Bounds);
}

template <class T, unsigned char K>
void OEKDTreeBranch<T,K>::init(OEKDTree<T,K>* tree, const int start, const int end, const Axis axis, const T* bounds)
{
   m_Tree  = tree;
   m_Start = start;
   m_End   = end;
   m_Axis  = axis;

   // Copy the passed bounds
   copyBounds(bounds, m_Bounds);
}

template <class T, unsigned char K>
OEKDTreeBranch<T,K>::~OEKDTreeBranch()
{
}

template <class T, unsigned char K>
void OEKDTreeBranch<T,K>::copyBounds(const T *in_Source, T *out_Dest)
{
   for (int i = 0; i < 2*K; i++)
      out_Dest[i] = in_Source[i];
}

template <class T, unsigned char K>
int OEKDTreeBranch<T,K>::getOffset() const
{
   return m_Start;
}

template <class T, unsigned char K>
OEKDTreeBranch<T,K>* OEKDTreeBranch<T,K>::getChild(const ChildIndex in_ChildIndex)
{
   if (getSize() <= m_Tree->getMinimumSize())
      return 0;

   OEKDTreeBranch<T,K>* ret = checkOut();

   int cstart, cend;
   unsigned char cdim;
   T tightenedBound;
   m_Tree->branchCalculateChildBounds(in_ChildIndex, m_Start, m_End, cstart, cend, cdim, tightenedBound);

   ret->init(m_Tree, cstart, cend, cdim/*(m_Axis+1)%K*/, m_Bounds);

   ret->m_Bounds[2*m_Axis + (1-in_ChildIndex)] = tightenedBound;
   
   return ret;
}

template <class T, unsigned char K>
int OEKDTreeBranch<T,K>::getOriginalIndex(const int in_Index) const
{
   return m_Tree->getOriginalIndex(m_Start + in_Index);
}

template <class T, unsigned char K>
T OEKDTreeBranch<T,K>::getMinimumBound(const Axis in_Axis) const
{
   return m_Bounds[2*in_Axis+0];
}

template <class T, unsigned char K>
T OEKDTreeBranch<T,K>::getMaximumBound(const Axis in_Axis) const
{
   return m_Bounds[2*in_Axis+1];
}

template <class T, unsigned char K>
int OEKDTreeBranch<T,K>::getSize() const
{
   int ret = m_End - m_Start + 1;
   if (ret < 0) ret = 0;
   return ret;
}

template <class T, unsigned char K>
OEKDTree<T,K>* OEKDTreeBranch<T,K>::getTree()
{
   return m_Tree;
}

} }

#endif // __OEKDTreeBranch_H__
