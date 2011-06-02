#include "DeformableMesh/DeformableMeshLoader.h"

#include "Mesh/VTKMeshWriter.h"

#include "Useful/PrintMacros.h"
#include "Useful/Noise.h"

#include <algorithm>

using namespace AWT;
typedef double T;

int main( int argc, char** argv )
{
   if ( argc < 4 )
   {
      std::cerr << "Must provide at least input and output filenames, and number of modes to perturb";
      exit( 1 );
   }

   DeformableMesh<T>::P mesh = DeformableMeshLoader<T>::load( argv[1] );

   std::cerr << "Model has " << mesh->getNumberOfActiveModes( ) << " modes." << std::endl;

   const unsigned int totalModes   = mesh->getNumberOfActiveModes( );
   const unsigned int nmodesToVary = std::min<unsigned int>( atoi( argv[3] ), totalModes - 1 ) ;

   T* modeWeights = new T[ totalModes ];
   for ( unsigned int i = 0; i < totalModes; ++i )
      modeWeights[i] = 0;

   modeWeights[0] = 1;

   if ( argc == 4 )
   {
      for ( unsigned int i = 0; i < nmodesToVary; ++i )
         modeWeights[ 1+i ] = Noise<T>::randu( -1, 1 );
   }
   else
   {
      for ( unsigned int i = 4; i < nmodesToVary+4 && i < argc; ++i )
         modeWeights[ i-4 ] = atof( argv[i] );
   }

   // Actually set the mode weights
   mesh->setModeWeights( modeWeights );

   PRINTVEC( modeWeights, totalModes );

   VTKMeshWriter<T>::write( mesh, argv[2] );
   DEBUGMACRO( "Saved to " << argv[2] );
}