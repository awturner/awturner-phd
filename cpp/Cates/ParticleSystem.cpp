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
#include "ParticleSystem.h"

#include "DeformableMesh/AddTuples.h"

#include "vnl/vnl_matrix.h"
#include "vnl/algo/vnl_symmetric_eigensystem.h"

#include "Mesh/TuplesImpl.h"

#include "Useful/ArrayVectorFunctions.h"

#include <vector>
#include <limits>
#include <queue>

template <class T>
struct AWT::ParticleSystem<T>::D
{
   typename Tuples<T>::P calculateMeanShape(std::vector< typename Tuples<T>::P >& particles)
   {
      const MeshIndex M = getNumberOfMeshes();
      const MeshIndex N = getNumberOfParticles();

      T vtx[3], vtxAdd[3];

      Tuples<T>::P meanShape = TuplesImpl<T>::getInstance(3, N);

      vtx[0] = vtx[1] = vtx[2] = 0;
      for (MeshIndex v = 0; v < N; ++v)
         meanShape->addPoint(vtx);

      for (MeshIndex i = 0; i < M; ++i)
      {
         for (MeshIndex v = 0; v < N; ++v)
         {
            particles[i]->getPoint(v, vtx);
            meanShape->getPoint(v, vtxAdd);

            for (MeshIndex ax = 0; ax < 3; ++ax)
               vtxAdd[ax] += vtx[ax];

            meanShape->setPoint(v, vtxAdd);
         }
      }

      for (MeshIndex v = 0; v < N; ++v)
      {
         meanShape->getPoint(v, vtxAdd);

         for (MeshIndex ax = 0; ax < 3; ++ax)
            vtxAdd[ax] /= M;

         meanShape->setPoint(v, vtxAdd);
      }

      return meanShape;
   }

   T shapeInnerProduct(typename Tuples<T>::P shapeA, typename Tuples<T>::P shapeB, typename Tuples<T>::P meanShape)
   {
      const MeshIndex N = shapeA->getNumberOfPoints();

      T ret = 0;

      T vtxA[3], vtxB[3], vtxMean[3];

      for (MeshIndex i = 0; i < N; ++i)
      {
         shapeA->getPoint(i, vtxA);
         shapeB->getPoint(i, vtxB);
         meanShape->getPoint(i, vtxMean);

         for (MeshIndex ax = 0; ax < 3; ++ax)
            ret += (vtxA[ax] - vtxMean[ax]) * (vtxB[ax] - vtxMean[ax]);
      }

      return ret;
   }

   template <class A>
   static T diracDelta(const A& i, const A& j)
   {
      return (i == j) ? 1 : 0;
   }

   vnl_matrix<T> calculateInnerProductMatrix(std::vector< typename Tuples<T>::P >& particles, typename Tuples<T>::P meanShape)
   {
      const MeshIndex M = getNumberOfMeshes();
      const MeshIndex N = getNumberOfParticles();

      vnl_matrix<T> ret(M, M);
      ret.fill(0);

      for (MeshIndex k = 0; k < M; ++k)
      {
         for (MeshIndex j = k; j < M; ++j)
         {
            // Take the inner product between two shapes
            ret(j, k) = shapeInnerProduct(particles[j], particles[k], meanShape);

            // Make matrix symmetric
            ret(k, j) = ret(j, k);
         }
      }

      if (M > 1)
         ret /= (M - 1);

      return ret;
   }

   void calculateCrossDelta(std::vector< typename Tuples<T>::P >& particles, const MeshIndex p, typename Tuples<T>::P& meanShape, 
      const vnl_symmetric_eigensystem<T>& eig, const T alpha, typename Tuples<T>::P crossDelta)
   {
      const MeshIndex M = getNumberOfMeshes();
      const MeshIndex N = getNumberOfParticles();

      T vtxEigDelta[3], vtxDelta[3], vtxMean[3], vtxK[3], vtxJ[3];

      // For each point...
      for (MeshIndex q = 0; q < N; ++q)
      {
         vtxDelta[0] = vtxDelta[1] = vtxDelta[2] = 0;

         // For each eigenvalue
         for (MeshIndex i = 0; i < M; ++i)
         {
            vtxEigDelta[0] = vtxEigDelta[1] = vtxEigDelta[2] = 0;

            // Get the position of the corresponding mean shape vertex
            meanShape->getPoint(q, vtxMean);

            // For each pairing of shapes
            for (MeshIndex k = 0; k < M; ++k)
            {
               particles[k]->getPoint(q, vtxK);

               for (MeshIndex j = 0; j < M; ++j)
               {
                  const T mult = eig.V(j,i) * eig.V(k,i);

                  particles[j]->getPoint(q, vtxJ);

                  for (MeshIndex ax = 0; ax < 3; ++ax)
                  {
                     vtxEigDelta[ax] += mult * (
                        (vtxJ[ax] - vtxMean[ax]) * (D::diracDelta(k, p) - 1 / static_cast<T>(N)) + 
                        (vtxK[ax] - vtxMean[ax]) * (D::diracDelta(j, p) - 1 / static_cast<T>(N))
                    );
                  }
               }
            }
            // vtxEigDelta now contains d(lambda_i)/d([a_p]_q)

            for (MeshIndex ax = 0; ax < 3; ++ax)
            {
               vtxEigDelta[ax] /= alpha + eig.D(i, i);
               vtxDelta[ax] += vtxEigDelta[ax];
            }
         }

         crossDelta->addPoint(vtxDelta);
      }
   }

   const MeshIndex getNumberOfMeshes() const
   {
      return static_cast<MeshIndex>(m_Pams.size());
   }

   const MeshIndex getNumberOfParticles() const
   {
      return nparticles;
   }

   MeshIndex nparticles;
   std::vector< typename ParticlesAndMesh<T>::P > m_Pams;
   ValueRange<T> sigmaRange;
   std::vector< MeshIndex > splitTimes;
};

template <class T>
AWT::ParticleSystem<T>::ParticleSystem()
{
   m_D = new D;

   m_D->nparticles = 0;
   m_D->sigmaRange.lower = 0;
   m_D->sigmaRange.lower = std::numeric_limits<T>::infinity();
}

template <class T>
AWT::ParticleSystem<T>::~ParticleSystem()
{
   delete m_D;
}

template <class T>
typename AWT::ParticleSystem<T>::P AWT::ParticleSystem<T>::getInstance()
{
   AUTOGETINSTANCE(AWT::ParticleSystem<T>, ());
}

template <class T>
GETNAMEMACRO(AWT::ParticleSystem<T>);

template <class T>
void AWT::ParticleSystem<T>::setSigmaBounds(const T lower, const T upper)
{
   m_D->sigmaRange.lower = lower;
   m_D->sigmaRange.upper = upper;
}

template <class T>
AWT::MeshIndex AWT::ParticleSystem<T>::getNumberOfMeshes()
{
   return static_cast<MeshIndex>(m_D->m_Pams.size());
}

template <class T>
AWT::MeshIndex AWT::ParticleSystem<T>::getNumberOfParticles()
{
   return (m_D->m_Pams.empty()) ? 0 : static_cast<MeshIndex>(m_D->m_Pams[0]->getParticles()->getNumberOfPoints());
}

template <class T>
typename AWT::Mesh<T>::P AWT::ParticleSystem<T>::getMesh(AWT::MeshIndex i)
{
   return m_D->m_Pams[i]->getMesh();
}

template <class T>
typename AWT::Tuples<T>::P AWT::ParticleSystem<T>::getParticles(AWT::MeshIndex i)
{
   return m_D->m_Pams[i]->getParticles();
}

template <class T>
void AWT::ParticleSystem<T>::addMesh(typename AWT::Mesh<T>::P mesh, typename AWT::Tuples<T>::P particles)
{
   if (!m_D->m_Pams.empty())
   {
      if (particles->getNumberOfPoints() != m_D->nparticles)
         AWTEXCEPTIONTHROW("Mismatched number of particles");
   }
   else
   {
      m_D->nparticles = particles->getNumberOfPoints();
   }

   ParticlesAndMesh<T>::P pam = ParticlesAndMesh<T>::getInstance(mesh, particles);
   m_D->m_Pams.push_back(pam);

   pam->setSigmaRange(m_D->sigmaRange);
}

#include <fstream>

template <class T>
void AWT::ParticleSystem<T>::update(const T alpha)
{
   static std::ofstream fs("updates.txt");

   static MeshIndex updates = 0;
   static MeshIndex lastSplitTime = 0;

   const MeshIndex M = m_D->getNumberOfMeshes();
   const MeshIndex N = m_D->getNumberOfParticles();

   // For more convenient access, grab the particles from each particle/mesh pair
   std::vector< Tuples<T>::P > particles;
   for (MeshIndex i = 0; i < M; ++i)
      particles.push_back(m_D->m_Pams[i]->getParticles());

   // 0. Calculate the mean shape
   Tuples<T>::P meanShape = m_D->calculateMeanShape(particles);
   
   // 1. Use the mean shape to calculate the shape inner product
   vnl_matrix<T> A = m_D->calculateInnerProductMatrix(particles, meanShape);

   // 2. Calculate the eigendecomposition of this
   vnl_symmetric_eigensystem<T> eig(A);
   PRINTVBL(eig.D);

   const T maxeig = (M > 1) ? (std::max(eig.D(M-1,M-1), eig.D(0,0)) + alpha) : (100*alpha);
   const T aalpha = maxeig / 100;

   std::vector< Tuples<T>::P > deltas;

   DEBUGMACRO(m_D->sigmaRange.upper);

   /*
   PRINTVBL(sqrt(aalpha));
   m_D->sigmaRange.lower = sqrt(aalpha);
   m_D->sigmaRange.upper = 100*sqrt(aalpha);
   */

   T selfTerm  = 0;
   T crossTerm = 0;

   T maxT = std::numeric_limits<T>::infinity();

   // For each set of particles...
   for (MeshIndex p = 0; p < M; ++p)
   {
      AddTuples<T>::P weightedSum = AddTuples<T>::getInstance(3);

      // To hold the maximum value of the time step
      T selfMaxT = maxT;

      // Calculate the entropy within just this surface
      Tuples<T>::P selfDelta  = TuplesImpl<T>::getInstance(3, N);
      selfTerm += m_D->m_Pams[p]->calculateInternalForceUpdate(m_D->sigmaRange, selfDelta, selfMaxT);
      weightedSum->insert(selfDelta, 1);

      // Calculate the cross-particle set entropy
      if (M > 1)
      {
         Tuples<T>::P crossDelta = TuplesImpl<T>::getInstance(3, N);
         m_D->calculateCrossDelta(particles, p, meanShape, eig, alpha, crossDelta);
         //weightedSum->add(crossDelta, -1);
      }
      
      deltas.push_back(weightedSum);

      // Want to use the minimum of all max permissible time steps
      // in order to be convergent
      maxT = std::min<T>(maxT, selfMaxT);
   }

   BLANKLINE;
   BLANKLINE;
   PRINTVBL(selfTerm);
   PRINTVBL(crossTerm);
   PRINTVBL(N);
   BLANKLINE;
   BLANKLINE;

   fs << updates << "\t" << N << "\t" << selfTerm << "\t" << crossTerm << std::endl;

   T maxMove = 0;
   T rmsMove = 0;
   T vtx[3];

   Tuples<T>::P actualDelta = TuplesImpl<T>::getInstance(3, N);

   PRINTVBL(maxT);
   for (MeshIndex p = 0; p < M; ++p)
   {
      m_D->m_Pams[p]->updateParticles(deltas[p], 0.9*maxT, actualDelta);

      for (MeshIndex i = 0; i < N; ++i)
      {
         const T sigmaI = m_D->m_Pams[p]->getSigma()->getPointElement(i, 0);

         actualDelta->getPoint(i, vtx);

         T tmp = dot<T>(vtx, vtx, 3) / (sigmaI*sigmaI);
         maxMove = std::max(maxMove, tmp);
         rmsMove += tmp;
      }
   }

   // Calculate the max and rms of the actual deltas.
   
   maxMove = sqrt(maxMove);
   rmsMove = sqrt(rmsMove / (N*M));

   PRINTVBL(maxMove);
   PRINTVBL(rmsMove);

   // Upper limit on the number of particles
   PRINTVBL(N);
   if (false && N < 4096 && lastSplitTime+10 <= updates)
   {
      // Check to see which points have slowed down, and split them
      Tuples<T>::P newMeanShape = m_D->calculateMeanShape(particles);
      for (MeshIndex i = 0; i < N; ++i)
      {
         T vtxOld[3];
         T vtxNew[3];

         T sigmaI = std::numeric_limits<T>::infinity();
         for (MeshIndex p = 0; p < M; ++p)
            sigmaI = std::min<T>(sigmaI, m_D->m_Pams[p]->getSigma()->getPointElement(i, 0));

         const T sigmaCut = sigmaI / 1000 / maxT;

         meanShape->getPoint(i, vtxOld);
         newMeanShape->getPoint(i, vtxNew);

         const T dist2 = deltaNormSquared(vtxOld, vtxNew, 3);

         // Don't let a particle split if it is already in a dense area
         if (dist2 < sigmaCut*sigmaCut)
         {
            //DEBUGMACRO("Splitting particle " << i << " (dnorm/sigma=" << deltaNorm(vtxOld, vtxNew, 3)/sigmaI << ")");
            for (MeshIndex p = 0; p < M; ++p)
            {
               m_D->m_Pams[p]->splitParticle(i);
            }

            ++m_D->nparticles;
            lastSplitTime = updates;
         }
      }
   }

   // Increment the number of times called
   ++updates;
}

template <class T>
void AWT::ParticleSystem<T>::split()
{
   BLANKLINE;
   DEBUGMACRO("=========");
   DEBUGMACRO("Splitting");
   DEBUGMACRO("=========");
   BLANKLINE;

   const MeshIndex M = m_D->getNumberOfMeshes();

   for (MeshIndex i = 0; i < M; ++i)
   {
      m_D->m_Pams[i]->splitParticles();
   }

   m_D->nparticles = m_D->m_Pams[0]->getParticles()->getNumberOfPoints();
}

template class AWT::ParticleSystem<double>;