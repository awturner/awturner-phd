#include "MeshBoundaryFinder.h"

#include "Mesh/MeshConnectivity.h"
#include "Mesh/TuplesImpl.h"

template <class T>
typename AWT::Tuples<T>::P AWT::MeshBoundaryFinder<T>::findBoundaryVertices( typename AWT::Mesh<T>::P mesh )
{
      MeshConnectivity<T>::P conn = MeshConnectivity<T>::getInstance( mesh );

      MeshIndex fs[3];
      MeshIndex vs[3];
      T vtx[3];

      Tuples<T>::P borderPoints = TuplesImpl<T>::getInstance( 3, 1000 );

      MESH_EACHFACE( mesh, f )
      {
         if ( conn->getAdjacentFaces( f, fs ) != 3 )
         {
            mesh->getFaceIndices( f, vs );

            for ( int i = 0; i < 3; ++i )
            {
               if ( conn->isVertexOnBoundary( vs[i] ) && conn->isVertexOnBoundary( vs[(i+1)%3] ) )
               {
                  DEBUGMACRO( (vs[i]) << " " << (vs[(i+1)%3]) );

                  mesh->getVertex( vs[i], vtx );
                  borderPoints->addPoint( vtx );

                  mesh->getVertex( vs[(i+1)%3], vtx );
                  borderPoints->addPoint( vtx );
               }
            }
         }
      }

      return borderPoints;
}

template class AWT::MeshBoundaryFinder<double>;
template class AWT::MeshBoundaryFinder<float>;