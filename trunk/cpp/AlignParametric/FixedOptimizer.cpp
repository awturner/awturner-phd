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
#include "FixedOptimizer.h"

#include "TypeDefs.h"
#include "Mesh/MeshFunctions.h"
#include "Mesh/MeshImpl.h"
#include "Mesh/VNLTuples.h"
#include "FixedSurface.h"

using namespace AWT;
using namespace AWT::AlignParametric;

AWT::AlignParametric::FixedOptimizer::FixedOptimizer()
: AWT::AlignParametric::ModelEigenvalueOptimizer()
{
}

AWT::AlignParametric::FixedOptimizer::~FixedOptimizer()
{
}

AWT::AlignParametric::FixedOptimizer::P AWT::AlignParametric::FixedOptimizer::getInstance()
{
   AUTOGETINSTANCE(AWT::AlignParametric::FixedOptimizer, ());
}

GETNAMEMACRO(AWT::AlignParametric::FixedOptimizer);

AWT::AlignParametric::FixedSurface::P AWT::AlignParametric::FixedOptimizer::addMesh(MeshType::P mesh)
{
   if (*faces == 0)
      faces = mesh->getFaces();

   FixedSurface::P ret = FixedSurface::getInstance(mesh);
   _add(ret);
   return ret;
}

FaceType::P AWT::AlignParametric::FixedOptimizer::getFaces()
{
   return faces;
}

void AWT::AlignParametric::FixedOptimizer::calculateSampleWeights(const MatrixType& meanShape, VectorType& weights)
{
   const Idx N = getNumberOfSamplesPerSurface();
   weights.set_size(N);

   MeshType::P meanMesh = MeshImpl<T>::getInstance(0, 0);
   meanMesh->setVertices(VNLTuplesType::getInstance(meanShape, true));
   meanMesh->setFaces(getFaces());

   const T meanArea = MeshFunctions<T>::getSurfaceArea(meanMesh);
   
   // Fill with zeros
   weights.fill(0);
   
   // Go through each face, calculate its area, and increment the total for each vertex
   MeshIndex vs[3];
   MESH_EACHFACE(meanMesh, f)
   {
      const T area = MeshFunctions<T>::getFaceArea(meanMesh, f);

      meanMesh->getFaceIndices(f, vs);

      for (MeshIndex ax = 0; ax < 3; ++ax)
         weights[ vs[ax] ] += area;
   }

   DEBUGMACRO("Using uniform weighting");
   weights.fill(1.0 / weights.size());
}