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
#include "Mesh/VTKMeshLoader.h"
#include "Mesh/VTKMeshWriter.h"

#include "MeshBreaker/FindGenerators.h"
#include "MeshBreaker/FlattenMesh.h"
#include "MeshBreaker/PelvicGeneratorDirection.h"
#include "Useful/ProfTimer.h"

using namespace AWT;

typedef double T;

int main( int argc, char** argv )
{
   char* inputFilename;
   char* outputFilename;
   FlattenMesh<T>::WeightType wt = FlattenMesh<T>::WEIGHT_FLOATER;
   FlattenMesh<T>::SpreadType st = FlattenMesh<T>::SPREAD_YOSHIZAWA;
   FindGenerators<T>::TraversalOrder ord = FindGenerators<T>::TRAVERSE_BREADTH;
   int fixed = -1;

   PRINTVBL( argc );
   for ( int i = 1; i < argc; ++i )
   {
      if ( strcmp( argv[i], "-in" ) == 0 )
      {
         inputFilename = argv[++i];
      }
      else if ( strcmp( argv[i], "-out" ) == 0 )
      {
         outputFilename = argv[++i];
      }
      else if ( strcmp( argv[i], "-ord" ) == 0 )
      {
         ord = FindGenerators<T>::getTraversalOrder( argv[++i] );
      }
      else if ( strcmp( argv[i], "-wt" ) == 0 )
      {
         wt = FlattenMesh<T>::getWeightType( argv[++i] );
      }
      else if ( strcmp( argv[i], "-st" ) == 0 )
      {
         st = FlattenMesh<T>::getSpreadType( argv[++i] );
      }
      else if ( strcmp( argv[i], "-f" ) == 0 )
      {
         fixed = atoi( argv[++i] );
      }
      else
      {
         DEBUGMACRO( "Unknown argument: " << argv[i] );
         DEBUGLINEANDEXIT( 1 );
      }
   }

   PRINTVBL( inputFilename );
   PRINTVBL( outputFilename );
   PRINTVBL( wt );
   PRINTVBL( st );
   PRINTVBL( fixed );
   PRINTVBL( ord );
   
   Mesh<T>::P mesh = VTKMeshLoader<T>::load( inputFilename, false );
   PRINTVBL( mesh->getNumberOfFaces() );

   FindGenerators<T>::P pgd;
   Mesh<T>::P flattened;
   
   {
      TimedBlock block( "Finding generators" );
      pgd = FindGenerators<T>::getInstance( mesh, ord );
   }
   
   {
      TimedBlock block( "Flattening" );
      flattened = FlattenMesh<T>::flatten( pgd, wt, st );
   }

   PRINTVBL( flattened->getTextureCoords()->getNumberOfPoints() );

   // Translate so that
   if ( fixed != -1 )
   {
      DEBUGMACRO( "Translating so that texture origin is at vertex " << fixed );

      T vtxFixed[3];
      flattened->getTextureCoords( )->getPoint( fixed, vtxFixed );

      T vtx[3];
      MESH_EACHVERTEX( flattened, v )
      {
         flattened->getTextureCoords( )->getPoint( v, vtx );
         vtx[0] -= vtxFixed[0];
         vtx[1] -= vtxFixed[1];
         vtx[2] -= vtxFixed[2];
         flattened->getTextureCoords( )->setPoint( v, vtx );
      }
   }

   DEBUGMACRO( "Writing to " << outputFilename );
   VTKMeshWriter<T>::write( flattened, outputFilename );
}