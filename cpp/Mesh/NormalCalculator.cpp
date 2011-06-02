#include "NormalCalculator.h"

#include "Mesh.h"
#include "Tuples.h"
#include "TuplesImpl.h"

template <class T>
void AWT::NormalCalculator<T>::calculateNormalsAndSet( typename AWT::Mesh<T>::P mesh, typename Tuples<T>::P vtxNormals, typename Tuples<T>::P faceNormals )
{
   bool createVtx  = false;
   bool createFace = false;

   T def[] = { 0, 0, 0 };

   if ( *vtxNormals == 0 )
   {
      createVtx = true;
      vtxNormals = TuplesImpl<T>::getInstance( 3, def, mesh->getNumberOfVertices( ) );
   }

   if ( *faceNormals == 0 )
   {
      createFace = true;
      faceNormals = TuplesImpl<T>::getInstance( 3, def, mesh->getNumberOfVertices( ) );
   }

   calculateNormals( mesh, vtxNormals, faceNormals );

   mesh->setVertexNormals( vtxNormals );
   mesh->setFaceNormals( faceNormals );
}

template class AWT::NormalCalculator<double>;
template class AWT::NormalCalculator<float>;