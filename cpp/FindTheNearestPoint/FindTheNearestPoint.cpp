#include "Mesh/VTKMeshLoader.h"
#include "MeshSearching/VerticesNearestVertexSearch.h"

using namespace AWT;

typedef double T;

int main( int argc, char** argv )
{
   Mesh<T>::P mesh = VTKMeshLoader<T>::load( argv[1] );

   VerticesNearestVertexSearch<T>::P searcher = VerticesNearestVertexSearch<T>::getInstance( );
   searcher->reset( );

   T vtxSearch[] = { atof(argv[2]), atof(argv[3]), atof(argv[4]) };
   searcher->setTestPoint( vtxSearch );

   mesh->searchVertices( searcher );

   T vtxFound[3];

   int idx = searcher->getNearestPoint( vtxFound );

   PRINTVBL( idx );
   PRINTVEC( vtxFound, 3 );
}