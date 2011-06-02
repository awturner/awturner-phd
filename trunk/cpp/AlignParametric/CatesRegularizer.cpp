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
#include "AlignParametric/CatesRegularizer.h"

#include "AlignParametric/CatesParticleSurface.h"
#include "Mesh/MeshImpl.h"
#include "Maths/Sequences.h"
#include "Mesh/MeshFunctions.h"
#include "Mesh/TuplesFunctions.h"
#include "Mesh/TuplesImpl.h"
#include "MeshSearching/VerticesTreeData.h"
#include "MeshSearching/FacesNearestPointSearch.h"
#include "MeshSearching/MeshApproxGeodesic.h"
#include "MeshSearching/VerticesNearestVerticesSearch.h"
#include "Useful/ArrayVectorFunctions.h"
#include "Useful/ColouredConsole.h"
#include "Useful/Noise.h"
#include "Maths/Gaussian.h"
#include "vnl/vnl_matlab_filewrite.h"

using namespace AWT;
using namespace AWT::AlignParametric;

#define CATESREG_USESELF 0

struct AWT::AlignParametric::CatesRegularizer::D
{
   bool fixSigma;

   struct Direction { T x; T y; T z; };
   typedef std::pair< T, Direction > DistanceDirection;
   typedef std::map< MeshIndex, DistanceDirection > DistanceDirectionMap;

   typedef std::map< MeshIndex, std::vector<MeshIndex> > SampleOnFaceMap;

   struct DistanceDirectionWeight
   {
      T         distance;
      Direction direction;
      T         weight;
   };

   typedef std::map< MeshIndex, DistanceDirectionWeight > DDWRow;
   typedef std::map< MeshIndex, DDWRow >                  DDWRows;

   typedef std::vector< MeshIndex >                       AdjRow;
   typedef std::map< MeshIndex, AdjRow >                  AdjRows;

   struct Adjacency
   {
      void prepare(const MeshIndex N)
      {
         ddwMatrix.clear();

         for (MeshIndex r = 0; r < N; ++r)
         {
            ddwMatrix[r] = DDWRow();
            adjMatrix[r] = AdjRow();
         }

         weightsRowSum.set_size(N);
         weightsRowSum.fill(0);
      }

      void insert(const MeshIndex row, const MeshIndex col, const DistanceDirectionWeight& ddw)
      {
         ddwMatrix[row][col] = ddw;
         adjMatrix[col].push_back(row);

         weightsRowSum(row)    += ddw.weight;
      }

      DDWRows       ddwMatrix;
      AdjRows       adjMatrix;

      vnl_vector<T> weightsRowSum;
   };

   
   MeshIndex maxSigmaIters;

   ValueRange<T> actualSigmaRange;

   CatesParticleSurface* surf;
   T meshSurfaceArea;

   TuplesType::P particles;

   TuplesType::P sigma;

   MeshType::P   particlesMesh;

   MeshApproxGeodesic<T>::P geo;

   std::map<MeshIndex,MeshIndex> particleFaceIndex;
   SampleOnFaceMap                 particlesOnFace;

   Adjacency adj;
   ValueRange<T> sigmaRange;

   DistanceType distanceType;

   void setSigma(const MeshIndex i, const T v, const AWT::ValueRange<T>& sigmaRange)
   {
      sigma->setPointElement(i, 0, sigmaRange.clamp(v));
   }

   T getSigma(const MeshIndex i) const
   {
      return sigma->getPointElement(i, 0);
   }

   VerticesNearestVerticesSearch<T>::P euclideanSearcher;

   // Using a Euclidean distance
   void getNearbyParticleDistancesE(const MeshIndex particle, const T maxDistance, DistanceDirectionMap& ddm, bool calculateDirections = false)
   {
      T vtx[3];
      T vtxOther[3];
      Direction di = { 0, 0, 0 };

      euclideanSearcher->reset();

      particles->getPoint(particle, vtx);
      euclideanSearcher->setTestPoint(vtx);
      euclideanSearcher->setRadius(maxDistance);

      particlesMesh->search(euclideanSearcher);

      ddm.clear();

      for (MeshIndex i = 0, imax = euclideanSearcher->getNumberOfPointsFound(); i < imax; ++i)
      {
         MeshIndex v = euclideanSearcher->getPointFoundIndex(i);

         particles->getPoint(v, vtxOther);

         FOREACHAXIS(ax)
            vtxOther[ax] -= vtx[ax];

         const T len = normalize<T>(vtxOther, 3);
         if (calculateDirections)
         {
            di.x = vtxOther[0];
            di.y = vtxOther[1];
            di.z = vtxOther[2];
         }
         else
         {
            di.x = di.y = di.z = 0;
         }

         ddm[v] = DistanceDirection(len, di);
      }
   }

   // Using a geodesic distance
   void getNearbyParticleDistancesG(const MeshIndex particle, const T maxDistance, DistanceDirectionMap& ddm, bool calculateDirections = false)
   {
      T vtx[3];

      particles->getPoint(particle, vtx);
      geo->setOrigin(vtx, particleFaceIndex[particle], maxDistance);

      ddm.clear();

      T dir[] = { 0, 0, 0 };
      Direction di = { 0, 0, 0 };

      const MeshIndex nfaces = geo->getNumberOfFoundCells();

      Idx cnt = 0;

      for (MeshIndex fi = 0; fi < nfaces; ++fi)
      {
         // This cell is inside the radius
         const MeshIndex f = geo->getFoundCell(fi);

         // Find all the points which were associated to it on the last reprojection
         for (std::vector<MeshIndex>::iterator it = particlesOnFace[f].begin(), en = particlesOnFace[f].end(); it != en; ++it)
         {
            particles->getPoint(*it, vtx);
            //PRINTVEC2("other", vtx,3);

            const T len = geo->distanceTo(vtx, f);

            // We may not actually need directions, e.g. for optimizing sigma
            if (calculateDirections)
            {
               geo->directionTo(vtx, f, dir, false);

               di.x = dir[0];
               di.y = dir[1];
               di.z = dir[2];
            }

            ddm[*it] = DistanceDirection(len, di);

            ++cnt;
         }
      }
   }

   void getNearbyParticleDistances(const MeshIndex i, const T maxDistance, DistanceDirectionMap& ddm, bool calculateDirections = false)
   {
      switch (distanceType)
      {
      case DT_APPROXGEODESIC:
         getNearbyParticleDistancesG(i, maxDistance, ddm, calculateDirections);
         break;
      case DT_EUCLIDEAN:
         getNearbyParticleDistancesE(i, maxDistance, ddm, calculateDirections);
         break;
      default:
         throw "Invalid distance type";
      }
   }

   void buildAdjacency(Adjacency& adj)
   {
      //DEBUGMACRO("Starting to build adjacency matrix...");
      const MeshIndex N = particles->getNumberOfPoints();
      
      adj.prepare(N);

      for (MeshIndex k = 0; k < N; ++k)
      {
         Idx cnt = 0;

         // Get the current estimate
         const T sigmaK = getSigma(k);
         const T cutoff = 3*sigmaK;

         // Rebuild the DistanceDirectionMap map with directions
         DistanceDirectionMap ddm;
         getNearbyParticleDistances(k, cutoff, ddm, true);

         // Go through each of the nearby points found
         for (DistanceDirectionMap::iterator it = ddm.begin(), en = ddm.end(); it != en; ++it)
         {
            const MeshIndex         j  = it->first;
            const DistanceDirection dd = it->second;

            if (j != k || CATESREG_USESELF)
            {
               const T wt = truncatedIsotropicGaussian<T>(dd.first*dd.first, sigmaK, cutoff, 3);

               DistanceDirectionWeight ddw = { dd.first, dd.second, wt };

               adj.insert(k, j, ddw);
               ++cnt;
            }
         }

         if (cnt == 0)
         {
            PRINTVBL(k);
            PAUSE;
         }
      }
   }

   void optimizeSigma()
   {
      if (fixSigma)
      {
         //DEBUGMACRO("Sigma is fixed");
         return;
      }
      const MeshIndex N = particles->getNumberOfPoints();

      DistanceDirectionMap ddm;

      ValueRange<T> tmpSigmaRange = { sigmaRange.upper, sigmaRange.lower };

      for (MeshIndex k = 0; k < N; ++k)
      {
         T sigmaK = getSigma(k);
         
         T old3Sigma = 0;
         T lastSigma = sigmaK;

         // Use Newton's method to iteratively update the value of sigma
         MeshIndex iters;
         ddm.clear();
         for (iters = 0; iters < maxSigmaIters; ++iters)
         {
            // Get the particle distances, truncated at 3*current sigma value
            // Only need to do this if sigmaK has increased
            if (3*sigmaK > old3Sigma)
            {
               // Make the range a little bigger (1.5x) so that if sigma increases, we don't need to recalc each time
               getNearbyParticleDistances(k, old3Sigma = 4.5*sigmaK, ddm, false);
            }

            T num = 0;
            T den = 0;

            // Go through each of the nearby points found
            for (DistanceDirectionMap::iterator it = ddm.begin(), en = ddm.end(); it != en; ++it)
            {
               const MeshIndex         j  = it->first;
               const DistanceDirection dd = it->second;

               if (j != k || CATESREG_USESELF)
               {
                  const T wt = truncatedIsotropicGaussian<T>(dd.first*dd.first, sigmaK, 3*sigmaK, 3);

                  num += wt * dd.first*dd.first;
                  den += wt;
               }
            }

            lastSigma = sigmaK;

            if (den != 0)
            {
               // Calculate the updated value
               sigmaK = sqrt(num / (3 * den));

               if (debug) PRINTVBL(sigmaK);

               // If sigma has gone out of the allowed range, then stop
               
               if (sigmaK < old3Sigma && sigmaK <= sigmaRange.lower)
                  break;

               if (sigmaK > old3Sigma && sigmaK >= sigmaRange.upper)
                  break;

               if (abs(sigmaK-lastSigma) < 1e-3)
                  break;
            }
            else
            {
               // This means that no nearby particles were found in this radius
               // so increase sigma
               sigmaK = 2*sigmaK;

               if (sigmaK > sigmaRange.upper)
                  break;
            }
         }

         setSigma(k, sigmaK, sigmaRange);
         
         tmpSigmaRange.lower = std::min(tmpSigmaRange.lower, sigmaK);
         tmpSigmaRange.upper = std::max(tmpSigmaRange.upper, sigmaK);
      }

      actualSigmaRange.lower = std::max(sigmaRange.lower, tmpSigmaRange.lower);
      actualSigmaRange.upper = std::min(sigmaRange.upper, tmpSigmaRange.upper);

      {
         ColouredConsole cons(12);
         DEBUGMACRO("Set sigma range: " << sigmaRange.lower       << " -> " << sigmaRange.upper);
         DEBUGMACRO("Sigma range: "     << actualSigmaRange.lower << " -> " << actualSigmaRange.upper);
      }

      //PAUSE;
   }

   void associateToFace(MeshIndex vertex, MeshIndex face)
   {
      const MeshIndex undefined = std::numeric_limits<MeshIndex>::max();

      // Is there currently a list of vertices on this face?
      if (particlesOnFace.find(face) == particlesOnFace.end())
         particlesOnFace[ face ] = std::vector<MeshIndex>();

      // Is the vertex currently associated to a face?
      std::map<MeshIndex,MeshIndex>::iterator fn = particleFaceIndex.find(vertex);
      if (fn != particleFaceIndex.end() && fn->second != undefined)
      {
         // Don't actually need to do anything
         if (fn->second == face)
            return;

         // Need to get rid of this face from the list of vertices on this face
         std::vector<MeshIndex>::iterator it = particlesOnFace[face].begin();
         while (it != particlesOnFace[face].end() && *it != vertex)
            ++it;

         if (it != particlesOnFace[face].end())
         {
            particlesOnFace[face].erase(it, it+1);
            particleFaceIndex[vertex] = undefined;
         }
      }

      if (particlesOnFace.find(face) == particlesOnFace.end())
         particlesOnFace[ face ] = std::vector<MeshIndex>();

      particleFaceIndex[vertex] = face;
      particlesOnFace[face].push_back(vertex);
   }

   T calculateNegativeEntropy(Adjacency& adj)
   {
      const MeshIndex N = particles->getNumberOfPoints();

      T logSum = 0;
      for (MeshIndex i = 0; i < N; ++i)
      {
         if (adj.weightsRowSum(i) == 0)
         {
            DEBUGMACRO(i << "\t" << adj.weightsRowSum(i));
         }

         logSum += log(adj.weightsRowSum(i) / N);
      }

      const T ret = -logSum/* / N*/;

      return ret;
   }

};

AWT::AlignParametric::CatesRegularizer::CatesRegularizer(CatesParticleSurface* surf, const ValueRange<T> sigmaRange)
{
   m_D = new D;

   m_D->euclideanSearcher = VerticesNearestVerticesSearch<T>::getInstance();

   m_D->maxSigmaIters = 20;

   m_D->surf            = surf;
   m_D->meshSurfaceArea = MeshFunctions<T>::getSurfaceArea(surf->getMesh());

   m_D->particles = surf->getSamples();

   m_D->distanceType = DT_EUCLIDEAN;
   
   // Construct a mesh around the particles for easier searching
   m_D->particlesMesh = MeshImpl<T>::getInstance(0, 0);
   m_D->particlesMesh->setVertices(m_D->particles);

   // Construct the geodesic distance calculator
   m_D->geo = MeshApproxGeodesic<T>::getInstance(surf->getMesh());

   m_D->sigma = TuplesImpl<T>::getInstance(1, m_D->particles->getNumberOfPoints());
   m_D->sigmaRange = sigmaRange;

   // Calculate a reasonable estimate for the initial sigma
   const T surfArea = MeshFunctions<T>::getSurfaceArea(surf->getMesh());
   const T areaPerSample = surfArea / m_D->particles->getNumberOfPoints();
   const T initSigma = sqrt(areaPerSample / (4 * atan(1.0)));

   PRINTVBL(initSigma);

   const T zero[] = { 0, 0, 0 };
   for (MeshIndex v = 0, vmax = m_D->particles->getNumberOfPoints(); v < vmax; ++v)
   {
      m_D->setSigma(v, initSigma /*sigmaRange.upper*/, sigmaRange);
   }

   m_D->fixSigma = false;

   DEBUGMACRO("Optimizing sigma");
   m_D->optimizeSigma();
   DEBUGMACRO("Done.");
   
}

AWT::AlignParametric::CatesRegularizer::~CatesRegularizer()
{
   delete m_D;
}

AWT::AlignParametric::CatesRegularizer::P AWT::AlignParametric::CatesRegularizer::getInstance(CatesParticleSurface* surf, const ValueRange<T> sigmaRange)
{
   AUTOGETINSTANCE(AWT::AlignParametric::CatesRegularizer, (surf, sigmaRange));
}

GETNAMEMACRO(AWT::AlignParametric::CatesRegularizer);

T AWT::AlignParametric::CatesRegularizer::getSigma(const MeshIndex i) const
{
   return m_D->getSigma(i);
}

void AWT::AlignParametric::CatesRegularizer::setSigma(const MeshIndex i, const T v)
{
   while (m_D->sigma->getNumberOfPoints() <= i)
      m_D->sigma->setPoint(m_D->sigma->getNumberOfPoints(), &m_D->sigmaRange.upper);

   m_D->setSigma(i, v, m_D->sigmaRange);
}

TuplesType::P AWT::AlignParametric::CatesRegularizer::getSigma()
{
   return m_D->sigma;
}

T AWT::AlignParametric::CatesRegularizer::getMinSigma() const
{
   return m_D->actualSigmaRange.lower;
}

const AWT::ValueRange<T>& AWT::AlignParametric::CatesRegularizer::getSigmaRange() const
{
   return m_D->sigmaRange;
}

void AWT::AlignParametric::CatesRegularizer::setSigmaRange(const AWT::ValueRange<T>& sigmaRange)
{
   m_D->sigmaRange.lower = sigmaRange.lower;
   m_D->sigmaRange.upper = sigmaRange.upper;
}

T AWT::AlignParametric::CatesRegularizer::calculateCost(const bool debug)
{
   m_D->particlesMesh->prepareToSearchVertices(true);

   // Update the optimal sigmas
   m_D->optimizeSigma();

   m_D->adj = D::Adjacency();
   m_D->buildAdjacency(m_D->adj);

   const T ret = m_D->calculateNegativeEntropy(m_D->adj);

   return ret;
}

T AWT::AlignParametric::CatesRegularizer::calculateNegativeEntropy()
{
   m_D->particlesMesh->prepareToSearchVertices(true);

   m_D->adj = D::Adjacency();
   m_D->buildAdjacency(m_D->adj);

   const T ret = m_D->calculateNegativeEntropy(m_D->adj);

   return ret;
}

void AWT::AlignParametric::CatesRegularizer::calculateUpdate(MatrixType& delta)
{
   D::Adjacency& adj = m_D->adj;
   const MeshIndex N = m_D->particles->getNumberOfPoints();

   if (delta.rows() != N || delta.cols() != 3)
      delta.set_size(N, 3);

   T vtxDelta[3];
   T vtxDelta2[3];
   T vtxDelta_combined[3];
   T num[3];
   T denom;

   {
      Idx di = 0;

      for (MeshIndex k = 0; k < N; ++k)
      {
         //PRINTVBL2("kkk", k);

         const T sigmaK = m_D->getSigma(k);
         const D::DDWRow& thisDDWRow = adj.ddwMatrix[k];
         const D::AdjRow& thisAdjRow = adj.adjMatrix[k];

         // Reset the accumulators
         vtxDelta[0] = vtxDelta[1] = vtxDelta[2] = 0;

         num[0] = num[1] = num[2] = 0;
         denom = 0;

         // This is the term given by Cates
         for (D::DDWRow::const_iterator it = thisDDWRow.begin(), en = thisDDWRow.end(); it != en; ++it)
         {
            const MeshIndex j                     = it->first;
            const D::DistanceDirectionWeight& ddw = it->second;

            const T wt = ddw.weight;
            denom += wt;

            if (j != k)
            {
               // -ve, because particle needs to move away from other particles
               num[0] -= wt * ddw.distance * ddw.direction.x;
               num[1] -= wt * ddw.distance * ddw.direction.y;
               num[2] -= wt * ddw.distance * ddw.direction.z;
            }
         }

         if (denom != 0)
         {
            for (MeshIndex ax = 0; ax < 3; ++ax)
               vtxDelta[ax] = num[ax] / denom / (sigmaK * sigmaK);
         }

         // This term isn't present in Cates' derivation
         vtxDelta2[0] = vtxDelta2[1] = vtxDelta2[2] = 0;

         if (true)
         {
            for (D::AdjRow::const_iterator it = thisAdjRow.begin(), en = thisAdjRow.end(); it != en; ++it)
            {
               const Idx i = *it;
               if (i==k) continue;

               const D::DistanceDirectionWeight& ddw = adj.ddwMatrix[i][k];

               if (adj.weightsRowSum(k) != 0)
               {
                  const T sigmaI = m_D->getSigma(i);

                  num[0] = ddw.weight * ddw.distance * ddw.direction.x;
                  num[1] = ddw.weight * ddw.distance * ddw.direction.y;
                  num[2] = ddw.weight * ddw.distance * ddw.direction.z;

                  denom = adj.weightsRowSum(i);

                  for (MeshIndex ax = 0; ax < 3; ++ax)
                     vtxDelta2[ax] += num[ax] / denom / (sigmaI * sigmaI);
               }
            }
         }

         for (MeshIndex ax = 0; ax < 3; ++ax)
         {
            vtxDelta_combined[ax] = vtxDelta[ax] + vtxDelta2[ax];
            delta(k, ax) = vtxDelta_combined[ax];
         }

         //delta(k, 3) = 0;
      }
   }
}

void AWT::AlignParametric::CatesRegularizer::associateSampleToFace(const AWT::MeshIndex i, const AWT::MeshIndex f)
{
   m_D->associateToFace(i, f);
}

CatesRegularizer::DistanceType AWT::AlignParametric::CatesRegularizer::getDistanceType() const
{
   return m_D->distanceType;
}

void AWT::AlignParametric::CatesRegularizer::setDistanceType(const CatesRegularizer::DistanceType v)
{
   if (v != m_D->distanceType)
   {
      m_D->distanceType = v;
      modified();
   }
}

void AWT::AlignParametric::CatesRegularizer::fixSigma(const bool state)
{
   m_D->fixSigma = state;
}