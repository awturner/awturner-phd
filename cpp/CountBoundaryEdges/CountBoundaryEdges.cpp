#include "Mesh/Mesh.h"
#include "Mesh/MeshBoundaryFinder.h"

#include "Mesh/VTKMeshLoader.h"

using namespace AWT;
typedef double T;

int main( int argc, char** argv )
{
   for ( int i = 1; i < argc; ++i )
   {
      Mesh<T>::P mesh = VTKMeshLoader<T>::load( argv[i] );
      Tuples<T>::P borderPoints = MeshBoundaryFinder<T>::findBoundaryVertices( mesh );

      std::cout << argv[i] << "\t" << borderPoints->getNumberOfPoints( )/2 << std::endl;

      for ( MeshIndex v = 0, vmax = borderPoints->getNumberOfPoints( ); v < vmax; ++v )
      {
         T vtx[3];
         borderPoints->getPoint( v, vtx );
         std::cerr << "\t\t" << vtx[0] << " " << vtx[1] << " " << vtx[2] << std::endl;
      }
   }
}