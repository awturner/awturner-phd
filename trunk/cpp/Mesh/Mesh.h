#ifndef __SURFACEMESH_H__
#define __SURFACEMESH_H__

#include "ObjectManagement/ManagedObject.h"
#include "MeshTypeDefs.h"
#include "MeshData.h"

#include "Tuples.h"

#include "MeshSearching/MeshSearch.h"
#include "MeshSearching/VerticesSearch.h"
#include "MeshSearching/FacesSearch.h"

#include "vnl/vnl_matrix_fixed.h"
#include "vnl/vnl_vector_fixed.h"
#include "vnl/vnl_det.h"

namespace AWT
{
   typedef unsigned int VertexUsedCount;

   namespace OEKDTree
   {
      template <class T, unsigned char K>
      class OEKDTree;

      template <class T, unsigned char K>
      class OEKDSearchAgent;
   }

   template <class T>
   class MeshSearch;

   template <class T>
   class VerticesSearch;

   template <class T>
   class FacesSearch;

   template <class T>
   class Mesh : public virtual ManagedObject
   {
   public:
      typedef ManagedAutoPointer<Mesh<T>> P;

   public:
      virtual MeshIndex                     getNumberOfVertices( ) const = 0;
      
      virtual typename Tuples<T>::P         getVertices( ) const = 0;
      virtual void                          setVertices( typename AWT::Tuples<T>::P in_Vertices ) = 0;

      virtual typename Tuples<MeshIndex>::P getFaces( ) const = 0;
      virtual void                          setFaces( typename AWT::Tuples<MeshIndex>::P in_Vertices ) = 0;

      virtual void                          getVertex( const MeshIndex in_Index, T* out_Vertex ) const = 0;

      virtual void                          setVertex( const MeshIndex in_Index, const T x, const T y, const T z ) = 0;
      virtual void                          setVertex( const MeshIndex in_Index, const T* in_Vertex ) = 0;

      virtual bool                          hasVertexNormals( ) const = 0;
      virtual typename Tuples<T>::P         getVertexNormals( ) const = 0;
      virtual void                          setVertexNormals( typename AWT::Tuples<T>::P in_VertexNormals ) = 0;

      virtual bool                          hasTextureCoords( ) const = 0;
      virtual typename Tuples<T>::P         getTextureCoords( ) const = 0;
      virtual void                          setTextureCoords( typename AWT::Tuples<T>::P in_TexCoords ) = 0;

      virtual bool                          hasFaces( ) const = 0;
      virtual MeshIndex                     getNumberOfFaces( ) const = 0;

      virtual void                          getFace( const MeshIndex in_Index, T* out_VertexA, T* out_VertexB, T* out_VertexC ) const = 0;
      
      virtual void                          getFaceIndices( const MeshIndex in_Index, MeshIndex* out_VertexIndices ) const = 0;

      virtual void                          setFaceIndices( const MeshIndex in_Index, const MeshIndex a, const MeshIndex b, const MeshIndex c ) = 0;
      virtual void                          setFaceIndices( const MeshIndex in_Index, const MeshIndex* in_VertexIndices ) = 0;

      virtual bool                          hasFaceNormals( ) const = 0;
      virtual typename Tuples<T>::P         getFaceNormals( ) const = 0;
      virtual void                          setFaceNormals( typename AWT::Tuples<T>::P in_VertexNormals ) = 0;

      virtual void                          prepareToSearchVertices( bool forceRebuild = false ) = 0;
      virtual void                          prepareToSearchFaces( bool forceRebuild = false ) = 0;

      virtual void                          search( typename MeshSearch<T>::P searcher ) = 0;
      virtual void                          searchVertices( typename VerticesSearch<T>::P searcher ) = 0;
      virtual void                          searchFaces( typename FacesSearch<T>::P searcher ) = 0;

      virtual typename MeshData<T>::P       getMeshData( ) = 0;

      virtual void                          lock( void* obj ) = 0;
      virtual void                          unlock( void* obj ) = 0;
   };
}

#define FOREACHAXIS( ax ) for ( unsigned int ax = 0; ax < 3; ++ax )
#define MESH_EACHVERTEX( mesh, var ) for ( AWT::MeshIndex var = 0, max##var = mesh->getNumberOfVertices( ); var < max##var; ++var )
#define MESH_EACHFACE( mesh, var ) for ( AWT::MeshIndex var = 0, max##var = mesh->getNumberOfFaces( ); var < max##var; ++var )

#endif // __SURFACEMESH_H__