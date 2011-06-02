#include "SearchAgent.h"

AWT::SimpleMesh::Mesh::P AWT::SimpleMesh::SearchAgent::getMesh( )
{
   return mesh;
}

void AWT::SimpleMesh::SearchAgent::setMesh( Mesh::P mesh )
{
   this->mesh = mesh;
}