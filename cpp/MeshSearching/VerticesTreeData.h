#ifndef __POINTEKDTREEDATA_H__
#define __POINTEKDTREEDATA_H__

#include "ObjectEKDTree/OEKDTreeData.h"
#include "Mesh/Tuples.h"

using namespace AWT::OEKDTree;

namespace AWT
{
   template <class T>
   class VerticesTreeData : public OEKDTree::OEKDTreeData<T,3>
   {
   public:
      typedef ManagedAutoPointer<VerticesTreeData<T>> P;

   protected:
      VerticesTreeData( typename Tuples<T>::P in_Data );
      virtual ~VerticesTreeData( );

   public:
      static P getInstance( typename Tuples<T>::P in_Data );
      
      virtual ObjectIndex getNumberOfObjects( ) const;

      virtual T getCoordinate( ObjectIndex objectIndex, AxisIndex axis ) const;
      virtual void getPosition( const ObjectIndex objectIndex, T* pos ) const;

      virtual T getMinimumBound( ObjectIndex objectIndex, AxisIndex axis ) const;
      virtual T getMaximumBound( ObjectIndex objectIndex, AxisIndex axis ) const;

      virtual std::string getClassName( ) const;

   protected:
      ManagedAutoPointer<Tuples<T>> m_Data;
   };
}

template <class T>
AWT::VerticesTreeData<T>::VerticesTreeData( typename AWT::Tuples<T>::P in_Data )
{
   if ( in_Data->getTupleSize( ) != 3 )
      AWTEXCEPTIONTHROW("Wrong number of elements in tuple");

   m_Data = in_Data;
}

template <class T>
AWT::VerticesTreeData<T>::~VerticesTreeData( )
{
}

template <class T>
typename AWT::VerticesTreeData<T>::P AWT::VerticesTreeData<T>::getInstance( typename AWT::Tuples<T>::P in_Data )
{
   AUTOGETINSTANCE( VerticesTreeData<T>, ( in_Data ) );
}

template <class T>
AWT::OEKDTree::ObjectIndex AWT::VerticesTreeData<T>::getNumberOfObjects( ) const
{
   return m_Data.getDataConst( )->getNumberOfPoints( );
}

template <class T>
T AWT::VerticesTreeData<T>::getCoordinate( AWT::OEKDTree::ObjectIndex objectIndex, AWT::OEKDTree::AxisIndex axis ) const
{
   return m_Data.getDataConst( )->getPointElement( objectIndex, axis );
}

template <class T>
void AWT::VerticesTreeData<T>::getPosition( AWT::OEKDTree::ObjectIndex objectIndex, T* pos ) const
{
   m_Data.getDataConst( )->getPoint( objectIndex, pos );
}

template <class T>
T AWT::VerticesTreeData<T>::getMinimumBound( AWT::OEKDTree::ObjectIndex objectIndex, AWT::OEKDTree::AxisIndex axis ) const
{
   return m_Data.getDataConst( )->getPointElement( objectIndex, axis );
}

template <class T>
T AWT::VerticesTreeData<T>::getMaximumBound( AWT::OEKDTree::ObjectIndex objectIndex, AWT::OEKDTree::AxisIndex axis ) const
{
   return m_Data.getDataConst( )->getPointElement( objectIndex, axis );
}

template <class T>
GETNAMEMACRO( AWT::VerticesTreeData<T> );

#endif // __POINTEKDTREEDATA_H__