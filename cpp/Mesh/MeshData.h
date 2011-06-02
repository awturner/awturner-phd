#ifndef __MESHDATA_H__
#define __MESHDATA_H__

#include "ObjectManagement/ManagedObject.h"

#include "Tuples.h"

namespace AWT
{
   template <class T>
   class Mesh;

   template <class T>
   class MeshData : public ManagedObject
   {
   public:
      typedef ManagedAutoPointer<MeshData<T>> P;

   protected:
      MeshData( Mesh<T>* mesh );
      virtual ~MeshData( );

   public:
      static P getInstance( Mesh<T>* mesh );

      virtual std::string getClassName( ) const;

      virtual typename Mesh<T>* getMesh( );

      virtual void addVertexData( const std::string& label, typename Tuples<T>::P data );
      virtual void addFaceData( const std::string& label, typename Tuples<T>::P data );

      virtual bool hasVertexData( const std::string& label ) const;
      virtual bool hasFaceData( const std::string& label ) const;

      virtual typename Tuples<T>::P getVertexData( const std::string& label );
      virtual typename Tuples<T>::P getFaceData( const std::string& label );

      virtual void removeVertexData( const std::string& label );
      virtual void removeFaceData( const std::string& label );

   protected:
      struct D;
      D* m_D;

   };
}

#endif // __MESHDATA_H__