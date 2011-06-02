#ifndef __MESHIMPL_H__
#define __MESHIMPL_H__

#include "Mesh.h"

#include <vector>
#include "Tuples.h"
#include "TuplesImpl.h"

namespace AWT
{
   template <class T>
   class MeshImpl : public Mesh<T>
   {
   public:
      typedef ManagedAutoPointer<MeshImpl<T>> P;

   protected:
      MeshImpl( const MeshIndex in_NumberOfVertices = 0, const MeshIndex in_NumberOfFaces = 0 );
      virtual ~MeshImpl( );

   public:
      static  P                    getInstance( const MeshIndex in_NumberOfVertices = 0, const MeshIndex in_NumberOfFaces = 0 );
      static  P                    getInstance( const typename Mesh<T>::P mesh );

      virtual MeshIndex            getNumberOfVertices( ) const;
      
      virtual typename Tuples<T>::P getVertices( ) const;
      virtual void                  setVertices( typename AWT::Tuples<T>::P in_Vertices );

      virtual typename Tuples<MeshIndex>::P getFaces( ) const;
      virtual void                 setFaces( typename AWT::Tuples<MeshIndex>::P in_Vertices );

      virtual void                 getVertex( const MeshIndex in_Index, T* out_Vertex ) const;

      virtual void                 setVertex( const MeshIndex in_Index, const T x, const T y, const T z );
      virtual void                 setVertex( const MeshIndex in_Index, const T* in_Vertex );

      virtual bool                  hasVertexNormals( ) const;
      virtual typename Tuples<T>::P getVertexNormals( ) const;
      virtual void                  setVertexNormals( typename AWT::Tuples<T>::P in_VertexNormals );

      virtual bool                  hasTextureCoords( ) const;
      virtual typename Tuples<T>::P getTextureCoords( ) const;
      virtual void                  setTextureCoords( typename AWT::Tuples<T>::P in_TexCoords );

      virtual bool                 hasFaces( ) const;
      virtual MeshIndex            getNumberOfFaces( ) const;

      virtual void                 getFace( const MeshIndex in_Index, T* out_VertexA, T* out_VertexB, T* out_VertexC ) const;
      
      virtual void                 getFaceIndices( const MeshIndex in_Index, MeshIndex* out_VertexIndices ) const;

      virtual void                 setFaceIndices( const MeshIndex in_Index, const MeshIndex a, const MeshIndex b, const MeshIndex c );
      virtual void                 setFaceIndices( const MeshIndex in_Index, const MeshIndex* in_VertexIndices );

      virtual bool                 hasFaceNormals( ) const;
      virtual typename Tuples<T>::P getFaceNormals( ) const;
      virtual void                 setFaceNormals( typename AWT::Tuples<T>::P in_VertexNormals );

      virtual void                 prepareToSearchVertices( bool forceRebuild = false );
      virtual void                 prepareToSearchFaces( bool forceRebuild = false );

      virtual void                 search( typename MeshSearch<T>::P searcher );

      virtual void                 searchVertices( typename VerticesSearch<T>::P searcher );
      virtual void                 searchFaces( typename FacesSearch<T>::P searcher );

      virtual ModifiedTime         getModifiedTime( ) const;

      virtual void                 lock( void* obj );
      virtual void                 unlock( void* obj );

      virtual std::string          getClassName( ) const;

      virtual typename MeshData<T>::P getMeshData( );

   protected:
      static T                     normalize( T* vec );
      void                         getFaceNormalUnnormalized( const MeshIndex in_Index, T* out_Normal ) const;

   protected:
      struct D;
      D* m_D;
   };
}

#endif // __MESHIMPL_H__