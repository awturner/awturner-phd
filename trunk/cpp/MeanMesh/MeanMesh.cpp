/*
* ################################################################################
* ### MIT License
* ################################################################################
* 
* Copyright (c) 2006-2011 Andy Turner
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/
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