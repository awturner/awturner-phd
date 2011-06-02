#ifndef __MESHALIGNMENT_H__
#define __MESHALIGNMENT_H__

#include "ObjectManagement/ManagedObject.h"
#include "MeshAlignmentDataListener.h"

namespace AWT
{
   template <class T>
   class Mesh;

   template <class T>
   class MeshAlignmentData : public virtual ManagedObject
   {
   public:
      typedef ManagedAutoPointer<MeshAlignmentData<T>> P;

   protected:
      MeshAlignmentData( );

      virtual ~MeshAlignmentData( );

   public:
      static P getInstance( );

      typename Mesh<T>::P getMovingPointsOriginal( ) const;

      typename Mesh<T>::P getFixedPointsOriginal( ) const;

      typename Mesh<T>::P getMovingPoints( ) const;

      typename Mesh<T>::P getFixedPoints( ) const;

      void setMovingPoints( typename Mesh<T>::P movingPoints );

      void setFixedPoints( typename Mesh<T>::P fixedPoints );

      void addListener( typename MeshAlignmentDataListener<T>::P madl );

      void removeListener( typename MeshAlignmentDataListener<T>::P madl );

      virtual std::string getClassName( ) const;

   protected:
      struct D;
      D* m_D;
   };
}

#endif // __MESHALIGNMENT_H__