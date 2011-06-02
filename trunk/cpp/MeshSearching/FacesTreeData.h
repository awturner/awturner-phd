#ifndef __TRIANGLEEKDTREEDATA_H__
#define __TRIANGLEEKDTREEDATA_H__

#include "ObjectEKDTree/OEKDTreeData.h"
#include "ObjectEKDTree/OEKDTree.h"
#include "Mesh/Tuples.h"
#include "Mesh/Mesh.h"

namespace AWT
{
   template <class T>
   class FacesTreeData : public OEKDTree::OEKDTreeData<T,3>
   {
   public:
      typedef ManagedAutoPointer<FacesTreeData<T>> P;

   protected:
      FacesTreeData( typename Tuples<MeshIndex>::P in_Faces, typename Tuples<T>::P in_Points );
      virtual ~FacesTreeData( );

   public:
      static P getInstance( typename Tuples<MeshIndex>::P in_Faces, typename Tuples<T>::P in_Points );
      
      virtual OEKDTree::ObjectIndex getNumberOfObjects( ) const;

      virtual T getCoordinate( OEKDTree::ObjectIndex objectIndex, OEKDTree::AxisIndex axis ) const;
      virtual void getPosition( const ObjectIndex objectIndex, T* pos ) const;

      virtual void getVertex( const OEKDTree::ObjectIndex objectIndex, const TupleIndex vertexIndex, T* out_Vertex ) const;

      virtual T getMinimumBound( OEKDTree::ObjectIndex objectIndex, OEKDTree::AxisIndex axis ) const;
      virtual T getMaximumBound( OEKDTree::ObjectIndex objectIndex, OEKDTree::AxisIndex axis ) const;

      virtual std::string getClassName( ) const;

   protected:
      typename Tuples<T>::P         m_Points;
      typename Tuples<MeshIndex>::P m_Faces;
   };
}

namespace AWT
{
   template <class T>
   FacesTreeData<T>::FacesTreeData( typename Tuples<MeshIndex>::P in_Faces, typename Tuples<T>::P in_Points )
   {
      if ( in_Faces->getTupleSize( ) != 3 || in_Points->getTupleSize( ) != 3 )
         AWTEXCEPTIONTHROW("Wrong number of elements in tuple");

      m_Points = in_Points;
      m_Faces  = in_Faces;

   }

   template <class T>
   FacesTreeData<T>::~FacesTreeData( )
   {
   }

   template <class T>
   typename FacesTreeData<T>::P FacesTreeData<T>::getInstance( typename Tuples<MeshIndex>::P in_Faces, typename Tuples<T>::P in_Points )
   {
      AUTOGETINSTANCE( FacesTreeData<T>, ( in_Faces, in_Points ) );
   }

   template <class T>
   std::string FacesTreeData<T>::getClassName( ) const
   {
      return "AWT::FacesTreeData<T>";
   }

   template <class T>
   OEKDTree::ObjectIndex FacesTreeData<T>::getNumberOfObjects( ) const
   {
      return m_Faces->getNumberOfPoints( );
   }

   template <class T>
   void FacesTreeData<T>::getVertex( const OEKDTree::ObjectIndex objectIndex, const TupleIndex vertexIndex, T* out_Vertex ) const
   {
      m_Points->getPoint( m_Faces->getPointElement( objectIndex, vertexIndex ), out_Vertex );
   }

   template <class T>
   T FacesTreeData<T>::getCoordinate( OEKDTree::ObjectIndex objectIndex, OEKDTree::AxisIndex axis ) const
   {
      MeshIndex idx = m_Faces->getPointElement( objectIndex, 0 );
      T ret = m_Points->getPointElement( idx, axis );
      return ret;
   }

   template <class T>
   void FacesTreeData<T>::getPosition( AWT::OEKDTree::ObjectIndex objectIndex, T* pos ) const
   {
      MeshIndex idx = m_Faces->getPointElement( objectIndex, 0 );
      m_Points->getPoint( idx, pos );
   }

   template <class T>
   T FacesTreeData<T>::getMinimumBound( OEKDTree::ObjectIndex objectIndex, OEKDTree::AxisIndex axis ) const
   {
      T ret = m_Points->getPointElement( m_Faces->getPointElement( objectIndex, 0 ), axis );

      for ( int i = 1; i < 3; ++i )
         ret = std::min<T>( ret, m_Points->getPointElement( m_Faces->getPointElement( objectIndex, i ), axis ) );

      return ret;
   }

   template <class T>
   T FacesTreeData<T>::getMaximumBound( OEKDTree::ObjectIndex objectIndex, OEKDTree::AxisIndex axis ) const
   {
      T ret = m_Points->getPointElement( m_Faces->getPointElement( objectIndex, 0 ), axis );

      for ( int i = 1; i < 3; ++i )
         ret = std::max<T>( ret, m_Points->getPointElement( m_Faces->getPointElement( objectIndex, i ), axis ) );

      return ret;
   }
}

#endif // __TRIANGLEEKDTREEDATA_H__