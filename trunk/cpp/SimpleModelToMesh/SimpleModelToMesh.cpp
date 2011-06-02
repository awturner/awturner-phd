#include "SimpleMesh/Mesh.h"
#include "SimpleMesh/MeshIO.h"
#include "SimpleMesh/ShapeModel.h"

#include <fstream>

using namespace AWT::SimpleMesh;

void readVector( int& i, char** argv, Vector& params )
{
   for ( Index m = 0; m < params.size(); ++m )
   {
      const char* modeweightStr = argv[++i];

      if ( strcmp( modeweightStr, "-" ) == 0 )
         break;

      params[m] = atof( modeweightStr );
   }

   PRINTVBL( params );
}

int main( int argc, char** argv )
{
   int i = 0;

   ShapeModel::P model = MeshIO::loadModel( std::ifstream( argv[++i] ) );

   Vector vec( model->getNumberOfModes() );
   vec.fill( 0 );
   vec[0] = 1;
   readVector( i, argv, vec );

   Mesh::P mesh = model->getMesh( vec );

   MeshIO::saveMesh( std::ofstream( argv[++i] ), mesh );
}