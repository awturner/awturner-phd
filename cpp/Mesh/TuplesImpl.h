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
#ifndef __SURFACEMESH_TUPLESIMPL_H__
#define __SURFACEMESH_TUPLESIMPL_H__

#include "Tuples.h"
#include <vector>

namespace AWT
{
   template <class T>
   class TuplesImpl : public Tuples<T>
   {
   public:
      typedef ManagedAutoPointer<TuplesImpl<T>> P;

   public:
      static typename TuplesImpl<T>::P getInstance( const TupleIndex n, const MeshIndex alloc );
      static typename TuplesImpl<T>::P getInstance( const TupleIndex n, const T* in_Default, const MeshIndex alloc );
      static P getInstance( const typename Tuples<T>::P tuples );

      virtual TupleIndex getTupleSize( ) const;

      virtual MeshIndex getNumberOfPoints( ) const;

      virtual T    getPointElement( const MeshIndex in_Index, const TupleIndex i ) const;
      virtual void getPoint( const MeshIndex in_Index, T* out_Point ) const;

      virtual void setPointElement( const MeshIndex in_Index, const TupleIndex i, const T in_Value );
      virtual void setPoint( const MeshIndex in_Index, const T* in_Point );

      virtual void addPoint( const T* in_Point );

      virtual void clear( );

      virtual void ensureSize( MeshIndex size );

      virtual void lock( void* obj );
      virtual void unlock( void* obj );

      virtual bool isLocked( ) const;

      virtual const T* getDefault( ) const;

      virtual std::string getClassName( ) const;

   protected:
      TuplesImpl( const TupleIndex n, const T* in_Default, const MeshIndex alloc );
      TuplesImpl( const TupleIndex n, const MeshIndex alloc );
      virtual ~TuplesImpl( );

      const TupleIndex m_N;
      std::vector<T>  m_Points;
      T*              m_Default;
   };
}

#include "Useful/Exception.h"
#include "Useful/PrintMacros.h"
#include <cmath>

template <class T>
typename AWT::TuplesImpl<T>::P AWT::TuplesImpl<T>::getInstance( const TupleIndex n, const AWT::MeshIndex alloc )
{
   AUTOGETINSTANCE( AWT::TuplesImpl<T>, ( n, alloc ) );
}

template <class T>
typename AWT::TuplesImpl<T>::P AWT::TuplesImpl<T>::getInstance( const TupleIndex n, const T* in_Default, const AWT::MeshIndex alloc )
{
   AUTOGETINSTANCE( AWT::TuplesImpl<T>, ( n, in_Default, alloc ) );
}

template <class T>
typename AWT::TuplesImpl<T>::P AWT::TuplesImpl<T>::getInstance( const typename Tuples<T>::P tuples )
{
   TuplesImpl<T>::P ret = getInstance( tuples->getTupleSize( ), tuples->getDefault( ), tuples->getNumberOfPoints( ) );

   T* point  = new T[ tuples->getTupleSize( ) ];
   T* point2 = new T[ tuples->getTupleSize( ) ];

   for ( MeshIndex i = 0, imax = tuples->getNumberOfPoints(); i < imax; ++i )
   {
      tuples->getPoint( i, point );
      ret->setPoint( i, point );
   }

   for ( MeshIndex v = 0, vmax = tuples->getNumberOfPoints(); v < vmax; ++v )
   {
      tuples->getPoint( v, point );
      ret   ->getPoint( v, point2 );

      /*
      T len = 0;
      
      for ( int j = 0; j < tuples->getTupleSize( ); ++j )
      {
         len      += (point[j]-point2[j])*(point[j]-point2[j]);
      }

      if ( len != 0 )
      {
         PRINTVEC( point, 3 );
         PRINTVEC( point2, 3 );
         PRINTVBL( len );
      }
      */
   }

   delete [] point2;
   delete [] point;

   return ret;
}

template <class T>
AWT::TuplesImpl<T>::TuplesImpl( const TupleIndex n, const T* in_Default, const AWT::MeshIndex alloc )
: m_N( n )
{
   m_Default = new T[m_N];

   for ( TupleIndex i = 0; i < m_N; ++i )
      m_Default[i] = in_Default[i];

   m_Points.reserve( m_N*alloc );
}

template <class T>
AWT::TuplesImpl<T>::TuplesImpl( const TupleIndex n, const AWT::MeshIndex alloc )
: m_N( n )
{
   m_Default = new T[m_N];

   for ( TupleIndex i = 0; i < m_N; ++i )
      m_Default[i] = 0;

   m_Points.reserve( m_N*alloc );
}

template <class T>
AWT::TuplesImpl<T>::~TuplesImpl( )
{
   delete m_Default;
}

template <class T>
GETNAMEMACRO( AWT::TuplesImpl<T> );

template <class T>
AWT::TupleIndex AWT::TuplesImpl<T>::getTupleSize( ) const
{
   return m_N;
}

template <class T>
AWT::MeshIndex AWT::TuplesImpl<T>::getNumberOfPoints( ) const
{
   return static_cast<AWT::MeshIndex>( m_Points.size( ) / m_N );
}

template <class T>
void AWT::TuplesImpl<T>::clear( )
{
   m_Points.clear( );
}

template <class T>
T AWT::TuplesImpl<T>::getPointElement( const AWT::MeshIndex in_Index, const TupleIndex i ) const
{
   /*if ( i < 0 || i >= m_N )
      AWTEXCEPTIONTHROW("Index out of bounds!");*/

   return m_Points[m_N*in_Index + i];
}

template <class T>
void AWT::TuplesImpl<T>::setPointElement( const AWT::MeshIndex in_Index, const TupleIndex i, const T in_Value )
{
   if ( i < 0 || i >= m_N )
      AWTEXCEPTIONTHROW("Index out of bounds!");

   while ( static_cast<MeshIndex>( m_Points.size( ) ) <= in_Index )
      setPoint( static_cast<MeshIndex>( m_Points.size( ) ), m_Default );

   m_Points[m_N*in_Index + i] = in_Value;
}

template <class T>
void AWT::TuplesImpl<T>::getPoint( const AWT::MeshIndex in_Index, T* out_Point ) const
{
   for ( int i = 0; i < m_N; ++i )
      out_Point[i] = m_Points[m_N*in_Index + i];
}

template <class T>
void AWT::TuplesImpl<T>::ensureSize( const MeshIndex size )
{
   this->lock( this );

   if ( m_Points.size( ) < m_N*size )
   {
      while ( m_Points.size( ) < m_N*size )
      {
         for ( TupleIndex i = 0; i < m_N; ++i )
            m_Points.push_back( m_Default[i] );
      }
      
      this->modified( );
   }

   this->unlock( this );
}

template <class T>
void AWT::TuplesImpl<T>::setPoint( const AWT::MeshIndex in_Index, const T* in_Point )
{
   ensureSize( in_Index + 1 );

   this->lock( this );

   for ( TupleIndex i = 0; i < m_N; ++i )
      m_Points[m_N*in_Index + i] = in_Point[i];

   this->modified( );

   this->unlock( this );
}

template <class T>
void AWT::TuplesImpl<T>::lock( void* /*obj*/ )
{
}

template <class T>
void AWT::TuplesImpl<T>::unlock( void* /*obj*/ )
{
}

template <class T>
bool AWT::TuplesImpl<T>::isLocked( ) const
{
   return false;
}

template <class T>
const T* AWT::TuplesImpl<T>::getDefault( ) const
{
   return &m_Default[0];
}

template <class T>
void AWT::TuplesImpl<T>::addPoint( const T* in_Point )
{
   setPoint( getNumberOfPoints( ), in_Point );
}

#endif __SURFACEMESH_TUPLESIMPL_H__