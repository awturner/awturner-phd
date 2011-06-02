#include "Mesh/MeshGenerator.h"
#include "Mesh/MeshImpl.h"
#include "Mesh/VTKMeshWriter.h"

using namespace AWT;

typedef double T;

int main( int argc, char** argv )
{
   MeshImpl<T>::P mesh = MeshImpl<T>::getInstance( 0, 0 );
   MeshGenerator<T>::generateTorus( mesh, atof( argv[1] ), atof( argv[2] ), atoi( argv[3] ), atoi( argv[4] ) );

   VTKMeshWriter<T>::write( mesh, argv[5] );

   return 0;
}