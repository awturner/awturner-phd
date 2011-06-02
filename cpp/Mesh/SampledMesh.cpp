/*
* ################################################################################
* ### MIT License
* ################################################################################
* 
* Copyright (c) 2006-2011 Andy Turner
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/
#include "SampledMesh.h"

#include "MeshImpl.h"
#include "SamplePoints.h"
#include "SampledTuples.h"

#include "SampledMeshData.h"

#include "ObjectEKDTree/OEKDTree.h"

#include "MeshSearching/VerticesTreeData.h"
#include "MeshSearching/VerticesSearch.h"
#include "MeshSearching/FacesSearch.h"



template <class T>
struct AWT::SampledMesh<T>::D
{
   SamplePoints::P                             m_SamplePoints;
   typename Mesh<T>::P                         m_Mesh;
   typename SampledTuples<T>::P                m_Vertices;
   typename SampledTuples<T>::P                m_VertexNormals;
   typename SampledTuples<T>::P                m_TextureCoords;

   typename Tuples<T>::P                       m_MeshVertexNormals;
   typename Tuples<T>::P                       m_MeshTextureCoords;

   typename OEKDTree::OEKDTree<T,3>::P         m_VerticesTree;
   typename VerticesTreeData<T>::P             m_VerticesTreeData;

   typename MeshData<T>::P                     m_MeshData;
};


template <class T>
AWT::SampledMesh<T>::SampledMesh( typename Mesh<T>::P mesh, SamplePoints::P sampPts )
{
   m_D = new D;

   m_D->m_MeshData = SampledMeshData<T>::getInstance( mesh->getMeshData( ), sampPts );

   m_D->m_SamplePoints = sampPts;
   m_D->m_Mesh = mesh;

   m_D->m_Vertices = SampledTuples<T>::getInstance( mesh->getVertices( ), sampPts );

   m_D->m_MeshVertexNormals = SampledTuples<T>::getInstance( mesh->getVertexNormals( ), sampPts );
   m_D->m_MeshTextureCoords = SampledTuples<T>::getInstance( mesh->getTextureCoords( ), sampPts );

   m_D->m_VerticesTree.set( 0 );

   m_D->m_VerticesTreeData = VerticesTreeData<T>::getInstance( *m_D->m_Vertices );

   if ( *m_D->m_MeshVertexNormals )
   {
      m_D->m_VertexNormals = SampledTuples<T>::getInstance( *m_D->m_MeshVertexNormals, sampPts );
   }
   else
   {
      m_D->m_VertexNormals.set( 0 );
   }

   if ( *m_D->m_MeshTextureCoords )
   {
      m_D->m_TextureCoords = SampledTuples<T>::getInstance( *m_D->m_MeshTextureCoords, sampPts );
   }
   else
   {
      m_D->m_TextureCoords.set( 0 );
   }
}

template <class T>
AWT::SampledMesh<T>::~SampledMesh( )
{
   delete m_D;
}

template <class T>
typename AWT::SampledMesh<T>::P AWT::SampledMesh<T>::getInstance( typename Mesh<T>::P mesh, SamplePoints::P sampPts )
{
   AUTOGETINSTANCE( SampledMesh<T>, ( mesh, sampPts ) );
}

template <class T>
GETNAMEMACRO( AWT::SampledMesh<T> );

template <class T>
void AWT::SampledMesh<T>::setVertices( typename AWT::Tuples<T>::P /*verts*/ )
{
}

template <class T>
typename AWT::Tuples<T>::P AWT::SampledMesh<T>::getVertices( ) const
{
   return m_D->m_Vertices;
}

#include "Useful/PrintMacros.h"

template <class T>
void AWT::SampledMesh<T>::getVertex( const MeshIndex in_Index, T* out_Vertex ) const
{
   m_D->m_Mesh->getVertex( m_D->m_SamplePoints->getSampleIndex( in_Index ), out_Vertex );
}

template <class T>
void AWT::SampledMesh<T>::setVertex( const MeshIndex in_Index, const T* in_Vertex )
{
}

template <class T>
void AWT::SampledMesh<T>::setVertex( const MeshIndex in_Index, const T a, const T b, const T c )
{
}

template <class T>
void AWT::SampledMesh<T>::setVertexNormals( typename AWT::Tuples<T>::P /*in_VertexNormals*/ )
{
}

template <class T>
void AWT::SampledMesh<T>::setTextureCoords( typename AWT::Tuples<T>::P /*in_VertexNormals*/ )
{
}

template <class T>
bool AWT::SampledMesh<T>::hasFaces( ) const
{
   return false;
}

template <class T>
AWT::MeshIndex AWT::SampledMesh<T>::getNumberOfFaces( ) const
{
   return 0;
}

template <class T>
typename AWT::Tuples<AWT::MeshIndex>::P AWT::SampledMesh<T>::getFaces( ) const
{
   return AWT::Tuples<MeshIndex>::P( );
}

template <class T>
void AWT::SampledMesh<T>::setFaces( typename AWT::Tuples<MeshIndex>::P )
{
   // Do nothing
}

template <class T>
void AWT::SampledMesh<T>::getFace( const AWT::MeshIndex /*in_Index*/, T* /*out_VertexA*/, T* /*out_VertexB*/, T* /*out_VertexC */) const
{
   AWTEXCEPTIONTHROW( "Not supported" );
}

template <class T>
void AWT::SampledMesh<T>::getFaceIndices( const AWT::MeshIndex /*in_Index*/, AWT::MeshIndex* /*out_VertexIndices*/ ) const
{
   AWTEXCEPTIONTHROW( "Not supported" );
}

template <class T>
void AWT::SampledMesh<T>::setFaceIndices( const MeshIndex /*in_Index*/, const MeshIndex /*a*/, const MeshIndex /*b*/, const MeshIndex /*c*/ )
{
   AWTEXCEPTIONTHROW( "Not supported" );
}

template <class T>
void AWT::SampledMesh<T>::setFaceIndices( const MeshIndex /*in_Index*/, const MeshIndex* /*in_VertexIndices*/ )
{
   AWTEXCEPTIONTHROW( "Not supported" );
}

template <class T>
bool AWT::SampledMesh<T>::hasFaceNormals( ) const
{
   return false;
}

template <class T>
typename AWT::Tuples<T>::P AWT::SampledMesh<T>::getFaceNormals( ) const
{
   return AWT::Tuples<T>::P( );
}

template <class T>
void AWT::SampledMesh<T>::setFaceNormals( typename AWT::Tuples<T>::P /*in_VertexNormals*/ )
{
   AWTEXCEPTIONTHROW( "Not supported" );
}

template <class T>
void AWT::SampledMesh<T>::prepareToSearchFaces( bool forceRebuild )
{
   AWTEXCEPTIONTHROW( "Not supported" );
}

template <class T>
void AWT::SampledMesh<T>::searchFaces( typename AWT::FacesSearch<T>::P /*searcher*/ )
{
   AWTEXCEPTIONTHROW( "Not supported" );
}

template <class T>
void AWT::SampledMesh<T>::lock( void* obj )
{
   m_D->m_Mesh->lock( obj );
}

template <class T>
void AWT::SampledMesh<T>::unlock( void* obj )
{
   m_D->m_Mesh->unlock( obj );
}

template <class T>
AWT::MeshIndex AWT::SampledMesh<T>::getNumberOfVertices( ) const
{
   return m_D->m_SamplePoints->getNumberOfSamples( );
}

template <class T>
bool AWT::SampledMesh<T>::hasVertexNormals( ) const
{
   getVertexNormals( );

   return m_D->m_Mesh->hasVertexNormals( );
}

template <class T>
typename AWT::Tuples<T>::P AWT::SampledMesh<T>::getVertexNormals( ) const
{
   Tuples<T>::P meshVertexNormals = m_D->m_Mesh->getVertexNormals( );

   if ( *m_D->m_MeshVertexNormals != *meshVertexNormals )
   {
      if ( *meshVertexNormals != 0 )
      {
         m_D->m_MeshVertexNormals = SampledTuples<T>::getInstance( m_D->m_Mesh->getVertexNormals( ), *m_D->m_SamplePoints );
      }
      else
      {
         m_D->m_MeshVertexNormals.set( 0 );
      }
   }

   return m_D->m_MeshVertexNormals;
}

template <class T>
bool AWT::SampledMesh<T>::hasTextureCoords( ) const
{
   getTextureCoords( );

   return m_D->m_Mesh->hasTextureCoords( );
}

template <class T>
typename AWT::Tuples<T>::P AWT::SampledMesh<T>::getTextureCoords( ) const
{
   Tuples<T>::P meshTextureCoords = m_D->m_Mesh->getTextureCoords( );

   if ( *m_D->m_MeshTextureCoords != *meshTextureCoords )
   {
      if ( *meshTextureCoords != 0 )
      {
         m_D->m_MeshTextureCoords = SampledTuples<T>::getInstance( m_D->m_Mesh->getTextureCoords( ), *m_D->m_SamplePoints );
      }
      else
      {
         m_D->m_MeshTextureCoords.set( 0 );
      }
   }

   return m_D->m_MeshTextureCoords;
}

template <class T>
void AWT::SampledMesh<T>::prepareToSearchVertices( bool forceRebuild )
{
   if ( *m_D->m_VerticesTree == 0 || forceRebuild )
   {
      m_D->m_VerticesTree = OEKDTree::OEKDTree<T,3>::getInstance( m_D->m_VerticesTreeData );
   }
   
   if ( m_D->m_VerticesTree->getModifiedTime( ) < m_D->m_Vertices->getModifiedTime( ) || forceRebuild )
   {
      m_D->m_VerticesTree->build( );
   }
}

template <class T>
void AWT::SampledMesh<T>::search( typename AWT::MeshSearch<T>::P searcher )
{
   if ( dynamic_cast<AWT::VerticesSearch<T>*>( *searcher ) != 0 )
      searchVertices( dynamic_cast<VerticesSearch<T>*>( *searcher ) );
   if ( dynamic_cast<AWT::FacesSearch<T>*>( *searcher ) != 0 )
      searchFaces( dynamic_cast<FacesSearch<T>*>( *searcher ) );
}

template <class T>
void AWT::SampledMesh<T>::searchVertices( typename AWT::VerticesSearch<T>::P search )
{
}

template <class T>
typename AWT::MeshData<T>::P AWT::SampledMesh<T>::getMeshData( )
{
   return m_D->m_MeshData;
}

template class AWT::SampledMesh<double>;
template class AWT::SampledMesh<float>;