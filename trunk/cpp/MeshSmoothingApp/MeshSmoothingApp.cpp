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
#include "Mesh/AreaAveragedNormalCalculator.h"
#include "Mesh/Mesh.h"
#include "Mesh/VTKMeshLoader.h"
#include "Mesh/VTKMeshWriter.h"

#include "MeshSmoothing/MeshSmoothing.h"

#include <iostream>
#include <cmath>



using namespace AWT;

typedef double T;

T calculateMeanEdgeLength( Mesh<T>::P mesh )
{
   T pntA[3], pntB[3], pntC[3];
   T delta[3];
   T tmp;
   T lenSum = 0;
   MeshIndex lenCnt = 0;
   //for ( MeshIndex f = 0; f < mesh->getNumberOfFaces( ); ++f )
   MESH_EACHFACE( mesh, f )
   {
      mesh->getFace( f, pntA, pntB, pntC );

      delta[0] = delta[1] = delta[2] = 0;

      for ( int i = 0; i < 3; ++i )
      {
         tmp = pntB[i] - pntA[i];
         delta[0] += tmp*tmp;

         tmp = pntC[i] - pntB[i];
         delta[1] += tmp*tmp;

         tmp = pntA[i] - pntC[i];
         delta[2] += tmp*tmp;
      }
      
      for ( int i = 0; i < 3; ++i )
      {
         lenSum += sqrt( delta[i] );
         ++lenCnt;
      }
   }

   return ( lenSum / lenCnt );
}

int main( int argc, char** argv )
{
   if ( argc < 5 )
   {
      std::cerr << "Must provide source and target filename, spatial and influence sigmas";
      DEBUGLINEANDEXIT( 1 );
   }

   double spatialSigma   = atof( argv[3] );
   double influenceSigma = atof( argv[4] );

   Mesh<double>::P mesh = VTKMeshLoader<T>::load( argv[1] );

   AreaAveragedNormalCalculator<T>::getInstance( )->calculateNormalsAndSet( mesh );

   T meanEdgeLength = calculateMeanEdgeLength( mesh );

   PRINTVBL( spatialSigma * meanEdgeLength );
   PRINTVBL( influenceSigma * meanEdgeLength );
   Mesh<double>::P smoothedMesh = MeshSmoothing<double>::smooth( mesh, spatialSigma * meanEdgeLength, influenceSigma  * meanEdgeLength );

   VTKMeshWriter<double>::write( smoothedMesh, argv[2] );
}