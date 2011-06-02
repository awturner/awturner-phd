#include "SimpleMesh/MeshIO.h"
#include "SimpleMesh/MeshFunctions.h"
#include "ArmInRegion/LuaRegion.h"

#include <fstream>

using namespace AWT;
using namespace AWT::SimpleMesh;

void usage( )
{
   std::cerr << "Usage: SimpleMeshCutter <lua region file> <input simple mesh> <output inside region> <output outside region>" << std::endl;
}

Mesh::P compactMesh( const Points& vs, const Faces& fs, const Index c, const Index nf )
{
   Mesh::P simple = Mesh::getInstance( vs.cols(), nf );
   {  
      simple->getVertices().update( vs );
      simple->getFaces().update( fs.extract(3,nf,0,c) );
   }

   // Need to work out how to renumber the vertices
   const Index nv = vs.cols();
   Index* map = new Index[nv];
   for ( Index i = 0; i < nv; ++i )
      map[i] = INVALID_INDEX;

   Index vp = 0;
   // Now, go through the faces and mark the vertices in order
   for ( Index i = c; i < c + nf; ++i )
   {
      Face face = fs.get_column(i);
      for ( Index a = 0; a < 3; ++a )
         if ( map[ face[a] ] == INVALID_INDEX )
            map[ face[a] ] = vp++;
   }


   // So, we have the remapping; create the points array for this
   Points nvs( 4, vp );
   Faces  nfs( 3, nf );

   nvs.set_row( 3, FAR_FAR_AWAY );

   for ( Index i = c; i < c + nf; ++i )
   {
      Face face = fs.get_column(i);

      for ( Index a = 0; a < 3; ++a )
      {
         if ( nvs( 3, map[ face[a] ] ) == FAR_FAR_AWAY )
            nvs.set_column( map[ face[a] ], vs.get_column( face[a] ) );

         face[a] = map[ face[a] ];
      }
      
      nfs.set_column( i-c, face );
   }

   Mesh::P ret = Mesh::getInstance( vp, nf );
   ret->getVertices().update( nvs );
   ret->getFaces().update( nfs );

   // Make sure to clean up
   delete [] map;

   return ret;
}

Point point( const double x, const double y, const double z, const double w )
{
   Point ret;

   ret[0] = x;
   ret[1] = y;
   ret[2] = z;
   ret[3] = w;

   return ret;
}

int main( int argc, char** argv )
{
   if (argc < 8)
   {
      DEBUGMACRO( "Not enough arguments" );
      DEBUGLINEANDEXIT(1);
   }

   const char* inFilename = argv[1];

   const Point  centre = point( atof(argv[2]), atof(argv[3]), atof(argv[4]), 1 );
   const double radius = atof(argv[5]);

   const char* insideFilename  = argv[6];
   const char* outsideFilename = argv[7];

   PRINTVBL( inFilename );
   PRINTVBL( centre );
   PRINTVBL( radius );
   PRINTVBL( insideFilename );
   PRINTVBL( outsideFilename );

   Mesh::P mesh = MeshIO::loadMesh( std::ifstream( inFilename ) );

   // Create inside and outside face meshes
   // But they are mutually exclusive, so essentially just divvy them up
   Faces newFaces( 3, mesh->nf );

   Index ip = 0;
   Index op = mesh->nf;

   double maxRR = 0;

   Point vs[3];
   for ( Index f = 0; f < mesh->nf; ++f )
   {
      Face face = mesh->getFace( f );
      getFaceVertices( mesh, f, vs );

      Index i;
      for ( i = 0; i < 3; ++i )
      {
         const double rr = ( centre - vs[i] ).squared_magnitude();

         maxRR = ( rr > maxRR ) ? rr : maxRR;

         if ( rr > radius*radius )
            break;
      }

      if ( i == 3 )
         newFaces.set_column( ip++, face );
      else
         newFaces.set_column( --op, face );
   }
   DEBUGMACRO( ip << "\t" << op );
   std::cout << sqrt(maxRR) << std::endl;

   // Now, create compact meshes from these face sets
   if ( strcmp( insideFilename, "-" ) != 0 )
   {
      Mesh::P insideMesh = compactMesh( mesh->getVertices(), newFaces, 0, ip );
      MeshIO::saveMesh( std::ofstream( insideFilename ), insideMesh );
   }

   if ( strcmp( outsideFilename, "-" ) != 0 )
   {
      Mesh::P outsideMesh = compactMesh( mesh->getVertices(), newFaces, ip, newFaces.cols()-ip );
      MeshIO::saveMesh( std::ofstream( outsideFilename ), outsideMesh );
   }
}

int main2( int argc, char** argv )
{
   if ( argc < 5 )
   {
      usage( );
      DEBUGLINEANDEXIT( 1 );
   }

   try
   {
      LuaRegion region( argv[1] );
      DEBUGLINE;

      Mesh::P mesh = MeshIO::loadMesh( std::ifstream( argv[2] ) );

      // Create inside and outside face meshes
      // But they are mutually exclusive, so essentially just divvy them up
      Faces newFaces( 3, mesh->nf );

      Index ip = 0;
      Index op = mesh->nf;

      Point vs[3];
      for ( Index f = 0; f < mesh->nf; ++f )
      {
         Face face = mesh->getFace( f );
         getFaceVertices( mesh, f, vs );

         Index i;
         for ( i = 0; i < 3; ++i )
         {
            if ( region.testPoint( vs[i].data_block() ) > 0 )
               break;
         }

         if ( i == 3 )
            newFaces.set_column( ip++, face );
         else
            newFaces.set_column( --op, face );
      }
      DEBUGMACRO( ip << "\t" << op );

      // Now, create compact meshes from these face sets
      Mesh::P insideMesh = compactMesh( mesh->getVertices(), newFaces, 0, ip );
      MeshIO::saveMesh( std::ofstream( argv[3] ), insideMesh );

      Mesh::P outsideMesh = compactMesh( mesh->getVertices(), newFaces, ip, newFaces.cols()-ip );
      MeshIO::saveMesh( std::ofstream( argv[4] ), outsideMesh );
   }
   catch ( std::exception ex )
   {
      PRINTVBL( ex.what() );
   }
}