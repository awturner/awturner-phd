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
#include "MeshApproxGeodesic.h"

#include <fstream>
#include "Mesh/MeshConnectivity.h"
#include "Useful/ArrayVectorFunctions.h"

#include "Useful/Exception.h"

#include <vector>
#include <map>
#include <set>
#include <queue>
#include <limits>

template <class T>
struct AWT::MeshApproxGeodesic<T>::D
{
   typedef std::pair<MeshIndex,MeshIndex> Parents;
   typedef std::pair<MeshIndex,Parents>   ChildAndParents;
   typedef std::pair<T,ChildAndParents>   QueuePair;

   typedef std::map< MeshIndex, bool > VisitedMap;
   typedef std::priority_queue<QueuePair, std::vector<QueuePair>, std::greater<QueuePair> > PriorityQueue;

   static const T farAway;
   static const MeshIndex undefined;

   T getVertexDistance(const MeshIndex i)
   {
      std::map<MeshIndex,T>::iterator it = vertexDistances.find(i);

      // If the distance isn't yet fixed, return infinity
      if (it == vertexDistances.end())
         return farAway;

      return it->second;
   }

   T setVertexDistance(const MeshIndex i, const T d, Parents parents)
   {
      vertexDistances[ i ] = d;
      vertexParents[ i ] = parents;
      return d;
   }

   // Normalize the basis
   void normalizeBasis(T vtx[3][3], bool debug = false)
   {
      T axx[3][3];

      // Translate relative to first point
      for (MeshIndex ax = 0; ax < 3; ++ax)
      {
         vtx[1][ax] -= vtx[0][ax];
         vtx[2][ax] -= vtx[0][ax];
         vtx[0][ax] -= vtx[0][ax];
      }

      cross(vtx[1], vtx[2], vtx[0]);
      if (dot(vtx[0], vtx[0], 3) == 0)
      {
         // The vectors are colinear, so can't do anything but 
         // put them on the x-axis
         vtx[1][0] = normalize<T>(vtx[1], 3);
         vtx[1][1] = vtx[1][2] = 0;

         vtx[2][0] = normalize<T>(vtx[2], 3);
         vtx[2][1] = vtx[2][2] = 0;

         vtx[0][0] = vtx[0][1] = vtx[0][2] = 0;

         return;
      }

      // Calculate the basis axes
      for (MeshIndex ax = 0; ax < 3; ++ax)
         axx[0][ax] = vtx[1][ax];
      const T x1 = normalize<T>(axx[0], 3);

      cross<T>(vtx[1], vtx[2], axx[2]);
      normalize<T>(axx[2], 3);

      cross<T>(axx[2], axx[0], axx[1]);

      // Now project the child point onto this basis
      // (already know the coordinates of the parent)
      T vtxProj[3];
      for (MeshIndex ax = 0; ax < 3; ++ax)
         vtxProj[ax] = axx[ax][0]*vtx[2][0] + axx[ax][1]*vtx[2][1] + axx[ax][2]*vtx[2][2];
      
      // Rewrite the coordinates
      for (MeshIndex ax = 0; ax < 3; ++ax)
      {
         vtx[0][ax] = 0;
         vtx[1][ax] = 0;
         vtx[2][ax] = vtxProj[ax];
      }
      vtx[1][0] = x1;

   }

   MeshIndex hiddenCheck(const T vtx[3][3], const T vtxS[3])
   {
      T vec1[3], vec2[3], vecCross[3];

      for (MeshIndex pnt = 0; pnt < 2; ++pnt)
      {
         for (MeshIndex ax = 0; ax < 3; ++ax)
         {
            vec1[ax] = vtxS[ax] - vtx[0][ax];
            vec2[ax] = vtx[2][ax] - vtx[0][ax];
         }

         cross<T>(vec1, vec2, vecCross);

         if (vecCross[2] < 0)
            return pnt;
      }

      return 2;
   }

   T calculateVertexDistance(const ChildAndParents& cp)
   {
      T vtx[3][3];

      // Get the 3 corner vertices
      mesh->getVertex(cp.second.first,  vtx[0]);
      mesh->getVertex(cp.second.second, vtx[1]);
      mesh->getVertex(cp.first,         vtx[2]);

      // These are the distances to the parent vertices
      const T d[] = { getVertexDistance(cp.second.first), getVertexDistance(cp.second.second) };

      return calculateVertexDistance(vtx, d);
   }

   T calculateVertexDistance(T vtx[3][3], const T d[2])
   {
      T ret;

      // Save some work if the points are far away
      if (d[0] == farAway)
      {
         ret = d[1] + deltaNorm(vtx[1], vtx[2], 3);
      }
      else if (d[1] == farAway)
      {
         ret = d[0] + deltaNorm(vtx[0], vtx[2], 3);
      }
      else
      {
         // Destructively project
         normalizeBasis(vtx);

         // Always take the -y root because the far point should always be +y
         const T xs = (d[0]*d[0] - d[1]*d[1] + vtx[1][0]*vtx[1][0]) / (2 * vtx[1][0]);
         const T ys = -sqrt(std::max<T>(0, d[0]*d[0] - xs*xs));

         const T vtxS[] = { xs, ys, 0 };

         // Now need to check whether the virtual source is "hidden"
         const MeshIndex hideVertex = hiddenCheck(vtx, vtxS);
         
         switch (hideVertex)
         {
         case 0:
            ret = d[0] + deltaNorm<T>(vtx[0], vtx[2], 3);
            break;
         case 1:
            ret = d[1] + deltaNorm<T>(vtx[1], vtx[2], 3);
            break;
         default:
            ret = deltaNorm<T>(vtx[2], vtxS, 3);
         }
      }

      return ret;
   }

   void convertChildAndParents(const ChildAndParents& cp, T vtx[3][3], T d[2])
   {
      mesh->getVertex(cp.second.first,  vtx[0]);
      mesh->getVertex(cp.second.second, vtx[1]);
      mesh->getVertex(cp.first,         vtx[2]);

      // These are the distances to the parent vertices
      d[0] = getVertexDistance(cp.second.first);
      d[1] = getVertexDistance(cp.second.second);
   }

   T calculateSegmentCrossing(T vtx[3][3], const T d[2], T& alpha, T& sourceToCrossing, T& crossingToTarget)
   {
      // Destructively project
      normalizeBasis(vtx, true);

      // Always take the -y root because the far point should always be +y
      const T xs = (vtx[0][0] != vtx[1][0]) ? ((d[0]*d[0] - d[1]*d[1] + vtx[1][0]*vtx[1][0]) / (2 * vtx[1][0])) : 0;
      const T ys = -sqrt(std::max<T>(0, d[0]*d[0] - xs*xs));

      const T vtxS[] = { xs, ys, 0 };

      if (vtx[2][1] - vtxS[1] == 0 || vtx[1][0] == 0)
      {
         if (vtx[2][0] < 0 && vtxS[0] < 0)
            alpha = 0;
         else if (vtx[2][0] > vtx[1][0] && vtxS[0] > vtx[1][0])
            alpha = 1;
         else
            alpha = (vtxS[0] + vtx[1][0]) / 2;
      }
      else
      {
         alpha = (vtxS[0] - vtxS[1] * (vtx[2][0] - vtxS[0]) / (vtx[2][1] - vtxS[1])) / vtx[1][0];
      }

      // Clamp between the two end points
      alpha = std::max<T>(std::min<T>(alpha, 1), 0);

      T vtxCrossing[3];
      for (MeshIndex ax = 0; ax < 3; ++ax)
         vtxCrossing[ax] = (1-alpha)*vtx[0][ax] + alpha*vtx[1][ax];

      sourceToCrossing = deltaNorm(vtxCrossing, vtxS, 2);
      crossingToTarget = deltaNorm(vtxCrossing, vtx[2], 2);

      return sourceToCrossing + crossingToTarget;
   }

   T calculateSegmentMinimum(const ChildAndParents& cp)
   {
      T vtx[3][3];
      T d[2];
      convertChildAndParents(cp, vtx, d);
      return calculateSegmentMinimum(vtx, d);
   }

   // This is the minimum possible value of the window in between the two end points
   // It is not sufficient to check whether the end points are greater than the max dist,
   // because the section in the middle might be less
   T calculateSegmentMinimum(T vtx[3][3], const T d[2])
   {
      normalizeBasis(vtx);

      const T xs = (d[0]*d[0] - d[1]*d[1] + vtx[1][0]*vtx[1][0]) / (2 * vtx[1][0]);
      const T ys = -sqrt(std::max<T>(0, d[0]*d[0] - xs*xs));

      const T vtxS[] = { xs, ys, 0 };

      if (vtxS[0] < 0)
      {
         return deltaNorm<T>(vtx[0], vtxS, 2);
      }
      else if (vtxS[0] > vtx[1][0])
      {
         return deltaNorm<T>(vtx[1], vtxS, 2);
      }
      else
      {
         return vtxS[1]*vtxS[1];
      }
   }

   QueuePair makeQueuePair(const MeshIndex child, const MeshIndex parent1, const MeshIndex parent2)
   {
      ChildAndParents cp(child, D::Parents(parent1, parent2));
      return QueuePair(calculateVertexDistance(cp), cp);
   }

   MeshIndex getAdjacentFaces(const MeshIndex f, MeshIndex* fs)
   {
      MeshIndex vs[3], fue[10];
      mesh->getFaceIndices(f, vs);

      MeshIndex ret = 0;
      for (MeshIndex i = 0; i < 3; ++i)
      {
         const MeshIndex nf = conn->getFacesUsingEdge(vs[i], vs[(i+1)%3], fue);

         for (MeshIndex ii = 0; ii < nf; ++ii)
         {
            if (fue[ii] != f)
            {
               fs[ret++] = fue[ii];
            }
         }
      }

      return ret;
   }

   void pushAdjacentFaces(PriorityQueue& q, VisitedMap& visitedFaces, const MeshIndex f)
   {
      MeshIndex vs[3], vvs[3];
      MeshIndex fs[10];
      mesh->getFaceIndices(f, vs);

      for (MeshIndex i = 0; i < 3; ++i)
      {
         const MeshIndex nf = conn->getFacesUsingEdge(vs[i], vs[(i+1)%3], fs);

         for (MeshIndex ii = 0; ii < nf; ++ii)
         {
            visitedFaces[ fs[ii] ] = true;

            if (fs[ii] != f)
            {
               mesh->getFaceIndices(fs[ii], vvs);

               MeshIndex p;
               for (p = 0; p < 3 && !(vvs[p] != vs[i] && vvs[p] != vs[(i+1)%3]); ++p);

               bool push = false;
               if (getVertexDistance(vvs[p]) == farAway)
               {
                  push = true;
               }
               else
               {
                  // Look at the faces adjacent to fs[ii] to see if any are unvisited
                  MeshIndex adjfaces[10];
                  const MeshIndex nadj = getAdjacentFaces(fs[ii], adjfaces);
                  
                  MeshIndex a;
                  for (a = 0; a < nadj; ++a)
                  {
                     if (visitedFaces.find(adjfaces[a]) == visitedFaces.end())
                        break;
                  }

                  if (a != nadj)
                     push = true;
               }

               if (push)
                  q.push(makeQueuePair(vvs[p], vs[i], vs[(i+1)%3]));
            }
         }
      }
   }

   void interpolate(const MeshIndex va, const MeshIndex vb, const T alpha, T vtxNew[3])
   {
      T vtxA[3], vtxB[3];
      mesh->getVertex(va, vtxA);
      mesh->getVertex(vb, vtxB);

      for (MeshIndex ax = 0; ax < 3; ++ax)
         vtxNew[ax] = (1 - alpha)*vtxA[ax] + alpha*vtxB[ax];
   }

   void updateTrackpoints(T vtxPrev[3], T vtxNow[3], const T vtxNew[3])
   {
      for (MeshIndex ax = 0; ax < 3; ++ax)
      {
         vtxPrev[ax] = vtxNow[ax];
         vtxNow[ax] = vtxNew[ax];
      }
   }

   MeshIndex pickCloserVertex(const T vtxNow[3], const MeshIndex* possibles, const MeshIndex nposs)
   {
      MeshIndex ret = undefined;
      T minDist = farAway;

      T vtx[3];

      for (MeshIndex i = 0; i < nposs; ++i)
      {
         mesh->getVertex(possibles[i], vtx);
         const T dist = getVertexDistance(possibles[i]) + deltaNorm<T>(vtx, vtxNow, 3);

         if (dist < minDist)
         {
            ret = possibles[i];
            minDist = dist;
         }
      }

      return ret;
   }

   typename AWT::Mesh<T>::P mesh;
   typename AWT::MeshConnectivity<T>::P conn;

   std::vector<MeshIndex> foundFaces;
   std::map<MeshIndex,T>  vertexDistances;

   std::map<MeshIndex,Parents> vertexParents;

   MeshIndex sourceCell;
   T sourcePoint[3];

   std::vector<QueuePair> nextQueuePairs;
};

template <class T>
const T AWT::MeshApproxGeodesic<T>::D::farAway = std::numeric_limits<T>::infinity();

template <class T>
const AWT::MeshIndex AWT::MeshApproxGeodesic<T>::D::undefined = std::numeric_limits<MeshIndex>::max();

template <class T>
AWT::MeshApproxGeodesic<T>::MeshApproxGeodesic(typename AWT::Mesh<T>::P mesh)
{
   m_D = new D;

   m_D->mesh = mesh;
   m_D->conn = MeshConnectivity<T>::getInstance(mesh);
}

template <class T>
AWT::MeshApproxGeodesic<T>::~MeshApproxGeodesic()
{
   delete m_D;
}

template <class T>
typename AWT::MeshApproxGeodesic<T>::P AWT::MeshApproxGeodesic<T>::getInstance(typename AWT::Mesh<T>::P mesh)
{
   AUTOGETINSTANCE(AWT::MeshApproxGeodesic<T>, (mesh));
}

template <class T>
GETNAMEMACRO(AWT::MeshApproxGeodesic<T>);

#include "MeshSearching/FacesNearestPointSearch.h"

template <class T>
void AWT::MeshApproxGeodesic<T>::setOrigin(const T vtxStart[3], const T distMax)
{
   FacesNearestPointSearch<T>::P searcher = FacesNearestPointSearch<T>::getInstance();
   searcher->reset();
   searcher->setTestPoint(vtxStart);

   m_D->mesh->search(searcher);

   T vtxNearest[3];
   const int np = searcher->getNearestPoint(vtxNearest);

   setOrigin(vtxStart, np, distMax);
}

template <class T>
void AWT::MeshApproxGeodesic<T>::setOrigin(const T vtxStart[3], const MeshIndex np, const T distMax)
{
   m_D->sourceCell = np;
   for (MeshIndex ax = 0; ax < 3; ++ax)
      m_D->sourcePoint[ax] = vtxStart[ax];

   m_D->foundFaces.clear();
   m_D->vertexDistances.clear();
   m_D->vertexParents.clear();

   D::PriorityQueue q;

   MeshIndex vs[3];
   T vtx[3];

   m_D->mesh->getFaceIndices(np, vs);

   // Mark the 3 vertices of the current triangle as fixed
   for (MeshIndex vi = 0; vi < 3; ++vi)
   {
      // Mark the vertex as fixed by setting its distance
      m_D->mesh->getVertex(vs[vi], vtx);
      const T dist = deltaNorm(vtxStart, vtx, 3);

      // Record the parents of these vertices as "undefined", because they 
      // aren't defined in the same way as for other vertices
      m_D->setVertexDistance(vs[vi], dist, D::Parents(D::undefined, D::undefined));
   }

   D::VisitedMap visitedFaces;

   // Push the starting face into the queue
   m_D->pushAdjacentFaces(q, visitedFaces, np);

   while (!q.empty())
   {
      // Get the top element on the queue
      const D::QueuePair qp = q.top();
      q.pop();

      const T thisDist      = qp.first;
      const MeshIndex thisV = qp.second.first;

      // If this distance has been set already, do nothing and move on
      if (m_D->getVertexDistance(thisV) == D::farAway)
      {
         // This is the best distance for the vertex, so record this as the best distance
         m_D->setVertexDistance(thisV, thisDist, qp.second.second);
      }

      const MeshIndex selfF = m_D->conn->getFaceIndex(thisV, qp.second.second.first, qp.second.second.second);
      const T segMin = m_D->calculateSegmentMinimum(qp.second);

      if (segMin < distMax)
      {
         // Push this face (or rather the adj faces) into the queue
         m_D->pushAdjacentFaces(q, visitedFaces, selfF);
      }
   }

   // Transfer the list of visited faces into the found faces
   for (D::VisitedMap::iterator it = visitedFaces.begin(), en = visitedFaces.end(); it != en; ++it)
      m_D->foundFaces.push_back(it->first);
}

template <class T>
AWT::MeshIndex AWT::MeshApproxGeodesic<T>::getNumberOfFoundCells() const
{
   return static_cast<AWT::MeshIndex>(m_D->foundFaces.size());
}

template <class T>
AWT::MeshIndex AWT::MeshApproxGeodesic<T>::getFoundCell(const MeshIndex i) const
{
   return m_D->foundFaces[i];
}

template <class T>
T AWT::MeshApproxGeodesic<T>::distanceTo(const T vtxEnd[3]) const
{
   FacesNearestPointSearch<T>::P searcher = FacesNearestPointSearch<T>::getInstance();
   searcher->reset();
   searcher->setTestPoint(vtxEnd);

   m_D->mesh->search(searcher);

   T vtxNearest[3];
   const int np = searcher->getNearestPoint(vtxNearest);

   return distanceTo(vtxEnd, np);
}

template <class T>
T AWT::MeshApproxGeodesic<T>::distanceTo(const T vtxEnd[3], const AWT::MeshIndex cell) const
{
   if (cell == m_D->sourceCell)
   {
      return deltaNorm(m_D->sourcePoint, vtxEnd, 3);
   }

   MeshIndex vs[3];
   m_D->mesh->getFaceIndices(cell, vs);

   T ret = D::farAway;
   T vtx[3][3];
   T ds[2];
   // Need to work out which the best side to come at this point from
   for (MeshIndex vi = 0; vi < 3; ++vi)
   {
      for (MeshIndex vii = 0; vii < 2; ++vii)
      {
         m_D->mesh->getVertex(vs[(vi+vii)%3], vtx[vii]);
         ds[vii] = m_D->getVertexDistance(vs[(vi+vii)%3]);
      }

      // Fast break; distance to point must be greater than distance to vertices
      if (ds[0] >= ret && ds[1] >= ret)
         continue;

      for (MeshIndex ax = 0; ax < 3; ++ax)
         vtx[2][ax] = vtxEnd[ax];

      ret = std::min(ret, m_D->calculateVertexDistance(vtx, ds));
   }

   //PRINTVBL(ret);

   return ret;
}

template <class T>
void AWT::MeshApproxGeodesic<T>::directionTo(const T vtxEnd[3], T dir[3], const bool debug) const
{
   FacesNearestPointSearch<T>::P searcher = FacesNearestPointSearch<T>::getInstance();
   searcher->reset();
   searcher->setTestPoint(vtxEnd);

   m_D->mesh->search(searcher);

   T vtxNearest[3];
   const int np = searcher->getNearestPoint(vtxNearest);

   return directionTo(vtxEnd, np, dir);
}

#if 1

template <class T>
void AWT::MeshApproxGeodesic<T>::directionTo(const T vtxEnd[3], const MeshIndex endcell, T dir[3], const bool debug) const
{
   T vtxPrev[3];
   T vtxNow[3];

   // Get the end position as our current position
   FOREACHAXIS(ax)
      vtxPrev[ax] = vtxNow[ax] = vtxEnd[ax];
   
   MeshIndex edgeNow[2];

   T vtxs[3][3];
   T ds[2];

   MeshIndex vs[3];
   m_D->mesh->getFaceIndices(endcell, vs);
   MeshIndex i;
   
   if (endcell == m_D->sourceCell)
   {
      // The easiest case to handle!  They're on the same face, so direction is just difference
      T lenSq = 0;
      for (MeshIndex ax = 0; ax < 3; ++ax)
      {
         dir[ax] = vtxEnd[ax] - m_D->sourcePoint[ax];
         lenSq += dir[ax]*dir[ax];
      }

      if (lenSq != 0)
      {
         const T len = sqrt(lenSq);
         for (MeshIndex ax = 0; ax < 3; ++ax)
            dir[ax] /= len;
      }

      return;
   }

   for (i = 0; i < 3; ++i)
   {
      // Get the parents of this vertex
      D::Parents parents = m_D->vertexParents[ vs[i] ];

      if ((parents.first == vs[(i+1)%3] && parents.second == vs[(i+2)%3]) || (parents.first == vs[(i+2)%3] && parents.second == vs[(i+1)%3]))
      {
         // Great, we're inside a good triangle
         m_D->mesh->getVertex(parents.first,  vtxs[0]);
         m_D->mesh->getVertex(parents.second, vtxs[1]);

         ds[0] = m_D->getVertexDistance(parents.first );
         ds[1] = m_D->getVertexDistance(parents.second);

         FOREACHAXIS(ax)
            vtxs[2][ax] = vtxNow[ax];
         
         T sourceToCrossing, crossingToTarget, alpha;
         const T dist = m_D->calculateSegmentCrossing(vtxs, ds, alpha, sourceToCrossing, crossingToTarget);

         if (alpha == 0 || alpha == 1)
         {
            edgeNow[0] = edgeNow[1] = (alpha == 0) ? parents.first : parents.second;
            m_D->mesh->getVertex(edgeNow[0], vtxNow);
         }
         else
         {
            edgeNow[0] = parents.first;
            edgeNow[1] = parents.second;

            m_D->mesh->getVertex(edgeNow[0], vtxs[0]);
            m_D->mesh->getVertex(edgeNow[1], vtxs[1]);
            FOREACHAXIS(ax)
               vtxNow[ax] = (1-alpha)*vtxs[0][ax] + alpha*vtxs[1][ax];
         }

         break;
      }
   }

   const bool trickyCase = i == 3;
   if (i == 3)
   {
      // We're in a bad triangle, i.e. one where there isn't a vertex which is a distance-child of the other two
      // This is a tricky case to handle properly, so just move to the nearest vertex and carry on
      edgeNow[0] = edgeNow[1] = m_D->pickCloserVertex(vtxNow, vs, 3);
      m_D->mesh->getVertex(edgeNow[0], vtxNow);
   }

   while (true)
   {
      //PRINTVEC(edgeNow, 2);

      if (deltaNormSquared(vtxNow, m_D->sourcePoint, 3) > 1e-8)
         FOREACHAXIS(ax) vtxPrev[ax] = vtxNow[ax];

      if (edgeNow[0] != edgeNow[1])
      {
         // We're on an edge, rather than on a vertex
         // Need to work out the correct parent
         
         // 3 Cases: 
         // * edgeNow[0] is a parent of edgeNow[1] -> use window formed by both parents of edgeNow[1]
         // * edgeNow[1] is a parent of edgeNow[0] -> use window formed by both parents of edgeNow[0]
         // * Neither                              -> Hmm, go to nearest vertex and use window formed by parents

         D::Parents parents[] = { m_D->vertexParents[ edgeNow[0] ], m_D->vertexParents[ edgeNow[1] ] };

         if (parents[1].first == edgeNow[0] || parents[1].second == edgeNow[0])
         {
            // edgeNow[0] is a parent of edgeNow[1]
            edgeNow[0] = edgeNow[1];
         }
         else if (parents[0].first == edgeNow[1] || parents[0].second == edgeNow[1])
         {
            // edgeNow[1] is a parent of edgeNow[0]
            edgeNow[1] = edgeNow[0];
         }
         else
         {
            // We're on a bad edge, i.e. one where there is no parent/child relationship.
            // Difficult to handle properly, so just move to the nearest vertex
            edgeNow[0] = edgeNow[1] = m_D->pickCloserVertex(vtxNow, edgeNow, 2);
         }
      }

      // We are at a vertex: move to its parent source
      const D::Parents& parents = m_D->vertexParents[ edgeNow[0] ];
      //DEBUGMACRO(parents.first << "\t" << parents.second);
      if (parents.first == D::undefined || parents.second == D::undefined)
      {
         // We've made it back to the starting cell
         // Take the edge ends themselves as the start
         m_D->mesh->getVertex(edgeNow[0], vtxs[0]);
         m_D->mesh->getVertex(edgeNow[1], vtxs[1]);

         ds[0] = m_D->getVertexDistance(edgeNow[0]);
         ds[1] = m_D->getVertexDistance(edgeNow[1]);
      }
      else
      {
         // Take the parents as the start
         m_D->mesh->getVertex(parents.first,  vtxs[0]);
         m_D->mesh->getVertex(parents.second, vtxs[1]);

         ds[0] = m_D->getVertexDistance(parents.first );
         ds[1] = m_D->getVertexDistance(parents.second);
      }

      FOREACHAXIS(ax)
         vtxs[2][ax] = vtxNow[ax];
      
      T sourceToCrossing, crossingToTarget, alpha;
      const T dist = m_D->calculateSegmentCrossing(vtxs, ds, alpha, sourceToCrossing, crossingToTarget);

      if (parents.first == D::undefined || parents.second == D::undefined)
      {
         if (sourceToCrossing > 1e-6)
         {
            m_D->mesh->getVertex(edgeNow[0], vtxs[0]);
            m_D->mesh->getVertex(edgeNow[1], vtxs[1]);
            FOREACHAXIS(ax)
               vtxNow[ax] = (1-alpha)*vtxs[0][ax] + alpha*vtxs[1][ax];
         }

         break;
      }
      else
      {
         if (alpha == 0 || alpha == 1)
         {
            edgeNow[0] = edgeNow[1] = (alpha == 0) ? parents.first : parents.second;
            m_D->mesh->getVertex(edgeNow[0], vtxNow);
         }
         else
         {
            edgeNow[0] = parents.first;
            edgeNow[1] = parents.second;

            m_D->mesh->getVertex(edgeNow[0], vtxs[0]);
            m_D->mesh->getVertex(edgeNow[1], vtxs[1]);
            FOREACHAXIS(ax)
               vtxNow[ax] = (1-alpha)*vtxs[0][ax] + alpha*vtxs[1][ax];
         }
      }
   }

   FOREACHAXIS(ax)
      dir[ax] = vtxPrev[ax] - m_D->sourcePoint[ax];

   if (dot(dir, dir, 3) != 0)
      normalize<T>(dir, 3);
}

#else

template <class T>
void AWT::MeshApproxGeodesic<T>::directionTo(const T vtxEnd[3], const MeshIndex endcell, T dir[3], const bool debug) const
{
   const T thresh = 1e-8;
   // Need to back trace from this cell to the target cell
   // Once there, will use the arrival path to determine the normal

   T vtxPrev[3];
   T vtxNow[3];

   //PRINTVBL(endcell);
   //PRINTVBL(m_D->sourceCell);

   for (MeshIndex ax = 0; ax < 3; ++ax)
      vtxPrev[ax] = vtxNow[ax] = vtxEnd[ax];

   MeshIndex vs[3];

   MeshIndex trackCells[] = { std::numeric_limits<MeshIndex>::max(), std::numeric_limits<MeshIndex>::max(), std::numeric_limits<MeshIndex>::max() };
   MeshIndex tracker = 0;

   MeshIndex cell = endcell;
   
   std::set<MeshIndex> visited;

   PRINTVBL(m_D->sourceCell);

   while (deltaNormSquared(vtxNow, m_D->sourcePoint, 3) > 1e-6 && cell != m_D->sourceCell)
   {
      T vtx[3];

      // Record that we've been to this cell
      //visited.insert(cell);

      /*
      if (debug)
         PRINTVBL(cell);
      */

      // Are we on a vertex or not?
      
      m_D->mesh->getFaceIndices(cell, vs);

      MeshIndex vi;
      for (vi = 0; vi < 3; ++vi)
      {
         m_D->mesh->getVertex(vs[vi], vtx);

         if (deltaNormSquared(vtxNow, vtx, 3) < 1e-10)
            break;
      }

      T bestDistance    = D::farAway;
      T bestNewDistance = D::farAway;
      T bestVtx[3];
      MeshIndex bestFace  = std::numeric_limits<MeshIndex>::max();
      MeshIndex bestFace2 = std::numeric_limits<MeshIndex>::max();

      T vtxs[3][3];
      T ds[2];

      typedef std::pair<MeshIndex,MeshIndex> Edge;
      typedef std::pair<Edge,MeshIndex> EdgeCell;
      std::vector< EdgeCell > checkEdges;

      /*
      std::ofstream os("drawposs.m");
      os << "clear; close; clc;" << std::endl;
      os << "figure; hold on;" << std::endl;
      os << "scatter3x([" << vtxNow[0] << " " << vtxNow[1] << " " << vtxNow[2] << "]', 'ko');" << std::endl;
      */

      if (vi != 3)
      {
         //BLANKLINE;
         //DEBUGMACRO("On Vertex");
         //BLANKLINE;
         // We are located at vertex vs[vi]
         // Find the edges between the one-ring vertices

         Container::Iterator<MeshIndex>::P it = m_D->conn->getFacesUsingVertex(vs[vi]);
         while (it->hasNext())
         {
            const MeshIndex f = it->next();

            {
               T vtxs[3][3];
               m_D->mesh->getFace(f, vtxs[0], vtxs[1], vtxs[2]);

               /*
               os << "ps = [" << std::endl;
               for (MeshIndex i = 0; i < 3; ++i)
                  os << "  " << vtxs[i][0] << " " << vtxs[i][1] << " " << vtxs[i][2] << std::endl;
               os << "]';" << std::endl;
               os << "plot3x(ps(:,[1:end 1]), 'g*-');" << std::endl;
               */
            }

            //if (f != cell)
            {
               // This face hasn't been visited before, so it's OK to consider
               
               // Get the indices of the vertices
               MeshIndex vvs[3];
               m_D->mesh->getFaceIndices(f, vvs);

               // Which is the non-shared vertex?
               MeshIndex p;
               for (p = 0; p < 3; ++p)
                  if (vvs[p] == vs[vi])
                     break;
               
               checkEdges.push_back(EdgeCell(Edge(vvs[(p+1)%3], vvs[(p+2)%3]), f));
            }
         }
      }
      else
      {
         /*
         BLANKLINE;
         DEBUGMACRO("On Face");
         BLANKLINE;
         */
         // We are on an edge or the face
         for (MeshIndex p = 0; p < 3; ++p)
            checkEdges.push_back(EdgeCell(Edge(vs[p], vs[(p+1)%3]), cell));

      }

      for (std::vector<EdgeCell>::iterator it = checkEdges.begin(), en = checkEdges.end(); it != en; ++it)
      {
         T vtxA[3], vtxB[3];
         m_D->mesh->getVertex(it->first.first, vtxA);
         m_D->mesh->getVertex(it->first.second, vtxB);

         /*
         os << "ps = [" << std::endl;
         os << "  " << vtxA[0] << " " << vtxA[1] << " " << vtxA[2] << std::endl;
         os << "  " << vtxB[0] << " " << vtxB[1] << " " << vtxB[2] << std::endl;
         os << "]';" << std::endl;
         os << "plot3x(ps, 'r');" << std::endl;

         os << "% vertex " << it->first.first << std::endl;
         os << "text(" << vtxA[0] << ", " << vtxA[1] << ", " << vtxA[2] << ", '" << (m_D->getVertexDistance(it->first.first )) << "');" << std::endl;
         os << "% vertex " << it->first.second << std::endl;
         os << "text(" << vtxB[0] << ", " << vtxB[1] << ", " << vtxB[2] << ", '" << (m_D->getVertexDistance(it->first.second)) << "');" << std::endl;
         */
      }
      
      //os << std::endl;

      // The faces that we've got so far are just the adjacent faces
      // what is more important is that the adjacent edge hasn't been visited
      
      for (std::vector<EdgeCell>::iterator it = checkEdges.begin(), en = checkEdges.end(); it != en; ++it)
      {
         Edge&     edge = it->first;
         MeshIndex face = it->second;

         MeshIndex fs[10];
         const MeshIndex nf = m_D->conn->getFacesUsingEdge(edge.first, edge.second, fs);

         MeshIndex i;
         for (i = 0; i < nf; ++i)
         {
            if (fs[i] != face && visited.find(fs[i]) == visited.end())
            {
               break;
            }
         }

         if (i != nf)
         {
            const MeshIndex adjFace = fs[i];

            // Only check this distance if the adjacent face is unvisited
            m_D->mesh->getVertex(edge.first, vtxs[0]);
            ds[0] = m_D->getVertexDistance(edge.first);

            m_D->mesh->getVertex(edge.second, vtxs[1]);
            ds[1] = m_D->getVertexDistance(edge.second);

            for (MeshIndex ax = 0; ax < 3; ++ax)
               vtxs[2][ax] = vtxNow[ax];

            T sourceToCrossing, crossingToTarget, alpha;
            const T dist = m_D->calculateSegmentCrossing(vtxs, ds, alpha, sourceToCrossing, crossingToTarget);

            /*
            {
               T thisVtx[3];
               m_D->mesh->getVertex(edge.first, vtxs[0]);
                  m_D->mesh->getVertex(edge.second, vtxs[1]);
               for (MeshIndex ax = 0; ax < 3; ++ax)
                  thisVtx[ax] = (1-alpha)*vtxs[0][ax] + alpha*vtxs[1][ax];
               os << "% best on face " << adjFace << std::endl;
               os << "% ds = " << ds[0] << ", " << ds[1] << std::endl;
               if (alpha == 0) os << "% dist = " << dist << ", ds[0] = " << ds[0] << ", s2c = " << sourceToCrossing << std::endl;
               if (alpha == 1) os << "% dist = " << dist << ", ds[1] = " << ds[1] << ", s2c = " << sourceToCrossing << std::endl;

               os << "scatter3(" << thisVtx[0] << ", " << thisVtx[1] << ", " << thisVtx[2] << ", 'b+');" << std::endl;
               os << "text("     << thisVtx[0] << ", " << thisVtx[1] << ", " << thisVtx[2] << ", '" << sourceToCrossing << "');" << std::endl;
            }
            */

            // Don't allow to stay at the same place
            if (dist < bestDistance && crossingToTarget > 1e-8)
            {
               // For now, assume we will move to this cell
               bestFace  = face;
               bestFace2 = adjFace;

               // Record the best distance
               bestDistance = dist;
               bestNewDistance = sourceToCrossing;

               // ...and where we will move to if we go here
               m_D->mesh->getVertex(edge.first, vtxs[0]);
               m_D->mesh->getVertex(edge.second, vtxs[1]);

               for (MeshIndex ax = 0; ax < 3; ++ax)
                  bestVtx[ax] = (1-alpha)*vtxs[0][ax] + alpha*vtxs[1][ax];
            }
         }
      }

      PRINTVBL(bestNewDistance);

      if (bestDistance == D::farAway)
         AWTEXCEPTIONTHROW("Could not find next move!");

      /*
      os << "quiver3(" << vtxNow[0] << ", " << vtxNow[1] << ", " << vtxNow[2] << ", " << (bestVtx[0]-vtxNow[0]) << ", " << (bestVtx[1]-vtxNow[1]) << ", " << (bestVtx[2]-vtxNow[2]) << ", 0, 'k:');" << std::endl;

      os << "axis equal" << std::endl;
      os.close();
      PAUSE;
      */

      for (MeshIndex ax = 0; ax < 3; ++ax)
         vtxNow[ax] = bestVtx[ax];

      cell = bestFace2;
   }

   //m_D->changeCellIfOnVertex(cell, vtxNow, thresh, visited);

   //while (deltaNormSquared(vtxNow, m_D->sourcePoint, 3) > 1e-6 && cell != m_D->sourceCell)
   //{
   //   bool dopause = debug;

   //   if (debug)
   //   {
   //      trackCells[ tracker++%3 ] = cell;

   //      for (MeshIndex i = 1; i < 3 && dopause; ++i)
   //         dopause &= trackCells[i] == trackCells[0];

   //      if (dopause)
   //         PRINTVEC(trackCells, 3);
   //   }

   //   if (debug)
   //   {
   //      PRINTVBL(cell);
   //   }

   //   if (false && debug) //xx
   //   {
   //      BLANKLINE;
   //      PRINTVEC(vtxNow, 3);
   //      PRINTVBL(cell);
   //      {
   //         m_D->mesh->getFaceIndices(cell, vs);
   //         for (int i = 0; i < 3; ++i)
   //         {
   //            T vtx[3];
   //            m_D->mesh->getVertex(vs[i], vtx);
   //            std::cerr << "\t" << vs[i] << "\t"; PRINTVEC(vtx, 3);
   //            std::cerr << "\t"; PRINTVBL(m_D->getVertexDistance(vs[i]));
   //         }
   //      }
   //   }

   //   visited.insert(cell);

   //   // Get the corner indices of this triangle
   //   m_D->mesh->getFaceIndices(cell, vs);
   //   //PRINTVEC(vs, 3);

   //   if (false && debug) //xx
   //   {
   //      DEBUGMACRO("Vertex distances:");
   //      for (MeshIndex ax = 0; ax < 3; ++ax)
   //         DEBUGMACRO(vs[ax] << "\t" << m_D->getVertexDistance(vs[ax]));
   //      BLANKLINE;

   //      DEBUGMACRO("Segment crossings:");
   //   }

   //   // Find the minimum distance on this cell
   //   T bestDist = D::farAway;
   //   T bestAlpha = 0.5;
   //   T bestMovedBy = 0;
   //   MeshIndex bestP = 3;

   //   T vtxs[3][3];
   //   T ds[2];      
   //   MeshIndex onEdge;

   //   {
   //      T shortest = D::farAway;

   //      for (MeshIndex p = 0; p < 3; ++p)
   //      {
   //         T delta[3];

   //         // Get the vertex locations
   //         m_D->mesh->getVertex(vs[(p+1)%3], vtxs[0]);
   //         m_D->mesh->getVertex(vs[(p+2)%3], vtxs[1]);

   //         for (MeshIndex ax = 0; ax < 3; ++ax)
   //         {
   //            vtxs[2][ax] = vtxs[1][ax] - vtxs[0][ax];
   //            delta[ax]   = vtxNow[ax]  - vtxs[0][ax];
   //         }

   //         normalize(vtxs[2], 3);
   //         cross(vtxs[2], delta, vtxs[0]);

   //         const T thisLength = normalize(vtxs[0], 3);

   //         if (thisLength < shortest)
   //         {
   //            onEdge = p;
   //            shortest = thisLength;
   //         }
   //      }
   //   }

   //   for (MeshIndex p = 0; p < 3; ++p)
   //   {
   //      if (p == onEdge)
   //         continue;

   //      // Don't consider faces which have been visited before
   //      {
   //         MeshIndex fs[2];
   //         const MeshIndex nf = m_D->conn->getFacesUsingEdge(vs[(p+1)%3], vs[(p+2)%3], fs);

   //         MeshIndex f;
   //         for (f = 0; f < nf; ++f)
   //         {
   //            if (fs[f] != cell && visited.find(fs[f]) == visited.end())
   //               break;
   //         }

   //         if (f == nf)
   //            continue;
   //      }

   //      // Get the vertex distances
   //      ds[0] = m_D->getVertexDistance(vs[(p+1)%3]);
   //      ds[1] = m_D->getVertexDistance(vs[(p+2)%3]);

   //      // Get the vertex locations
   //      m_D->mesh->getVertex(vs[(p+1)%3], vtxs[0]);
   //      m_D->mesh->getVertex(vs[(p+2)%3], vtxs[1]);

   //      // Get the current position
   //      for (MeshIndex ax = 0; ax < 3; ++ax)
   //         vtxs[2][ax] = vtxNow[ax];

   //      //D::ChildAndParents cp(vs[p], D::Parents(vs[(p+1)%3], vs[(p+2)%3]));

   //      T alpha, sourceToCrossing, crossingToTarget;
   //      const T dist = m_D->calculateSegmentCrossing(vtxs, ds, alpha, sourceToCrossing, crossingToTarget);

   //      if (false && debug) //xx
   //      {
   //         BLANKLINE;
   //         DEBUGMACRO("For edge: " << vs[(p+1)%3] << "," << vs[(p+2)%3]);
   //         DEBUGMACRO("\t" << "s2c = " << sourceToCrossing << ", c2t = " << crossingToTarget << ", dist = " << dist << ", alpha = " << alpha);
   //      }

   //      if (dist-bestDist < -thresh || (abs(dist-bestDist)<thresh && crossingToTarget > bestMovedBy))
   //      {
   //         if (false && debug) //xx
   //         {
   //            PRINTBOOL(dist < bestDist);
   //            PRINTBOOL(abs(dist-bestDist)<thresh && crossingToTarget > bestMovedBy);
   //         }

   //         bestDist = dist;
   //         bestP = p;
   //         bestAlpha = alpha;
   //         bestMovedBy = crossingToTarget;
   //      }
   //   }
   //   
   //   // Get the new location by interpolating between the best parents
   //   m_D->interpolate(vs[(bestP+1)%3], vs[(bestP+2)%3],bestAlpha, vtxNew);

   //   // And shuffle one point along the path
   //   m_D->updateTrackpoints(vtxPrev, vtxNow, vtxNew);

   //   // Now we know which the best segment is, but what face does that point to?

   //   if (bestAlpha != 0 && bestAlpha != 1)
   //   {
   //      // We're not on a vertex, so can just look for the face on the other side
   //      MeshIndex fs[10];
   //      const MeshIndex nf = m_D->conn->getFacesUsingEdge(vs[(bestP+1)%3], vs[(bestP+2)%3], fs);

   //      for (MeshIndex f = 0; f < nf; ++f)
   //      {
   //         if (fs[f] != cell && visited.find(fs[f]) == visited.end())
   //         {
   //            // Move to the other triangle
   //            cell = fs[f];
   //            //DEBUGMACRO("Changing cell to " << cell);
   //            break;
   //         }
   //      }
   //   }
   //   else
   //   {
   //      // Check where we are now: if it is on a vertex, we need to pick the best triangle from this vertex
   //      m_D->changeCellIfOnVertex(cell, vtxNow, thresh, visited);
   //   }

   //   if (debug && dopause)
   //   {
   //      PAUSE;
   //   }
   //}

   if (cell == m_D->sourceCell)
   {
      for (MeshIndex ax = 0; ax < 3; ++ax)
         dir[ax] = vtxNow[ax] - m_D->sourcePoint[ax];
   }
   else
   {
      for (MeshIndex ax = 0; ax < 3; ++ax)
         dir[ax] = vtxPrev[ax] - m_D->sourcePoint[ax];
   }

   if (dot<T>(dir, dir, 3) != 0)
      normalize(dir, 3);
}

#endif

template class AWT::MeshApproxGeodesic<double>;