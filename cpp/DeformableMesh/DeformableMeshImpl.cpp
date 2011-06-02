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
#include "DeformableMeshImpl.h"

#include "Mesh/Tuples.h"
#include "Mesh/MeshImpl.h"
#include "Mesh/MeshData.h"
#include "AddTuples.h"

#include "Useful/PrintMacros.h"

#include <algorithm>


template <class T>
struct AWT::DeformableMeshImpl<T>::D
{
   void addModeInternal( typename Tuples<T>::P tuples, T weight )
   {
      m_Tuples.push_back( tuples );

      m_AddTuples->insert( tuples, weight );
   }

   typename Tuples<T>::P getModeInternal( const MeshIndex m )
   {
      return m_Tuples[m];
   }

   void setModeWeightImpl(const AWT::MeshIndex m, const T weight, const bool updateMesh)
   {
      m_AddTuples->setScale( m, weight );

      if ( updateMesh )
         updateMeshImpl( );
   }

   void updateMeshImpl( )
   {
      T vtx[3];
      //for ( MeshIndex v = 0; v < m_Mesh->getNumberOfVertices( ); ++v )
      MESH_EACHVERTEX( m_Mesh, v )
      {
         m_AddTuples->getPoint( v, vtx );
         m_Mesh->setVertex( v, vtx );
      }
   }

   typename Mesh<T>::P                m_Mesh;
   typename AddTuples<T>::P           m_AddTuples;

   typename MeshData<T>::P            m_MeshData;

   std::vector<typename Tuples<T>::P> m_Tuples;

   MeshIndex m_ActiveModes;
};

template <class T>
AWT::DeformableMeshImpl<T>::DeformableMeshImpl( typename AWT::Mesh<T>::P mesh )
{
   m_D = new D;

   m_D->m_MeshData = MeshData<T>::getInstance( this );

   // Take a copy of the mesh
   m_D->m_Mesh = MeshImpl<T>::getInstance( typename AWT::Mesh<T>::P( mesh ) );
   
   m_D->m_AddTuples = AddTuples<T>::getInstance( 3 );

   addMode( mesh->getVertices( ) );
   m_D->m_ActiveModes = 1;
}

template <class T>
AWT::DeformableMeshImpl<T>::~DeformableMeshImpl( )
{
   delete m_D;
}

template <class T>
typename AWT::DeformableMeshImpl<T>::P AWT::DeformableMeshImpl<T>::getInstance( typename AWT::Mesh<T>::P mesh )
{
   AUTOGETINSTANCE( AWT::DeformableMeshImpl<T>, ( mesh ) );
}

template <class T>
AWT::ModifiedTime AWT::DeformableMeshImpl<T>::getModifiedTime( ) const
{
   return std::max<ModifiedTime>( DeformableMesh<T>::getModifiedTime( ), m_D->m_AddTuples->getModifiedTime( ) );
}

template <class T>
AWT::MeshIndex AWT::DeformableMeshImpl<T>::getNumberOfActiveModes( ) const
{
   return std::min( m_D->m_ActiveModes, m_D->m_AddTuples->getNumberOfMembers( ) );
}

template <class T>
void AWT::DeformableMeshImpl<T>::setActiveModes( const AWT::MeshIndex m )
{
   m_D->m_ActiveModes = std::min<MeshIndex>( m, m_D->m_AddTuples->getNumberOfMembers( ) );
}

template <class T>
void AWT::DeformableMeshImpl<T>::activateAllModes( )
{
   m_D->m_ActiveModes = m_D->m_AddTuples->getNumberOfMembers( );
}


template <class T>
void AWT::DeformableMeshImpl<T>::setModeWeight(const AWT::MeshIndex m, const T weight)
{
   m_D->setModeWeightImpl( m, weight, true );
   modified( );
}

template <class T>
T AWT::DeformableMeshImpl<T>::getModeWeight(const AWT::MeshIndex m) const
{
   return m_D->m_AddTuples->getScale( m );
}

template <class T>
void AWT::DeformableMeshImpl<T>::setModeWeights( const T* weights )
{
   for ( unsigned int i = 0; i < getNumberOfActiveModes( ); ++i )
      m_D->setModeWeightImpl( i, weights[i], false );

   m_D->updateMeshImpl( );
   modified( );
}

template <class T>
void AWT::DeformableMeshImpl<T>::getModeWeights( T* weights ) const
{
   for ( unsigned int i = 0; i < getNumberOfActiveModes( ); ++i )
      weights[i] = getModeWeight( i );
}

template <class T>
void AWT::DeformableMeshImpl<T>::addMode( typename AWT::Tuples<T>::P tuples )
{
   T weight;

   if ( getNumberOfActiveModes( ) == 0 )
   {
      weight = 1;
   }
   else
   {
      weight = 0;
   }

   m_D->addModeInternal( tuples, weight );
}

template <class T>
typename AWT::Tuples<T>::P AWT::DeformableMeshImpl<T>::getMode( const AWT::MeshIndex m )
{
   return m_D->getModeInternal( m );
}

template <class T>
AWT::MeshIndex AWT::DeformableMeshImpl<T>::getNumberOfVertices( ) const
{
   return m_D->m_Mesh->getNumberOfVertices( );
}

template <class T>
typename AWT::Tuples<T>::P AWT::DeformableMeshImpl<T>::getVertices( ) const
{
   return m_D->m_Mesh->getVertices( );
}

template <class T>
void AWT::DeformableMeshImpl<T>::setVertices(typename AWT::Tuples<T>::P)
{
   AWTEXCEPTIONTHROW( "Not supported" );
}

template <class T>
typename AWT::Tuples<AWT::MeshIndex>::P AWT::DeformableMeshImpl<T>::getFaces( ) const
{
   return m_D->m_Mesh->getFaces( );
}

template <class T>
GETNAMEMACRO( AWT::DeformableMeshImpl<T> );

template <class T>
void AWT::DeformableMeshImpl<T>::lock( void* obj )
{
}

template <class T>
void AWT::DeformableMeshImpl<T>::unlock( void* obj )
{
}

template <class T>
void AWT::DeformableMeshImpl<T>::setFaces( typename AWT::Tuples<MeshIndex>::P faces )
{
   m_D->m_Mesh->setFaces( faces );
}

template <class T>
void AWT::DeformableMeshImpl<T>::getVertex(const AWT::MeshIndex v, T* vtx) const
{
   m_D->m_Mesh->getVertex( v, vtx );
}

template <class T>
void AWT::DeformableMeshImpl<T>::setVertex(const AWT::MeshIndex, const T, const T, const T)
{
   AWTEXCEPTIONTHROW( "Not supported" );
}

template <class T>
void AWT::DeformableMeshImpl<T>::setVertex(const AWT::MeshIndex, const T*)
{
   AWTEXCEPTIONTHROW( "Not supported" );
}

template <class T>
bool AWT::DeformableMeshImpl<T>::hasFaceNormals( ) const
{
   return m_D->m_Mesh->hasFaceNormals( );
}

template <class T>
bool AWT::DeformableMeshImpl<T>::hasVertexNormals( ) const
{
   return m_D->m_Mesh->hasVertexNormals( );
}

template <class T>
bool AWT::DeformableMeshImpl<T>::hasTextureCoords( ) const
{
   return m_D->m_Mesh->hasTextureCoords( );
}

template <class T>
typename AWT::Tuples<T>::P AWT::DeformableMeshImpl<T>::getTextureCoords( ) const
{
   return m_D->m_Mesh->getTextureCoords( );
}

template <class T>
void AWT::DeformableMeshImpl<T>::setTextureCoords( typename Tuples<T>::P in_TextureCoords )
{
   m_D->m_Mesh->setTextureCoords( in_TextureCoords );
}


template <class T>
typename AWT::Tuples<T>::P AWT::DeformableMeshImpl<T>::getVertexNormals( ) const
{
   return m_D->m_Mesh->getVertexNormals( );
}

template <class T>
void AWT::DeformableMeshImpl<T>::setVertexNormals( typename AWT::Tuples<T>::P v )
{
   m_D->m_Mesh->setVertexNormals( v );
}

template <class T>
typename AWT::Tuples<T>::P AWT::DeformableMeshImpl<T>::getFaceNormals( ) const
{
   return m_D->m_Mesh->getFaceNormals( );
}

template <class T>
void AWT::DeformableMeshImpl<T>::setFaceNormals( typename AWT::Tuples<T>::P v )
{
   m_D->m_Mesh->setFaceNormals( v );
}

template <class T>
bool AWT::DeformableMeshImpl<T>::hasFaces( ) const
{
   return m_D->m_Mesh->hasFaces( );
}

template <class T>
AWT::MeshIndex AWT::DeformableMeshImpl<T>::getNumberOfFaces( ) const
{
   return m_D->m_Mesh->getNumberOfFaces( );
}

template <class T>
void AWT::DeformableMeshImpl<T>::getFace(const AWT::MeshIndex v, T* x, T* y, T* z) const
{
   m_D->m_Mesh->getFace( v, x, y, z );
}

template <class T>
void AWT::DeformableMeshImpl<T>::getFaceIndices(const AWT::MeshIndex v, AWT::MeshIndex* vs) const
{
   m_D->m_Mesh->getFaceIndices( v, vs );
}

template <class T>
void AWT::DeformableMeshImpl<T>::setFaceIndices(const AWT::MeshIndex v, const AWT::MeshIndex a, const AWT::MeshIndex b, const AWT::MeshIndex c)
{
   m_D->m_Mesh->setFaceIndices( v, a, b, c );
}

template <class T>
void AWT::DeformableMeshImpl<T>::setFaceIndices(const AWT::MeshIndex v, const AWT::MeshIndex* vs)
{
   m_D->m_Mesh->setFaceIndices( v, vs );
}

template <class T>
void AWT::DeformableMeshImpl<T>::prepareToSearchFaces( bool forceRebuild )
{
   m_D->m_Mesh->prepareToSearchFaces( forceRebuild );
}

template <class T>
void AWT::DeformableMeshImpl<T>::prepareToSearchVertices( bool forceRebuild )
{
   m_D->m_Mesh->prepareToSearchVertices( forceRebuild );
}

template <class T>
void AWT::DeformableMeshImpl<T>::search(typename AWT::MeshSearch<T>::P searcher)
{
   m_D->m_Mesh->search( searcher );
}

template <class T>
void AWT::DeformableMeshImpl<T>::searchVertices(typename AWT::VerticesSearch<T>::P searcher)
{
   m_D->m_Mesh->searchVertices( searcher );
}

template <class T>
void AWT::DeformableMeshImpl<T>::searchFaces(typename AWT::FacesSearch<T>::P searcher )
{
   m_D->m_Mesh->searchFaces( searcher );
}

template <class T>
void AWT::DeformableMeshImpl<T>::getJacobian(const AWT::MeshIndex v, T *pnt, vnl_matrix<T>& jac) const
{
   // Get the three vertices at the corners
   T pointA[3];
   T pointB[3];
   T pointC[3];
   MeshIndex vtxIndex[3];

   m_D->m_Mesh->getFaceIndices( v, vtxIndex );
   m_D->m_Mesh->getFace( v, pointA, pointB, pointC );

   T det = pointA[0]*pointB[1]*pointC[2]-pointA[0]*pointC[1]*pointB[2]-pointA[1]*pointB[0]*pointC[2]+pointA[1]*pointC[0]*pointB[2]+pointA[2]*pointB[0]*pointC[1]-pointA[2]*pointC[0]*pointB[1];

   if ( det == 0 )
   {
      jac.fill( 0 );
      return;
   }

   T weights[3];
   weights[0] = ( ( pointB[1]*pointC[2]-pointC[1]*pointB[2])*pnt[0] + (-pointB[0]*pointC[2]+pointC[0]*pointB[2])*pnt[1] + ( pointB[0]*pointC[1]-pointC[0]*pointB[1])*pnt[2] ) / det;
   weights[1] = ( (-pointA[1]*pointC[2]+pointC[1]*pointA[2])*pnt[0] + ( pointA[0]*pointC[2]-pointC[0]*pointA[2])*pnt[1] + (-pointA[0]*pointC[1]+pointC[0]*pointA[1])*pnt[2] ) / det;
   weights[2] = ( ( pointA[1]*pointB[2]-pointB[1]*pointA[2])*pnt[0] + (-pointA[0]*pointB[2]+pointB[0]*pointA[2])*pnt[1] + ( pointA[0]*pointB[1]-pointB[0]*pointA[1])*pnt[2] ) / det;

   // Should add up to 1, but let's check anyhoo
   T weightNorm = 1 / ( weights[0] + weights[1] + weights[2] );

   weights[0] *= weightNorm;
   weights[1] *= weightNorm;
   weights[2] *= weightNorm;

   T tmp[3];

   for ( unsigned int c = 0; c < getNumberOfActiveModes( ); ++c )
   {
      // Reset, because we're going to take the weight average
      jac[0][c] = jac[1][c] = jac[2][c] = 0;

      for ( unsigned int cc = 0; cc < 3; ++cc )
      {
         m_D->m_Tuples[c]->getPoint( vtxIndex[cc], tmp );

         jac( 0, c ) += tmp[0] * weights[cc];
         jac( 1, c ) += tmp[1] * weights[cc];
         jac( 2, c ) += tmp[2] * weights[cc];
      }
   }
}

template <class T>
typename AWT::MeshData<T>::P AWT::DeformableMeshImpl<T>::getMeshData( )
{
   return m_D->m_MeshData;
}

template class AWT::DeformableMeshImpl<double>;
template class AWT::DeformableMeshImpl<float>;
