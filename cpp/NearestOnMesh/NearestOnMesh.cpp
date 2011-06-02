#include "SimpleMesh/Mesh.h"
#include "SimpleMesh/MeshIO.h"
#include "SimpleMesh/ShapeModel.h"

#include "Mesh/MeshImpl.h"
#include "Mesh/VNLTuples.h"
#include "MeshSearching/FacesNearestPointSearch.h"

#include "Useful/ArrayVectorFunctions.h"

#include <fstream>

using namespace AWT;

typedef double T;

Mesh<T>::P simpleToMesh( SimpleMesh::Mesh::P mesh )
{
   Mesh<T>::P ret = MeshImpl<T>::getInstance( 0, 0 );

   Tuples<T>::P vs = TuplesImpl<T>::getInstance( 3, mesh->nv );
   for ( MeshIndex i = 0; i < mesh->nv; ++i )
   {
      SimpleMesh::Point p = mesh->getVertex(i);
      vs->setPoint(i, p.data_block());
   }

   Tuples<MeshIndex>::P fs = TuplesImpl<MeshIndex>::getInstance( 3, mesh->nf );
   for ( MeshIndex i = 0; i < mesh->nf; ++i )
   {
      SimpleMesh::Face f = mesh->getFace(i);
      fs->setPoint(i, f.data_block());
   }

   ret->setVertices(vs);
   ret->setFaces(fs);

   return ret;
}

int main( int argc, char** argv )
{
   int i = 0;

   if (argc < 5)
   {
      DEBUGMACRO( "Not enough arguments" );
      DEBUGLINEANDEXIT(1);
   }

   const char* refFilename( argv[++i] );
   const char* queryFilename( argv[++i] );
   const char* outFilename( argv[++i] );
   const double scale = atof(argv[++i] );

   Mesh<T>::P referenceMesh = simpleToMesh( SimpleMesh::MeshIO::loadMesh( std::ifstream( refFilename ) ) );

   SimpleMesh::Mesh::P queryMesh = SimpleMesh::MeshIO::loadMesh( std::ifstream( queryFilename ) );
   Mesh<T>::P qqueryMesh    = simpleToMesh( queryMesh );

   SimpleMesh::Mesh::P outputMesh = SimpleMesh::Mesh::getInstance(queryMesh->nv, queryMesh->nf);
   outputMesh->getFaces().update( queryMesh->getFaces() );

   referenceMesh->prepareToSearchVertices();

   FacesNearestPointSearch<T>::P searcher = FacesNearestPointSearch<T>::getInstance();

   T vtx[3], vtxNearest[4];
   for (MeshIndex v = 0; v < qqueryMesh->getNumberOfVertices(); ++v)
   {
      qqueryMesh->getVertex(v, vtx);

      searcher->reset();
      searcher->setTestPoint(vtx);

      referenceMesh->search(searcher);

      searcher->getNearestPoint(vtxNearest);

      vtxNearest[3] = 1;
      outputMesh->getVertices().set_column(v, vtxNearest);
   }

   outputMesh->getVertices() *= scale;
   outputMesh->getVertices().set_row(3,1);

   SimpleMesh::MeshIO::saveMesh(std::ofstream( outFilename ), outputMesh);
}