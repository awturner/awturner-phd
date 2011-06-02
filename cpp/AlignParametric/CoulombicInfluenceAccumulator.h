#ifndef __CoulombicInfluenceAccumulator_H__
#define __CoulombicInfluenceAccumulator_H__

#include "MeshSearching/VerticesSearch.h"

#include "MeshSearching/VerticesTreeData.h"
#include "Useful/ArrayVectorFunctions.h"

#include <limits>

namespace AWT
{
   namespace AlignParametric
   {
      template <class T>
      class CoulombicInfluenceAccumulator : public AWT::VerticesSearch<T>
      {
      public:
         typedef ManagedAutoPointer< CoulombicInfluenceAccumulator<T> > P;
         static P getInstance( );

      protected:
         CoulombicInfluenceAccumulator( );
         virtual ~CoulombicInfluenceAccumulator( );

      public:
         virtual std::string getClassName( ) const;

         virtual void calculateBoxDistanceBounds2( typename OEKDTree::OEKDTreeBranch<T,3>* in_Branch, AWT::OEKDTree::SqDistBounds<T>& in_Bounds ) const;
         virtual bool shouldCheck( typename AWT::OEKDTree::OEKDTreeBranch<T,3>* in_Branch, const AWT::OEKDTree::SqDistBounds<T>& in_Bounds ) const;

         virtual void checkObject( const int in_Index );

         // This is the particle which we are searching relative to
         virtual void initialize( const MeshIndex i, const T point[3] );
         virtual void getTestPoint( T point[3] ) const;
         virtual MeshIndex getTestIndex( ) const;

         // The required percentage change to bother searching
         virtual void setThreshold( const T in_Thresh );
         virtual T    getThreshold( ) const;

         // Minimum distance, make sure distances are at least as big as this
         // (for numerical stability)
         virtual void setCutoff( const T v );
         virtual T    getCutoff( ) const;

         // The accumulated update
         virtual void getUpdate( T* out_Update ) const;
         virtual T    getEnergy( ) const;

         virtual T    getMinDistance( ) const;

         virtual void reset( );

      protected:
         T m_UpdateNorm;
         T m_Update[3];
         T m_PotentialEnergy;
         T m_Cutoff;
         T m_Threshold;

         T m_MaxR;
         T m_MinR;

         MeshIndex m_TestIndex;
         T         m_TestPoint[3];
      };
   }
}

template <class T>
GETNAMEMACRO( AWT::AlignParametric::CoulombicInfluenceAccumulator<T> );

template <class T>
typename AWT::AlignParametric::CoulombicInfluenceAccumulator<T>::P AWT::AlignParametric::CoulombicInfluenceAccumulator<T>::getInstance( )
{
   AUTOGETINSTANCE( AWT::AlignParametric::CoulombicInfluenceAccumulator<T>, ( ) );
}

template <class T>
AWT::AlignParametric::CoulombicInfluenceAccumulator<T>::CoulombicInfluenceAccumulator( )
{
   m_TestIndex = INVALID_INDEX;
   
   reset( );
}

template <class T>
AWT::AlignParametric::CoulombicInfluenceAccumulator<T>::~CoulombicInfluenceAccumulator( )
{
}

template <class T>
void AWT::AlignParametric::CoulombicInfluenceAccumulator<T>::reset( )
{
   AWT::VerticesSearch<T>::reset();

   m_Update[0] = m_Update[1] = m_Update[2] = 0;
   m_UpdateNorm = 0;
   m_PotentialEnergy = 0;

   m_MaxR = 0;
}

template <class T>
void AWT::AlignParametric::CoulombicInfluenceAccumulator<T>::initialize( const MeshIndex i, const T point[3] )
{
   m_TestIndex    = i;

   for ( MeshIndex ax = 0; ax < 3; ++ax )
      m_TestPoint[ax] = point[ax];

   m_MinR = std::numeric_limits<T>::infinity();

   modified();
}

template <class T>
void AWT::AlignParametric::CoulombicInfluenceAccumulator<T>::getTestPoint( T point[3] ) const
{
   for ( MeshIndex ax = 0; ax < 3; ++ax )
      point[ax] = m_TestPoint[ax];
}

template <class T>
AWT::MeshIndex AWT::AlignParametric::CoulombicInfluenceAccumulator<T>::getTestIndex( ) const
{
   return m_TestIndex;
}

template <class T>
void AWT::AlignParametric::CoulombicInfluenceAccumulator<T>::calculateBoxDistanceBounds2( typename AWT::OEKDTree::OEKDTreeBranch<T,3>* in_Branch, AWT::OEKDTree::SqDistBounds<T>& in_bounds ) const
{
   boxChecked();

   T nearestPoint[3];
   for ( int i = 0; i < 3; i++ )
      nearestPoint[i] = std::min( std::max( m_TestPoint[i], in_Branch->getMinimumBound( i ) ), in_Branch->getMaximumBound( i ) );

   const T ret = deltaNormSquared<T>( m_TestPoint, nearestPoint, 3 );

   /*
   T bounds[6];
   for ( int i = 0; i < 3; i++ )
   {
      bounds[2*i+0] = in_Branch->getMinimumBound( i );
      bounds[2*i+1] = in_Branch->getMaximumBound( i );
   }
   */

   in_bounds.lower = ret;
   in_bounds.upper = std::numeric_limits<T>::max();
}

template <class T>
bool AWT::AlignParametric::CoulombicInfluenceAccumulator<T>::shouldCheck( typename AWT::OEKDTree::OEKDTreeBranch<T,3>* in_Branch, const AWT::OEKDTree::SqDistBounds<T>& in_Bounds ) const
{
   // Always check if there is no current update, or if the lower bound is zero
   // (otherwise would get a divide by zero)
   if ( m_UpdateNorm == 0 || in_Bounds.lower < m_Cutoff*m_Cutoff )
      return true;

   if ( in_Bounds.lower > m_MaxR*m_MaxR && getNumberOfChecks() > 10 )
      return false;

   // The maximum influence is that of a virtual point located in_LowerBound away, multiplied by
   // the number of points not yet calculated
   const T ubInfluenceMagnitude = static_cast<T>( in_Branch->getSize() ) / in_Bounds.lower;

   return ubInfluenceMagnitude / m_UpdateNorm > m_Threshold;
}

template <class T>
void AWT::AlignParametric::CoulombicInfluenceAccumulator<T>::checkObject( const int in_Index )
{
   // Don't check thyself
   if ( in_Index == m_TestIndex )
      return;

   const VerticesTreeData<T>* data = *m_Data;

   T otherPoint[3];
   T delta[3];
   data->getPosition( in_Index, otherPoint );

   // We've got the other point; now, calculate the update
   const T rr = deltaNorm<T>( m_TestPoint, otherPoint, 3 );

   if ( rr > 1e-10 )
   {
      const T r = std::max( m_Cutoff, rr );
      
      objectChecked();

      //DEBUGMACRO( "Found another particle, distance is " << rr );
      //PRINTVEC( otherPoint, 3 );

      for ( MeshIndex ax = 0; ax < 3; ++ax )
      {
         delta[ax] = 2 * (m_TestPoint[ax] - otherPoint[ax]) / pow(r,4);
         m_Update[ax] += delta[ax];
      }
   
      // Keep a track of the PE for the regularization cost
      m_PotentialEnergy += 1 / (r*r);

      // Also, calculate the updated norm of the update
      m_UpdateNorm = norm<T>( m_Update, 3 );

      // Record to see if we can stop...
      m_MinR = std::min<T>( m_MinR, r );
      m_MaxR = std::max<T>( m_MaxR, r );
   }
}

template <class T>
T    AWT::AlignParametric::CoulombicInfluenceAccumulator<T>::getMinDistance( ) const
{
   return m_MinR;
}

template <class T>
void AWT::AlignParametric::CoulombicInfluenceAccumulator<T>::setThreshold( const T v )
{
   if ( v < 0 || v > 1 )
      throw "Threshold must be in the range 0..1";

   if ( v != m_Threshold )
   {
      m_Threshold = v;
      modified();
   }
}

template <class T>
T    AWT::AlignParametric::CoulombicInfluenceAccumulator<T>::getThreshold( ) const
{
   return m_Threshold;
}

template <class T>
void AWT::AlignParametric::CoulombicInfluenceAccumulator<T>::getUpdate( T* out_Update ) const
{
   for ( MeshIndex ax = 0; ax < 3; ++ax )
      out_Update[ax] = m_Update[ax];
}

template <class T>
T    AWT::AlignParametric::CoulombicInfluenceAccumulator<T>::getEnergy( ) const
{
   return m_PotentialEnergy;
}

template <class T>
void AWT::AlignParametric::CoulombicInfluenceAccumulator<T>::setCutoff( const T v )
{
   if ( v != m_Cutoff )
   {
      m_Cutoff = v;
      modified();
   }
}

template <class T>
T    AWT::AlignParametric::CoulombicInfluenceAccumulator<T>::getCutoff( ) const
{
   return m_Cutoff;
}

#endif // __CoulombicInfluenceAccumulator_H__