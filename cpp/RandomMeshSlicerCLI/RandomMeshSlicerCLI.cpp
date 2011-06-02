#include "Mesh/Mesh.h"
#include "Mesh/VTKMeshLoader.h"
#include "Mesh/VTKMeshWriter.h"
#include "RandomMeshSlicer/RandomMeshSlicer.h"

#include <iostream>
#include <time.h>

#include "Useful/Noise.h"

using namespace AWT;

typedef double T;

int main( int argc, char** argv )
{
   if ( argc < 4 )
   {
      std::cerr << "Usage: RandomMeshSlicerCLI <mesh filename> <# slice planes> <printf for fragment filenames>" << std::endl;
      DEBUGLINEANDEXIT( 1 );
   }

   Noise<double>::timeSeed( );

   Mesh<T>::P mesh = VTKMeshLoader<T>::load( argv[1] );

   unsigned int nplanes = atoi( argv[2] );

   if ( nplanes > 5 )
   {
      std::cerr << "Too many planes! (n <= 5)" << std::endl;
      DEBUGLINEANDEXIT( 1 );
   }
   
   RandomMeshSlicer<T>::P rms = RandomMeshSlicer<T>::getInstance(mesh, nplanes);
   std::cerr << "Created " << rms->getNumberOfFragments( ) << " fragments." << std::endl;

   char buffer[80];
   for ( unsigned int i = 0; i < rms->getNumberOfFragments( ); ++i )
   {
      Mesh<T>::P frag = rms->getFragment( i );

      sprintf_s( buffer, argv[3], i );

      VTKMeshWriter<T>::write( frag, buffer );
   }
}