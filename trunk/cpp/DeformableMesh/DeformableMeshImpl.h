#ifndef __DEFORMABLEMESHIMPL_H__
#define __DEFORMABLEMESHIMPL_H__

#include "DeformableMesh.h"
#include "vnl/vnl_matrix.h"

namespace AWT
{
   /*! \class DeformableMeshImpl
   \brief Implementation of DeformableMesh

   
   */

   template <class T>
   class DeformableMeshImpl : public DeformableMesh<T>
   {
   public:
      typedef ManagedAutoPointer< DeformableMeshImpl<T> > P;

   protected:
      DeformableMeshImpl( typename AWT::Mesh<T>::P mesh );
      ~DeformableMeshImpl( );

   public:
      static P getInstance( typename AWT::Mesh<T>::P mesh );

      virtual MeshIndex getNumberOfActiveModes( ) const;

      virtual void setActiveModes( const MeshIndex m );
      virtual void activateAllModes( );
      
      virtual void setModeWeight( const MeshIndex m, const T weight );

      virtual T getModeWeight( const MeshIndex m ) const;

      virtual void setModeWeights( const T* weights );

      virtual void getModeWeights( T* weights ) const;

      virtual void getJacobian( const MeshIndex v, T* pnt, vnl_matrix<T>& jac ) const;

      virtual void addMode( typename Tuples<T>::P tuples );

      virtual typename Tuples<T>::P getMode( const MeshIndex m );

      virtual MeshIndex getNumberOfVertices( ) const;

      virtual typename Tuples<T>::P getVertices( ) const;

      virtual void setVertices( typename Tuples<T>::P );

      virtual typename Tuples<MeshIndex>::P getFaces( ) const;

      virtual std::string getClassName( ) const;

      virtual void lock( void* obj );

      virtual void unlock( void* obj );

      virtual bool                  hasTextureCoords( ) const;
      virtual typename Tuples<T>::P getTextureCoords( ) const;
      virtual void                  setTextureCoords( typename AWT::Tuples<T>::P in_TexCoords );
      virtual void setFaces(typename Tuples<MeshIndex>::P);
      virtual void getVertex(const MeshIndex,T *) const;
      virtual void setVertex(const MeshIndex,const T *);
      virtual void setVertex(const MeshIndex,const T,const T,const T);
      virtual bool hasVertexNormals(void) const;
      virtual typename Tuples<T>::P getVertexNormals(void) const;
      virtual void setVertexNormals(typename Tuples<T>::P);
      virtual bool hasFaces(void) const;
      virtual MeshIndex getNumberOfFaces(void) const;
      virtual void getFace(const MeshIndex,T *,T *,T *) const;
      virtual void getFaceIndices(const MeshIndex,MeshIndex *) const;
      virtual void setFaceIndices(const MeshIndex,const MeshIndex *);
      virtual void setFaceIndices(const MeshIndex,const MeshIndex,const MeshIndex,const MeshIndex);
      virtual bool hasFaceNormals(void) const;
      virtual typename Tuples<T>::P getFaceNormals(void) const;
      virtual void setFaceNormals(typename Tuples<T>::P);

      virtual void prepareToSearchVertices(bool forceRebuild = false );
      virtual void prepareToSearchFaces(bool forceRebuild = false );
      virtual void search(typename MeshSearch<T>::P);
      virtual void searchVertices(typename VerticesSearch<T>::P);
      virtual void searchFaces(typename FacesSearch<T>::P);  

      virtual typename MeshData<T>::P getMeshData( );

      virtual ModifiedTime getModifiedTime( ) const;
   protected:
      struct D;
      D* m_D;
   };
}

#endif // __DEFORMABLEMESHIMPL_H__