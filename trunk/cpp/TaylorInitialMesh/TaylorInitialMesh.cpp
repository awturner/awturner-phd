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

#include "Useful/ArrayVectorFunctions.h"
#include "Mesh/Mesh.h"
#include "Mesh/MeshImpl.h"
#include "Mesh/VTKMeshLoader.h"
#include "Mesh/VTKMeshWriter.h"
#include "MeshSearching/FacesNearestPointSearch.h"
#include "MeshSearching/VerticesNearestVertexSearch.h"

using namespace AWT;

typedef double T;
typedef Mesh<T> MeshType;

int main(int argc, char** argv)
{
   MeshType::P splitMesh;
   MeshType::P unsplitMesh;
   MeshType::P deformedMesh;
   MeshType::P targetMesh;
   MeshType::P outputMesh;
   
   char* outFilename = 0;

   for (int i = 1; i < argc; ++i)
   {
      if (strcmp(argv[i], "-splitmesh") == 0)
      {
         splitMesh = VTKMeshLoader<T>::load(argv[++i], false);
         if (!splitMesh->hasTextureCoords())
         {
            DEBUGMACRO("Mesh must have texture coordinates");
            DEBUGLINEANDEXIT(1);
         }
      }
      else if (strcmp(argv[i], "-unsplitmesh") == 0)
      {
         unsplitMesh = VTKMeshLoader<T>::load(argv[++i], false);
      }
      else if (strcmp(argv[i], "-deformed") == 0)
      {
         deformedMesh = VTKMeshLoader<T>::load(argv[++i], false);
      }
      else if (strcmp(argv[i], "-targetMesh") == 0)
      {
         targetMesh = VTKMeshLoader<T>::load(argv[++i], false);
      }
      else if (strcmp(argv[i], "-out") == 0)
      {
         outFilename = argv[++i];
      }
      else
      {
         DEBUGMACRO("Unknown argument: " << argv[i]);
         DEBUGLINEANDEXIT(1);
      }
   }

   if (outFilename == 0)
   {
      DEBUGMACRO("Output filename must be set");
      DEBUGLINEANDEXIT(1);
   }

   // Make sure that the meshes are compatible
   if (splitMesh->getNumberOfFaces() != unsplitMesh->getNumberOfFaces())
   {
      DEBUGMACRO("Split and unsplit meshes must have the same number of faces");
      DEBUGLINEANDEXIT(1);
   }

   if (unsplitMesh->getNumberOfVertices() != deformedMesh->getNumberOfVertices())
   {
      DEBUGMACRO("Unsplit and deformed meshes must have the same number of vertices");
      DEBUGLINEANDEXIT(1);
   }

   const MeshIndex N = splitMesh->getNumberOfVertices();

   // Create the output mesh - it is essentially the same as the split mesh, just with different vertex locations
   outputMesh = MeshImpl<T>::getInstance(N, 0);
   outputMesh->setFaces(splitMesh->getFaces());

   // Create a mesh searcher
   VerticesNearestVertexSearch<T>::P vertexSearcher = VerticesNearestVertexSearch<T>::getInstance();
   FacesNearestPointSearch<T>::P faceSearcher = FacesNearestPointSearch<T>::getInstance();

   T vtx[3], vtxNearest[3];

   // Now, go through all of the vertices
   for (MeshIndex i = 0; i < N; ++i)
   {
      // Get the i-th vertex from the split mesh
      splitMesh->getVertex(i, vtx);

      // Find the closest vertex in the unsplit mesh
      vertexSearcher->reset();
      vertexSearcher->setTestPoint(vtx);
      unsplitMesh->searchVertices(vertexSearcher);

      MeshIndex nv = vertexSearcher->getNearestPoint(vtxNearest);

      // Find the same numbered vertex in the deformed mesh
      deformedMesh->getVertex(nv, vtx);

      // Project this vertex back to the target surface to get better location
      faceSearcher->reset();
      faceSearcher->setTestPoint(vtx);
      targetMesh->searchFaces(faceSearcher);

      MeshIndex nf = faceSearcher->getNearestPoint(vtxNearest);

      // Write vertex location to new mesh
      outputMesh->setVertex(i, vtxNearest);
   }

   outputMesh->setTextureCoords(splitMesh->getTextureCoords());
   
   VTKMeshWriter<T>::write(outputMesh, outFilename);
}