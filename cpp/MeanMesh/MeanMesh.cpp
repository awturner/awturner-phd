#include "Mesh/Mesh.h"
#include "Mesh/VTKMeshLoader.h"
#include "Mesh/VTKMeshWriter.h"

using namespace AWT;

typedef double T;

int main( int argc, char** argv )
{
   Mesh<T>::P mesh = VTKMeshLoader<T>::load( argv[2], false );

   T vtx[3];
   T vtxAdd[3];

   unsigned int meshCount = 1;

   for ( int i = 3; i < argc; ++i )
   {
      DEBUGMACRO( argv[i] );
      Mesh<T>::P addMesh = VTKMeshLoader<T>::load( argv[i], false );

      if ( addMesh->getNumberOfVertices() != mesh->getNumberOfVertices() )
      {
         PRINTVBL( addMesh->getNumberOfVertices() );
         PRINTVBL( mesh->getNumberOfVertices() );

         DEBUGMACRO( "Meshes are incompatible!" );
         AWTEXCEPTIONTHROW( "Meshes are incompatible!" );
      }

      DEBUGMACRO( "Skipping connectivity check" );

      MESH_EACHVERTEX( mesh, v )
      {
         mesh->getVertex( v, vtx );
         addMesh->getVertex( v, vtxAdd );

         FOREACHAXIS( ax )
            vtx[ax] += vtxAdd[ax];

         mesh->setVertex( v, vtx );
      }
      ++meshCount;

      BLANKLINE;
   }

   DEBUGMACRO( "Dividing through..." );
   MESH_EACHVERTEX( mesh, v )
   {
      mesh->getVertex( v, vtx );

      FOREACHAXIS( ax )
         vtx[ax] /= meshCount;

      mesh->setVertex( v, vtx );
   }

   DEBUGMACRO( "Writing mean mesh to " << argv[1] );
   VTKMeshWriter<T>::write( mesh, argv[1] );

   DEBUGMACRO( "Done." );
}