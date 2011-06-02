#include "SimpleMesh/Mesh.h"
#include "SimpleMesh/MeshIO.h"
#include "SimpleMesh/MeshFunctions.h"

#include <fstream>

using namespace AWT::SimpleMesh;

int main( int argc, char** argv )
{
   Mesh::P mesh = MeshIO::loadMesh( std::ifstream( argv[1] ) );

   std::cout << "nv " << mesh->nv << std::endl;
   std::cout << "nf " << mesh->nf << std::endl;
   std::cout << "centroid " << calculateCentroid(mesh) << std::endl;
   std::cout << "sa " << calculateSurfaceArea(mesh) << std::endl;
}