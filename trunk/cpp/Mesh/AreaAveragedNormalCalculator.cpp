#include "AreaAveragedNormalCalculator.h"

#include "Useful/Exception.h"
#include "Useful/ArrayVectorFunctions.h"

#include "Mesh.h"
#include "Tuples.h"

template <class T>
struct AWT::AreaAveragedNormalCalculator<T>::D
{
   static void getFaceNormalUnnormalized( const Mesh<T>* mesh, const AWT::MeshIndex in_Index, T* out_Normal )
   {
      T point[3][3];
      mesh->getFace( in_Index, &point[0][0], &point[1][0], &point[2][0] );

      // Get the directions of the edges relative to the zero-th point
      for ( MeshIndex j = 0; j < 3; ++j )
      {
         point[1][j] -= point[0][j];
         point[2][j] -= point[0][j];
      }

      cross( point[1], point[2], out_Normal );
   }
};

template <class T>
AWT::AreaAveragedNormalCalculator<T>::AreaAveragedNormalCalculator( )
{
   m_D = new D;
}

template <class T>
AWT::AreaAveragedNormalCalculator<T>::~AreaAveragedNormalCalculator( )
{
   delete m_D;
}

template <class T>
typename AWT::AreaAveragedNormalCalculator<T>::P AWT::AreaAveragedNormalCalculator<T>::getInstance( )
{
   AUTOGETINSTANCE( AWT::AreaAveragedNormalCalculator<T>, ( ) );
}

template <class T>
GETNAMEMACRO( AWT::AreaAveragedNormalCalculator<T> );

template <class T>
void AWT::AreaAveragedNormalCalculator<T>::calculateNormals( typename Mesh<T>::P mesh, typename Tuples<T>::P vtxNormals, typename Tuples<T>::P faceNormals )
{
   if ( !mesh->hasFaces( ) )
      AWTEXCEPTIONTHROW("Can't calculate normals without faces!");

   T zeroNormal[] = { 0, 0, 0 };

   vtxNormals->ensureSize( mesh->getNumberOfVertices( ) );
   faceNormals->ensureSize( mesh->getNumberOfFaces( ) );
   
   // Firstly, zero all of the vertex normals
   //for ( MeshIndex i = 0; i < mesh->getNumberOfVertices( ); ++i )
   MESH_EACHVERTEX( mesh, i )
      vtxNormals->setPoint( i, &zeroNormal[0] );

   // and all of the face normals
   //for ( MeshIndex i = 0; i < mesh->getNumberOfFaces( ); ++i )
   MESH_EACHFACE( mesh, f )
      faceNormals->setPoint( f, zeroNormal );

   T vertexNormals[3][3];
   MeshIndex vertexIndices[3];
   T faceNormal[3];

   // Now, go through and calculate all of the triangle cross products, and add this to
   // each of the vertices
   //for ( MeshIndex f = 0; f < mesh->getNumberOfFaces( ); ++f )
   MESH_EACHFACE( mesh, f )
   {
      mesh->getFaceIndices( f, &vertexIndices[0] );

      D::getFaceNormalUnnormalized( *mesh, f, faceNormal );
      //normalize( faceNormal );

      // Increment the current vertex normals by the UNNORMALIZED cross
      // product - this effectively weights the normals by the area of the face.
      // We will normalize it at the end
      vtxNormals->getPoint( vertexIndices[0], &vertexNormals[0][0] );
      vtxNormals->getPoint( vertexIndices[1], &vertexNormals[1][0] );
      vtxNormals->getPoint( vertexIndices[2], &vertexNormals[2][0] );

      for ( MeshIndex j = 0; j < 3; ++j )
      {
         vertexNormals[0][j] += faceNormal[j];
         vertexNormals[1][j] += faceNormal[j];
         vertexNormals[2][j] += faceNormal[j];
      }

      vtxNormals->setPoint( vertexIndices[0], vertexNormals[0] );
      vtxNormals->setPoint( vertexIndices[1], vertexNormals[1] );
      vtxNormals->setPoint( vertexIndices[2], vertexNormals[2] );

      if ( dot( faceNormal, faceNormal, 3 ) != 0 )
      {
         // Now we can set the face normal - but we want this one to be normalized
         normalize( faceNormal, 3 );
         faceNormals->setPoint( f, faceNormal );
      }
   }

   T point[3];
   // Finally, we can normalize the vertex normals
   //for ( MeshIndex i = 0; i < mesh->getNumberOfVertices( ); ++i )
   MESH_EACHVERTEX( mesh, i )
   {
      vtxNormals->getPoint( i, point );
      normalize( &point[0], 3 );
      vtxNormals->setPoint( i, point );
   }

}

template class AWT::AreaAveragedNormalCalculator<double>;
template class AWT::AreaAveragedNormalCalculator<float>;