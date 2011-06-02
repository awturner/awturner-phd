#include "Mesh/VTKMeshLoader.h"
#include "Mesh/VTKMeshWriter.h"
#include "Mesh/TuplesFunctions.h"

#include "vnl/vnl_matrix.h"

using namespace AWT;

typedef double T;

int main( int argc, char** argv )
{
   vnl_matrix<T> theFlip(2,2);
   theFlip.set_identity( );

   if ( strcmp( argv[2], "X" ) == 0 )
      theFlip(0,0) = -1;
   else if ( strcmp( argv[2], "Y" ) == 0 )
      theFlip(1,1) = -1;
   else if ( strcmp( argv[2], "XY" ) == 0 )
      theFlip(0,0) = theFlip(1,1) = -1;
   else
   {
      DEBUGMACRO( "Unrecognised flip: must be X, Y or XY" );
      DEBUGLINEANDEXIT( 1 );
   }

   Mesh<T>::P mesh = VTKMeshLoader<T>::load( argv[1] );
   if ( !mesh->hasTextureCoords() )
   {
      DEBUGMACRO( "Mesh doesn't have texture coordinates to flip" );
      DEBUGLINEANDEXIT( 1 );
   }

   std::cerr << theFlip;
   T vtx[3];
   MESH_EACHVERTEX( mesh, v )
   {
      mesh->getTextureCoords()->getPoint( v, vtx );
      vtx[0] *= theFlip(0,0);
      vtx[1] *= theFlip(1,1);
      mesh->getTextureCoords()->setPoint( v, vtx );
   }
   mesh->setTextureCoords( TuplesFunctions<T>::transform( mesh->getTextureCoords(), theFlip ) );
   VTKMeshWriter<T>::write( mesh, argv[1] );
}