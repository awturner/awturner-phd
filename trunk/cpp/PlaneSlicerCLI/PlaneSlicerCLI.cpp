#include "DeformableMesh/DeformableMeshLoader.h"

#include "Mesh/VTKMeshLoader.h"
#include "Mesh/VTKMeshWriter.h"

#include "Useful/PrintMacros.h"
#include "Useful/Noise.h"

#include "MeshSlicer/MeshSlicer.h"
#include "MeshSlicer/PlanarCut.h"

#include <algorithm>

using namespace AWT;
typedef double T;

int main( int argc, char** argv )
{
   if ( argc < 8 )
   {
      std::cerr << "Must provide at least input, 2 output filenames and plane normal";
      exit( 1 );
   }

   Mesh<T>::P mesh = VTKMeshLoader<T>::load( argv[1], false );
   PlanarCut<T>::P plane = PlanarCut<T>::getInstance( );
   MeshSlicer<T>::P slicer = MeshSlicer<T>::getInstance( );

   T planeNormal[4];
   for ( int i = 0; i < 4; ++i )
      planeNormal[i] = atof( argv[i+4] );

   PRINTVEC( planeNormal, 4 );

   plane->setPlane( planeNormal );

   slicer->setCuttingFunction( plane );

   MeshSlicedPair<T>::P sliced = slicer->sliceMesh( mesh );

   VTKMeshWriter<T>::write( sliced->getIncluded( ), argv[2] );
   VTKMeshWriter<T>::write( sliced->getExcluded( ), argv[3] );
}