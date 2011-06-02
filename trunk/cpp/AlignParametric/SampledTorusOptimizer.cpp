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
#include "SampledTorusOptimizer.h"

#include "TypeDefs.h"
#include "Mesh/MeshFunctions.h"
#include "Mesh/MeshImpl.h"
#include "Mesh/VNLTuples.h"
#include "SampledTorusMesh.h"
#include "Useful/ColouredConsole.h"

using namespace AWT;
using namespace AWT::AlignParametric;

AWT::AlignParametric::SampledTorusOptimizer::SampledTorusOptimizer()
: AWT::AlignParametric::ModelEigenvalueOptimizer()
{
   nk[0] = nk[1] = 1;
}

AWT::AlignParametric::SampledTorusOptimizer::~SampledTorusOptimizer()
{
}

AWT::AlignParametric::SampledTorusOptimizer::P AWT::AlignParametric::SampledTorusOptimizer::getInstance()
{
   AUTOGETINSTANCE(AWT::AlignParametric::SampledTorusOptimizer, ());
}

GETNAMEMACRO(AWT::AlignParametric::SampledTorusOptimizer);

void AWT::AlignParametric::SampledTorusOptimizer::setSamplingMesh(MeshType::P mesh)
{
   if (!mesh->hasTextureCoords())
      throw "Mesh must have texture coordinates";

   if (!mesh->hasFaces() || mesh->getNumberOfFaces() == 0)
      throw "Mesh must have faces";

   if (mesh != this->samplingMesh)
   {
      this->samplingMesh = mesh;
      modified();
   }
}

MeshType::P AWT::AlignParametric::SampledTorusOptimizer::getSamplingMesh()
{
   return samplingMesh;
}

void AWT::AlignParametric::SampledTorusOptimizer::setNumberOfKernels(const MeshIndex nk)
{
   const MeshIndex _nk[] = { nk, nk };
   setNumberOfKernels(_nk);
}

void AWT::AlignParametric::SampledTorusOptimizer::setNumberOfKernels(const MeshIndex nk[2])
{
   if (nk[0] != this->nk[0] || nk[1] != this->nk[1])
   {
      this->nk[0] = nk[0];
      this->nk[1] = nk[1];

      modified();
   }
}

void AWT::AlignParametric::SampledTorusOptimizer::getNumberOfKernels(MeshIndex nk[2])
{
   nk[0] = this->nk[0];
   nk[1] = this->nk[1];
}

SampledTorusMesh::P AWT::AlignParametric::SampledTorusOptimizer::addMesh(MeshType::P mesh)
{
   SampledTorusMesh::P ret = SampledTorusMesh::getInstance(mesh, mesh->getTextureCoords() /*samplingMesh->getTextureCoords()*/, nk);
   _add(ret);
   return ret;
}

FaceType::P AWT::AlignParametric::SampledTorusOptimizer::getFaces()
{
   return samplingMesh->getFaces();
}

void AWT::AlignParametric::SampledTorusOptimizer::calculateSampleWeights(const MatrixType& meanShape, VectorType& weights)
{
   const Idx N = getNumberOfSamplesPerSurface();
   weights.set_size(N);

   {
      ColouredConsole cons(ColouredConsole::COL_RED | ColouredConsole::COL_BRIGHT);
      DEBUGMACRO("HACKHACKHACK! Uniformly weighting the first 2500 samples.");

      for (Idx i = 0; i < N; ++i)
         weights(i) = (i < 2500) ? 1 : 0;

      return;
   }

   MeshType::P meanMesh = MeshImpl<T>::getInstance(0, 0);
   meanMesh->setVertices(VNLTuplesType::getInstance(meanShape, true));
   meanMesh->setFaces(samplingMesh->getFaces());

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
}