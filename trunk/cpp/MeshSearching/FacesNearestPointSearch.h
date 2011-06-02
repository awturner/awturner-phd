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
#ifndef __FACESNEARESTPOINTSEARCH_H__
#define __FACESNEARESTPOINTSEARCH_H__

#include "FacesSearch.h"

#include "MeshSearching/FacesTreeData.h"

#include <limits>

namespace AWT
{
   template <class T>
   class FacesNearestPointSearch : public FacesSearch<T>
   {
      template <class T, unsigned char K = 3>
      friend class OEKDTree::OEKDTree;

      template <class T, unsigned char K = 3>
      friend class OEKDTree::OEKDTreeBranch;

   public:
      typedef ManagedAutoPointer< FacesNearestPointSearch<T> > P;

   public:
      static P getInstance( );

   protected:
      FacesNearestPointSearch( const T in_MaxDistance = std::numeric_limits<T>::infinity( ) );
      virtual ~FacesNearestPointSearch( );

   public:
      virtual void calculateBoxDistanceBounds2( typename OEKDTreeBranch<T,3>* in_Branch, SqDistBounds<T>& bounds  ) const;
      virtual bool shouldCheck( typename AWT::OEKDTree::OEKDTreeBranch<T,3>* in_Branch, const SqDistBounds<T>& bounds ) const;

      virtual void checkObject( const int in_Index );

      virtual void setTestPoint( const T* in_TestPoint  );
      virtual void getTestPoint( T* out_TestPoint ) const;

      virtual void setTestNormal( const T* in_TestNormal );
      virtual void getTestNormal( T* out_TestNormal ) const;

      virtual int  getNearestPoint( T* out_NearestPoint = 0 ) const;

      virtual void reset( );

      virtual T getCompatibilityCosine( ) const;
      virtual void setCompatibilityCosine( const T v );

      virtual std::string getClassName( ) const;

   protected:
      int m_NearestIndex;
      T   m_NearestPoint[3];

      T m_InitMaxDistance2;
      T m_MaxDistance2;
      T m_TestPoint[3];
      T m_TestNormal[3];

      T m_CompatibilityCosine;
   };
}

#include "Useful/ArrayVectorFunctions.h"
#include "FaceQueries.h"
#include "BoxQueries.h"
#include "PointQueries.h"

template <class T>
GETNAMEMACRO( AWT::FacesNearestPointSearch<T> );

namespace AWT
{
   template <class T>
   typename typename FacesNearestPointSearch<T>::P FacesNearestPointSearch<T>::getInstance( )
   {
      AUTOGETINSTANCE( FacesNearestPointSearch<T>, ( ) );
   }

   template <class T>
   FacesNearestPointSearch<T>::FacesNearestPointSearch( const T in_MaxDistance )
   {
      m_InitMaxDistance2 = in_MaxDistance*in_MaxDistance;
      m_CompatibilityCosine = -1;

      reset( );
   }

   template <class T>
   FacesNearestPointSearch<T>::~FacesNearestPointSearch( )
   {

   }

   template <class T>
   void FacesNearestPointSearch<T>::calculateBoxDistanceBounds2( typename OEKDTree::OEKDTreeBranch<T,3>* in_Branch, OEKDTree::SqDistBounds<T>& bounds ) const
   {
      boxChecked();
      bounds.lower = BoxQueries<T>::getClosestDistance2( m_TestPoint, in_Branch, m_MaxDistance2 );
      bounds.upper = std::numeric_limits<T>::max();
   }

   template <class T>
   bool FacesNearestPointSearch<T>::shouldCheck( typename AWT::OEKDTree::OEKDTreeBranch<T,3>* in_Branch, const OEKDTree::SqDistBounds<T>& bounds ) const
   {
      return bounds.lower < m_MaxDistance2;
   }

   template <class T>
   void FacesNearestPointSearch<T>::checkObject( const int in_Index )
   {
      objectChecked();
      T nearestOnTriangle[3];

      if ( m_CompatibilityCosine > -1 )
      {
         T faceNormal[3];
         FaceQueries<T>::getNormal( m_Data, in_Index, faceNormal );

         if ( dot( m_TestNormal, faceNormal, 3 ) < m_CompatibilityCosine )
         {

            return;
         }
      }
      
      FaceQueries<T>::getNearestPointOnTriangle( m_TestPoint, m_Data, in_Index, nearestOnTriangle );
      
      T dist2 = PointQueries<T>::getDistanceSquared( m_TestPoint, nearestOnTriangle, m_MaxDistance2 );

      if ( dist2 >= m_MaxDistance2 )
      {

         return;
      }

      m_NearestIndex = in_Index;
      m_MaxDistance2 = dist2;

      for ( int i = 0; i < 3; i++ )
         m_NearestPoint[i] = nearestOnTriangle[i];
      

   }

   template <class T>
   void FacesNearestPointSearch<T>::setTestNormal( const T* in_TestPoint  )
   {
      T len = 0;
      for ( int i = 0; i < 3; i++ )
      {
         m_TestNormal[i] = in_TestPoint[i];
         len += m_TestNormal[i]*m_TestNormal[i];
      }

      if ( len != 0 )
      {
         // Normalize if length isn't zero
         len = sqrt( len );
         for ( int i = 0; i < 3; ++i )
            m_TestNormal[i] /= len;
      }
   }

   template <class T>
   void FacesNearestPointSearch<T>::getTestNormal( T* out_TestPoint ) const
   {
      for ( int i = 0; i < 3; i++ )
         out_TestPoint[i] = m_TestNormal[i];
   }

   template <class T>
   void FacesNearestPointSearch<T>::setTestPoint( const T* in_TestPoint  )
   {
      for ( int i = 0; i < 3; i++ )
         m_TestPoint[i] = in_TestPoint[i];
   }

   template <class T>
   void FacesNearestPointSearch<T>::getTestPoint( T* out_TestPoint ) const
   {
      for ( int i = 0; i < 3; i++ )
         out_TestPoint[i] = m_TestPoint[i];
   }

   template <class T>
   int  FacesNearestPointSearch<T>::getNearestPoint( T* out_NearestPoint = 0 ) const
   {
      if ( m_NearestIndex != -1 && out_NearestPoint != 0 )
      {
         for ( int i = 0; i < 3; i++ )
            out_NearestPoint[i] = m_NearestPoint[i];
      }

      return m_NearestIndex;
   }

   template <class T>
   void FacesNearestPointSearch<T>::reset( )
   {
      FacesSearch<T>::reset();
      m_NearestIndex = -1;
      m_MaxDistance2 = m_InitMaxDistance2;
   }

   template <class T>
   T FacesNearestPointSearch<T>::getCompatibilityCosine( ) const
   {
      return m_CompatibilityCosine;
   }

   template <class T>
   void FacesNearestPointSearch<T>::setCompatibilityCosine( const T v )
   {
      m_CompatibilityCosine = v;
      modified( );
   }
}

#endif __FACESNEARESTPOINTSEARCH_H__