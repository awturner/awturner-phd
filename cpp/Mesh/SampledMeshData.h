#ifndef __SAMPLEDMESHDATA_H__
#define __SAMPLEDMESHDATA_H__

#include "MeshData.h"
#include "SamplePoints.h"
#include "Tuples.h"

namespace AWT
{
   template <class T>
   class SampledMeshData : public MeshData<T>
   {
   public:
      typedef ManagedAutoPointer<SampledMeshData<T>> P;

   protected:
      SampledMeshData( typename MeshData<T>::P data, SamplePoints::P points );
      virtual ~SampledMeshData( );

   public:
      static typename SampledMeshData<T>::P getInstance( typename MeshData<T>::P data, SamplePoints::P points );
      virtual std::string getClassName( ) const;

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

#endif // __SAMPLEDMESHDATA_H__