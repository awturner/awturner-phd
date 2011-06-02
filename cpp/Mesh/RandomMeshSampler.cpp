#include "RandomMeshSampler.h"

#include "Mesh.h"
#include "SamplePoints.h"

template <class T>
struct AWT::RandomMeshSampler<T>::D
{
   MeshIndex m_NumberOfSamples;
};

template <class T>
AWT::RandomMeshSampler<T>::RandomMeshSampler( )
{
   m_D = new D;

   m_D->m_NumberOfSamples = 1000;
}

template <class T>
AWT::RandomMeshSampler<T>::~RandomMeshSampler( )
{
   delete m_D;
}

template <class T>
GETNAMEMACRO( AWT::RandomMeshSampler<T> );

template <class T>
typename AWT::RandomMeshSampler<T>::P AWT::RandomMeshSampler<T>::getInstance( )
{
   AUTOGETINSTANCE( RandomMeshSampler<T>, ( ) );
}

template <class T>
AWT::SamplePoints::P AWT::RandomMeshSampler<T>::sampleMesh( typename Mesh<T>::P mesh )
{
   AWT::SamplePoints::P samplePoints;

   if ( mesh->getNumberOfVertices( ) < m_D->m_NumberOfSamples )
   {
      samplePoints = AWT::SamplePoints::getInstance( mesh->getNumberOfVertices( ) );
      //for ( MeshIndex i = 0; i < mesh->getNumberOfVertices( ); ++i )
      MESH_EACHVERTEX( mesh, i )
         samplePoints->add( i );
   }
   else
   {
      samplePoints = AWT::SamplePoints::getInstance( m_D->m_NumberOfSamples );
      bool* used = new bool[ mesh->getNumberOfVertices( ) ];
      //for ( MeshIndex i = 0; i < mesh->getNumberOfVertices( ); ++i )
      MESH_EACHVERTEX( mesh, i )
         used[i] = false;

      for ( MeshIndex i = 0; i < m_D->m_NumberOfSamples; ++i )
      {
         MeshIndex ii;
         do
         {
            ii = ( rand( ) | ( rand( ) << 16 ) ) % mesh->getNumberOfVertices( );
         } while ( used[ii] );

         samplePoints->add( ii );
         used[ii] = true;
      }

      delete[] used;
   }

   return samplePoints;
}

template <class T>
void AWT::RandomMeshSampler<T>::setMaximumNumberOfSamples( const AWT::MeshIndex m )
{
   m_D->m_NumberOfSamples = m;
}

template <class T>
AWT::MeshIndex AWT::RandomMeshSampler<T>::getMaximumNumberOfSamples( ) const
{
   return m_D->m_NumberOfSamples;
}

template class AWT::RandomMeshSampler<double>;
template class AWT::RandomMeshSampler<float>;