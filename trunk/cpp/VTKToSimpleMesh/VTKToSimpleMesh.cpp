#include "Mesh/VTKMeshLoader.h"

#include <fstream>

using namespace AWT;

int main( int argc, char** argv )
{
   for ( int i = 1; i < argc; ++i )
   {
      Mesh<double>::P mesh = VTKMeshLoader<double>::load( argv[i], false );

      char buffer[256];
      sprintf_s( buffer, "%s.smesh", argv[i] );

      std::ofstream os( buffer );

      // Write the vertices
      os << "v" "\t" << mesh->getNumberOfVertices() << std::endl;

      double vtx[3];
      MESH_EACHVERTEX( mesh, v )
      {
         mesh->getVertex( v, vtx );

         os << vtx[0] << "\t" << vtx[1] << "\t" << "\t" << vtx[2] << "\n";
      }
      
      // Write the faces
      os << "f" "\t" << mesh->getNumberOfFaces() << std::endl;

      MeshIndex fac[3];
      MESH_EACHFACE( mesh, f )
      {
         mesh->getFaceIndices( f, fac);

         os << fac[0] << "\t" << fac[1] << "\t" << "\t" << fac[2] << "\n";
      }

      if (mesh->hasTextureCoords())
      {
         Tuples<double>::P tex = mesh->getTextureCoords();

         os << "t" "\t" << tex->getNumberOfPoints() << "\n";

         double vtx[3];
         MESH_EACHVERTEX( mesh, v )
         {
            tex->getPoint(v,vtx);

            os << vtx[0] << "\t" << vtx[1] << "\n";
         }
      }

      os << "e" << std::endl;
      os.close( );

      DEBUGMACRO( "Written " << argv[i] << " to " << buffer );
   }
}