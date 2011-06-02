/*! \file
\brief Calculates Moghari's attribute vector (2nd Order)

Takes a mesh file and calculates Moghari's 9-point attribute vector
for each mesh vertex, as described in "Global registration of multiple 
bone fragments using statistical atlas models: feasibility experiments.", 
2008, Moghari MH, Abolmaesumi P.

The attribute vector is written to standard output.

*/

#include "Mesh/VTKMeshLoader.h"

#include "Mesh/Mesh.h"
#include "Mesh/Tuples.h"
#include "DeformableMeshFitting/MoghariMeasure.h"

using namespace AWT;

typedef double T;

int main( int argc, char** argv )
{
   if ( argc < 3 )
   {
      std::cerr << "Usage: CalculateMoghari <input model filename> <radius>" << std::endl;
      DEBUGLINEANDEXIT( 1 );
   }

   T radius = static_cast<T>( atof( argv[2] ) );

   Mesh<T>::P           mesh    = VTKMeshLoader<T>::load( argv[1] );
   MoghariMeasure<T>::P mogMeas = MoghariMeasure<T>::getInstance( );
   
   Tuples<T>::P         as      = mogMeas->calculate( mesh, radius );

   T tuple[9];
   for ( MeshIndex v = 0; v < as->getNumberOfPoints( ); ++v )
   {
      as->getPoint( v, tuple );

      for ( int i = 0; i < 9; ++i )
         std::cout << tuple[i] << " ";

      std::cout << std::endl;
   }
}