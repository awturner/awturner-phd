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
#include "CoulombicRegularizer.h"

#include "CoulombicInfluenceAccumulator.h"
#include "Mesh/MeshImpl.h"
#include "CoulombicParticleSurface.h"

using namespace AWT;
using namespace AWT::AlignParametric;

struct AWT::AlignParametric::CoulombicRegularizer::D
{
   CoulombicParticleSurface* surf;
   
   TuplesType::P particles;
   MeshType::P  particlesMesh;

   T m_Cutoff;
   T m_Threshold;

   MatrixType updates;
   T energy;
};

AWT::AlignParametric::CoulombicRegularizer::CoulombicRegularizer(CoulombicParticleSurface* surf)
{
   m_D = new D;
   m_D->surf = surf;

   m_D->particles = surf->getSamples();
   m_D->particlesMesh = MeshImpl<T>::getInstance(0, 0);
   m_D->particlesMesh->setVertices(m_D->particles);

   m_D->updates.set_size(3, m_D->particlesMesh->getNumberOfVertices());
   m_D->updates.fill(0);
   m_D->energy = 0;
}

AWT::AlignParametric::CoulombicRegularizer::~CoulombicRegularizer()
{
   delete m_D;
}

AWT::AlignParametric::CoulombicRegularizer::P AWT::AlignParametric::CoulombicRegularizer::getInstance(CoulombicParticleSurface* surf)
{
   AUTOGETINSTANCE(AWT::AlignParametric::CoulombicRegularizer, (surf));
}

GETNAMEMACRO(AWT::AlignParametric::CoulombicRegularizer);

T AWT::AlignParametric::CoulombicRegularizer::getMinDistance(const Idx p)
{
   return 10000;
}

T AWT::AlignParametric::CoulombicRegularizer::getCutoffDistance() const
{
   return m_D->m_Cutoff;
}

void AWT::AlignParametric::CoulombicRegularizer::setCutoffDistance(const T v)
{
   if (v != m_D->m_Cutoff)
   {
      m_D->m_Cutoff = v;
      modified();
   }
}

T AWT::AlignParametric::CoulombicRegularizer::getThreshold() const
{
   return m_D->m_Threshold;
}

void AWT::AlignParametric::CoulombicRegularizer::setThreshold(const T v)
{
   if (v != m_D->m_Threshold)
   {
      m_D->m_Threshold = v;
      modified();
   }
}

// Calculate the force which tries to push these particles apart
void AWT::AlignParametric::CoulombicRegularizer::calculateUpdate(MatrixType& reg)
{
   reg.update(m_D->updates);
}

T AWT::AlignParametric::CoulombicRegularizer::calculateCost(const bool debug)
{
   // This is a searcher - set it up how we want it.
   CoulombicInfluenceAccumulator<T>::P influence = CoulombicInfluenceAccumulator<T>::getInstance();
   influence->setThreshold(0.3);
   influence->setCutoff(m_D->m_Cutoff);

   // Rebuild the Kd tree of the particle mesh so that we can search the particles
   m_D->particlesMesh->prepareToSearchVertices(true);

   const MeshIndex N = m_D->particlesMesh->getNumberOfVertices();
   
   MatrixType updates(N, 3);

   T vtx[3];
   T upd[3];
   T energy = 0;

   MESH_EACHVERTEX(m_D->particlesMesh, v)
   {
      m_D->particlesMesh->getVertex(v, vtx);

      influence->reset();
      influence->initialize(v, vtx);

      // Search the mesh, influence does its magic and calculates things
      m_D->particlesMesh->searchVertices(influence);

      // Now extract the update and the energy for this particle
      influence->getUpdate(upd);
      for (MeshIndex ax = 0; ax < 3; ++ax)
         updates(v,ax) = upd[ax];

      energy += influence->getEnergy();
   }

   m_D->updates = updates;
   m_D->energy = -energy / N;

   return m_D->energy;
}

void AWT::AlignParametric::CoulombicRegularizer::associateSampleToFace(const MeshIndex i, const MeshIndex f)
{
   // This doesn't need to do anything, the Euclidean regularizer doesn't use particle faces at all.
}
