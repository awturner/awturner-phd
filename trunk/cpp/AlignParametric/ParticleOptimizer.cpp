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
#include "ParticleOptimizer.h"

#define WITH_MATLAB 0

#if WITH_MATLAB
//#include "Useful/MatlabEngine.h"
#endif

#include "Mesh/MeshImpl.h"
#include "Mesh/MeshFunctions.h"
#include "Mesh/VNLTuples.h"

#include "ParticleSurfaceRefinement.h"

using namespace AWT;
using namespace AWT::AlignParametric;

AWT::AlignParametric::ParticleOptimizer::ParticleOptimizer(MeshIndex _N)
: N(_N)
{
   // Put in a default refinement strategy
   ParticleSurfaceRefinement::P psr = ParticleSurfaceRefinement::getInstance();

   setRefinementStrategy(psr);

   faces = TuplesImpl<MeshIndex>::getInstance(3, 0);
}

MeshIndex AWT::AlignParametric::ParticleOptimizer::getNumberOfParticlesPerSurface() const
{
   return N;
}

void AWT::AlignParametric::ParticleOptimizer::calculateSampleWeights(const MatrixType& meanShape, VectorType& weights)
{
   const Idx N = getNumberOfSamplesPerSurface();
   weights.set_size(N);
   weights.fill(1.0 / N);
}

void AWT::AlignParametric::ParticleOptimizer::checkNumberOfPoints(TuplesType::P particles)
{
   // Make sure that there are the correct number of particles
   if (particles->getNumberOfPoints() != getNumberOfParticlesPerSurface())
   {
      PRINTVBL(getNumberOfParticlesPerSurface());
      PRINTVBL(particles->getNumberOfPoints());
      throw "Wrong number of particles specified";
   }
}

FaceType::P AWT::AlignParametric::ParticleOptimizer::getFaces()
{
   return faces;
}
