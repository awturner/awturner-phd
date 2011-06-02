#ifndef __CONSTANTTUPLES_H__
#define __CONSTANTTUPLES_H__

#include "Mesh/Tuples.h"

namespace AWT
{
   /*! \class ConstantTuples
   \brief A set of tuples which cannot be changed

   Fakes a set of tuples which you can't change.  It makes it look like there are
   N tuples, all set to the same thing.

   Doesn't look like this is used anywhere.
   */

   template <class T>
   class ConstantTuples : public Tuples<T>
   {
   public:
      typedef ManagedAutoPointer< ConstantTuples<T> > P;

   protected:
      ConstantTuples( const TupleIndex tupleSize, const T* def, const MeshIndex numberOfPoints )
      {
         m_TupleSize = tupleSize;
         m_NumberOfPoints = numberOfPoints;

         m_Def = new T[tupleSize];
         for ( int i = 0; i < tupleSize; ++i )
            m_Def[i] = def[i];
      }

      ~ConstantTuples( )
      {
         delete m_Def;
      }

   public:
      static P getInstance( const TupleIndex tupleSize, const T* def, const MeshIndex numberOfPoints )
      {
         AUTOGETINSTANCE( ConstantTuples<T>, (tupleSize, def, numberOfPoints) );
      }

      virtual TupleIndex getTupleSize( ) const
      {
         return m_TupleSize;
      }

      virtual MeshIndex getNumberOfPoints( ) const
      {
         return m_NumberOfPoints;
      }

      virtual T    getPointElement( const MeshIndex in_Index, const TupleIndex i ) const
      {
         return m_Def[i];
      }

      virtual void getPoint( const MeshIndex in_Index, T* out_Point ) const
      {
         for ( int i = 0; i < 3; ++i )
            out_Point[i] = m_Def[i];
      }

      virtual void setPointElement( const MeshIndex in_Index, const TupleIndex i, const T in_Value )
      {
         AWTEXCEPTIONTHROW( "Not supported" );
      }

      virtual void setPoint( const MeshIndex in_Index, const T* in_Point )
      {
         AWTEXCEPTIONTHROW( "Not supported" );
      }

      virtual void ensureSize( MeshIndex size )
      {
         m_NumberOfPoints = std::max<MeshIndex>( size, m_NumberOfPoints );
      }

      virtual void clear( )
      {
         m_NumberOfPoints = 0;
      }

      virtual void lock( void* /*obj*/ )
      {
      }

      virtual void unlock( void* /*obj*/ )
      {
      }

      virtual bool isLocked( ) const
      {
         return true;
      }

      virtual const T* getDefault( ) const
      {
         return m_Def;
      }

      virtual std::string getClassName( ) const;

   protected:
      TupleIndex m_TupleSize;
      MeshIndex  m_NumberOfPoints;

      T* m_Def;
   };
}

template <class T>
GETNAMEMACRO( AWT::ConstantTuples<T> );

#endif // __CONSTANTTUPLES_H__