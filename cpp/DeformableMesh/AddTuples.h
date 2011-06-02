#ifndef __ADDTUPLES_H__
#define __ADDTUPLES_H__

#include "Mesh/Tuples.h"
#include <vector>
#include "Useful/Exception.h"

namespace AWT
{
   /*! \class AddTuples
   \brief A weighted sum of tuples

   Takes a number of tuples objects with the same number of elements per tuple
   and the same number of tuples, and provides a weighted sum of them
   */

   template <class T>
   class AddTuples : public Tuples<T>
   {
   public:
      typedef ManagedAutoPointer< AddTuples<T> > P;

   protected:
      AddTuples( TupleIndex tupleSize )
      {
         m_TupleSize = tupleSize;

         m_Default[0] = m_Default[1] = m_Default[2] = 0;
      }

      virtual ~AddTuples( )
      {
      }

   public:
      static P getInstance( TupleIndex tupleSize )
      {
         AUTOGETINSTANCE( AddTuples<T>, ( tupleSize ) );
      }

      virtual void insert( Tuples<T>::P tuples, T weight = 1 )
      {
         m_Tuples.push_back( tuples );
         m_Multipliers.push_back( weight );
      }

      virtual TupleIndex getTupleSize( ) const
      {
         return m_TupleSize;
      }

      virtual MeshIndex getNumberOfComponents( ) const
      {
         return static_cast<MeshIndex>( m_Tuples.size( ) );
      }

      virtual MeshIndex getNumberOfPoints( ) const
      {
         return m_Tuples.back( )->getNumberOfPoints( );
      }

      virtual T    getPointElement( const MeshIndex in_Index, const TupleIndex i ) const
      {
         T ret = 0;

         T weight;

         //size_t p = m_Tuples.size( );
         //do
         //{
         //   --p;

         //   weight = m_Multipliers[p];

         //   ret += weight * m_Tuples[p]->getPointElement( in_Index, i );
         //}
         //while ( p != 0 );

         
         std::vector<Tuples<T>::P>::const_iterator begin = m_Tuples.begin( );
         std::vector<Tuples<T>::P>::const_iterator end   = m_Tuples.end( );

         std::vector<T>::const_iterator beginMult = m_Multipliers.begin( );

         for ( ; begin != end; ++begin, ++beginMult )
         {
            weight = *beginMult;
            ret += weight * (*begin)->getPointElement( in_Index, i );
         }
         

         return ret;
      }

      virtual void getPoint( const MeshIndex in_Index, T* out_Point ) const
      {
         std::vector<Tuples<T>::P>::const_iterator begin = m_Tuples.begin( );
         std::vector<Tuples<T>::P>::const_iterator end   = m_Tuples.end( );

         std::vector<T>::const_iterator beginMult = m_Multipliers.begin( );

         out_Point[0] = out_Point[1] = out_Point[2] = 0;

         T tmp[3];
         T weight;

         for ( ; begin != end; ++begin, ++beginMult )
         {
            weight = *beginMult;
            (*begin)->getPoint( in_Index, tmp );
            
            out_Point[0] += weight * tmp[0];
            out_Point[1] += weight * tmp[1];
            out_Point[2] += weight * tmp[2];
         }
      }

      virtual void addPoint( const T* in_Point )
      {
         setPoint( getNumberOfPoints( ), in_Point );
      }

      virtual void setPointElement( const MeshIndex /*in_Index*/, const TupleIndex /*i*/, const T /*in_Value*/ )
      {
         AWTEXCEPTIONTHROW( "Not supported" );
      }

      virtual void setPoint( const MeshIndex /*in_Index*/, const T* /*in_Point*/ )
      {
         AWTEXCEPTIONTHROW( "Not supported" );
      }

      virtual void ensureSize( MeshIndex /*size*/ )
      {
         AWTEXCEPTIONTHROW( "Not supported" );
      }

      virtual void clear( )
      {
         // Do nothing
      }

      virtual void lock( void* )
      {
         // Do nothing, this will always appear locked
      }

      virtual void unlock( void* )
      {
         // Do nothing, this will always appear locked
      }

      virtual bool isLocked( ) const
      {
         return true;
      }

      virtual T getScale( const unsigned int index ) const
      {
         return m_Multipliers[index];
      }

      virtual void setScale( const unsigned int index, const T sc )
      {
         m_Multipliers[index] = sc;
         modified( );
      }

      virtual void setScale( const T* scales )
      {
         for ( unsigned int i = 0; i < m_Multipliers.size( ); ++i )
            m_Multipliers[i] = scales[i];
         modified();
      }

      ModifiedTime getModifiedTime( ) const
      {
         ModifiedTime ret = Tuples<T>::getModifiedTime( );

         std::vector<Tuples<T>::P>::const_iterator begin = m_Tuples.begin( );
         std::vector<Tuples<T>::P>::const_iterator end   = m_Tuples.end( );

         for ( ; begin != end; ++begin )
         {
            ret = std::max<ModifiedTime>( ret, (*begin)->getModifiedTime( ) );
         }

         return ret;
      }

      virtual const T* getDefault( ) const
      {
         return m_Default;
      }

      virtual std::string getClassName( ) const;

      virtual unsigned int getNumberOfMembers( ) const
      {
         return static_cast<unsigned int>( m_Tuples.size( ) );
      }

   protected:
      T m_Default[3];
      TupleIndex m_TupleSize;
      std::vector<typename Tuples<T>::P> m_Tuples;
      std::vector<T>          m_Multipliers;
   };
}

template <class T>
GETNAMEMACRO( AWT::AddTuples<T> );

#endif __ADDTUPLES_H__