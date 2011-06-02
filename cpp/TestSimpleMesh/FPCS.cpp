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
#if 0
#include "SimpleMesh/Connectivity.h"
#include "SimpleMesh/FacesInRadius.h"
#include "SimpleMesh/FacesIntersectPlane.h"
#include "SimpleMesh/Mesh.h"
#include "SimpleMesh/MeshFunctions.h"
#include "SimpleMesh/MeshIO.h"
#include "SimpleMesh/Search.h"
#include "SimpleMesh/UniformSampler.h"
#include "SimpleMesh/VerticesBetweenPlanes.h"
#include "SimpleMesh/VerticesInRadius.h"

#include "Useful/ArrayVectorFunctions.h"
#include "Useful/Noise.h"

#include <set>
#include <fstream>
#include <algorithm>

#include "vnl/vnl_matrix.h"
#include "vnl/algo/vnl_svd.h"
#include "vnl/vnl_matlab_filewrite.h"

typedef AWT::Noise<AWT::SimpleMesh::Coordinate> random;

using namespace AWT::SimpleMesh;

typedef std::vector<IndexLine> IndexLines;

struct IntersectionPoint
{
   Point p;
   Index vs[2];
};
typedef std::vector<IntersectionPoint> IntersectionPoints;

struct FPCSTarget
{
   Coordinate length;
   Coordinate ratio;
   Coordinate dihedral;
};

struct FPCSTargets
{
   FPCSTarget targets[2];
   Coordinate sinAngle;
};

const double PI = atan(1.0) * 4;
const double DEG_TO_RAD = PI / 180.0;

double calculateSinAngle(const Point line1[2], const Point line2[2])
{
   const Point delta[] = { (line1[1] - line1[0]).normalize(), (line2[1] - line2[0]).normalize() };

   Point crossVec;
   AWT::cross<Coordinate>(delta[0].data_block(), delta[1].data_block(), crossVec.data_block());
   crossVec[3] = 0;

   return crossVec.magnitude();
}

void findExtrema(Points& points, const Point& axis, Point extrema[2])
{
   Coordinate extremes[] = { FAR_FAR_AWAY, -FAR_FAR_AWAY };
   Point extremePnt[2];

   for (Index col = 0; col < points.cols(); ++col)
   {
      Point mu = points.get_column(col);
      Coordinate c = dot(axis, mu, 4);

      if (c < extremes[0])
      {
         extremes[0] = c;
         extrema[0] = mu;
      }

      if (c > extremes[1])
      {
         extremes[1] = c;
         extrema[1] = mu;
      }
   }
}

void calculateFPCSTargets(Points& pts, Points& nmls, FPCSTargets& tgts)
{
   Point delta[] = { pts.get_column(1)-pts.get_column(0), pts.get_column(3)-pts.get_column(2) };
   tgts.targets[0].length = delta[0].magnitude();
   tgts.targets[1].length = delta[1].magnitude();

   // Calculate where these lines cross
   // Assume that they are truly planar
   Point a = pts.get_column(0);
   Point b = pts.get_column(1) - a;
   Point c = pts.get_column(2) - a;
   Point d = pts.get_column(3) - a;

   Coordinate lambda = (dot(c,b,3) + dot(d,b,3)) / (2 * dot(b,b,3));
   Point x = a + lambda * b;

   tgts.targets[0].ratio = (x - pts.get_column(0)).magnitude() / (pts.get_column(1) - pts.get_column(0)).magnitude();
   tgts.targets[1].ratio = (x - pts.get_column(2)).magnitude() / (pts.get_column(3) - pts.get_column(2)).magnitude();

   // Store the cos of the dihedral angle
   tgts.targets[0].dihedral = inner_product(nmls.get_column(1), nmls.get_column(0));
   tgts.targets[1].dihedral = inner_product(nmls.get_column(3), nmls.get_column(2));

   // Also store the angle between the lines as a discriminator
   delta[0].normalize();
   delta[1].normalize();

   Point crossVec;
   AWT::cross<Coordinate>(delta[0].data_block(), delta[1].data_block(), crossVec.data_block());
   crossVec[3] = 0;

   tgts.sinAngle = crossVec.magnitude();

}

const double radiusForAngle(const double theta)
{
   return sqrt(2 - 2*cos(theta));
}

void constructSearchSet(Search::P normalSearcher, Mesh::P mesh, const FPCSTarget& tgt, const double delta, std::vector<IntersectionPoint>& res)
{
   Mesh::P normalMesh = normalSearcher->getMesh();
   const Index N = normalMesh->nv;

   VerticesBetweenPlanes::P vbp = VerticesBetweenPlanes::getInstance(2);

   const double theta = acos(tgt.dihedral);
   const double dtheta = 10.0*DEG_TO_RAD;

   const double upperD = cos(std::max(0.0, theta-dtheta/2));
   const double lowerD = cos(std::min(theta+dtheta/2,PI));

   typedef std::pair<Index,Index> IndexPair;
   std::set<IndexPair> checkSet;

   for (Index i = 0; i < N; ++i)
   {
      vbp->reset();

      Point q = normalMesh->getVertex(i);

      Point p0 = -q; p0(3) = upperD;
      Point p1 = q;  p1(3) = -lowerD;

      vbp->setTestPlane(0, p0);
      vbp->setTestPlane(1, p1);

      /*
      PRINTVBL(theta/DEG_TO_RAD);
      PRINTVBLMATLAB(q);
      PRINTVBLMATLAB(p0);
      PRINTVBLMATLAB(p1);
      */
      
      normalSearcher->search(vbp);

      std::vector<Index>& found = vbp->getVertices();

      std::vector<Index>::iterator it = found.begin();
      std::vector<Index>::iterator en = found.end();
      
      if (!found.empty())
      {
         for (; it != en; ++it)
         {
            // Push both ways around into the potential set
            Index vs[] = { i, *it };

            Point p0 = mesh->getVertex(vs[0]);
            Point p1 = mesh->getVertex(vs[1]);

            if (abs((p0-p1).squared_magnitude() - tgt.length) < delta)
            {
               for (Index swap = 0; swap < 2; ++swap)
                  checkSet.insert(IndexPair(vs[swap],vs[1-swap]));
       
               PRINTVBL(i);
               PRINTVBL(found.size());
               PRINTVBL(checkSet.size());
            }
         }
      }

      if (i % 500 == 499)
         DEBUGMACRO(i << "/" << N);
   }

   PRINTVBL(checkSet.size());
   PAUSE;

   {
      // Push the found pairs into the result set
      std::set<IndexPair>::iterator it = checkSet.begin();
      std::set<IndexPair>::iterator en = checkSet.end();

      if (it != en)
      {
         do
         {
            IntersectionPoint isp = { (1-tgt.ratio)*mesh->getVertex(it->first) + tgt.ratio*mesh->getVertex(it->second), it->first, it->second };
            res.push_back(isp);
         }
         while (++it != en);
      }
   }
}

void constructSearchSet(Search::P searcher, Points& normals, const FPCSTarget& tgt, const double delta, std::vector<IntersectionPoint>& res)
{
   VerticesInRadius::P vir = VerticesInRadius::getInstance();

   vir->setInnerRadius(tgt.length - delta);
   vir->setOuterRadius(tgt.length + delta);

   Mesh::P mesh = searcher->getMesh();
   Points& vs = mesh->getVertices();
   const Index N = vs.cols();

   typedef std::pair<Index,Index> IndexPair;
   std::set<IndexPair> checkSet;

   const double theta = acos(tgt.dihedral);
   const double dtheta = 1*DEG_TO_RAD;
   
   const double upperD = cos(std::max(0.0, theta-dtheta/2));
   const double lowerD = cos(std::min(theta+dtheta/2,PI));

   for (Index i = 0; i < N; ++i)
   {
      if (i % 500 == 499)
         PRINTVBL(i);

      const Index checkSetSize = checkSet.size();

      vir->reset();
      vir->setTestPoint(mesh->getVertex(i));

      searcher->search(vir);

      std::vector<Index>& found = vir->getVertices();

      std::vector<Index>::iterator it = found.begin();
      std::vector<Index>::iterator en = found.end();

      for (; it != en; ++it)
      {
         // Push both ways around into the potential set
         Index vs[] = { i, *it };

         // Check to see if the normals of these points are compatible
         const double dihedral = inner_product(normals.get_column(vs[0]), normals.get_column(vs[1]));
         if (dihedral <= upperD && dihedral >= lowerD)
         {
            for (Index swap = 0; swap < 2; ++swap)
               checkSet.insert(IndexPair(vs[swap],vs[1-swap]));
         }
      }

      if (checkSet.size() != checkSetSize)
         DEBUGMACRO(i << "\t" << N << "\t" << checkSet.size() << "\t" << ((0.0+checkSet.size())/i));
   }

   {
      // Push the found pairs into the result set

      std::set<IndexPair>::iterator it = checkSet.begin();
      std::set<IndexPair>::iterator en = checkSet.end();

      if (it != en)
      {
         do
         {
            IntersectionPoint isp = { (1-tgt.ratio)*mesh->getVertex(it->first) + tgt.ratio*mesh->getVertex(it->second), it->first, it->second };
            res.push_back(isp);
         }
         while (++it != en);
      }
   }
}

Mesh::P sampleMesh(Mesh::P model, const Index nsamples, Points& normals)
{
   Mesh::P ret = Mesh::getInstance(nsamples,0);
   UniformSampler::P sampler = UniformSampler::getInstance(model);

   normals.set_size(4,nsamples);

   Matrix& vs = ret->getVertices();
   for (Index i = 0; i < nsamples; ++i)
   {
      PointIndexWeights piw = sampler->sample();
      vs.set_column(i,piw.p);

      normals.set_column(i, calculateFaceNormal(model, piw.i, true));
   }

   return ret;
}

Search::P constructNormalSearcher(Mesh::P mesh)
{
   Mesh::P normalMesh = Mesh::getInstance(mesh->nv, 0);
   calculateVertexNormals(mesh, normalMesh->getVertices(), true);

   // Make these into points
   normalMesh->getVertices().set_row(3,1);

   return Search::getInstance(normalMesh, false);
}

void pickCoplanarPoints(Search::P searcher, Points& pts, Points& nmls)
{
   Mesh::P mesh = searcher->getMesh();

   // Calculate the mesh's centroid
   Point centroid = calculateCentroid(mesh);

   // Pick a random normal
   Point plane;
   for (Index ax = 0; ax < 3; ++ax)
      plane(ax) = random::randu(-1,1);
   
   plane(3) = -dot(plane, centroid, 3);

   FacesIntersectPlane::P fip = FacesIntersectPlane::getInstance();
   fip->setTestPlane(plane);

   searcher->search(fip);

   typedef std::vector<IndexLine> IndexLines;
   IndexLines& faces = fip->getFaces();

   Index col;
   Matrix p[2] = { Matrix(4, faces.size()), Matrix(4, faces.size()) };

   Matrix covar(4,4);
   Point sumx;
   Coordinate sumw;

   Points mus(4, faces.size());

   covar.fill(0);
   sumx.fill(0);
   sumw = 0;

   col = 0;
   for (IndexLines::iterator it = faces.begin(), en = faces.end(); it != en; ++it, ++col)
   {
      Point mu = (it->p[0]+it->p[1]) * 0.5;
      mus.set_column(col, mu);

      mu(3) = 0;

      Coordinate len = (it->p[0]-it->p[1]).magnitude();

      covar += outer_product(mu, mu) * len;
      sumx  += mu * len;
      sumw  += len;
   }

   // Correct for zero mean shift
   covar = covar - outer_product(sumx, sumx) / sumw;

   Transformation axes = vnl_svd<Coordinate>(covar).U();

   // Pick the extremal points in the principal direction
   Point extremePnt[2][2];
   findExtrema(mus, axes.get_column(0), extremePnt[0]);
   findExtrema(mus, axes.get_column(1), extremePnt[1]);

   pts.set_size(4,4);
   pts.set_column(0,extremePnt[0][0]);
   pts.set_column(1,extremePnt[0][1]);
   pts.set_column(2,extremePnt[1][0]);
   pts.set_column(3,extremePnt[1][1]);

   // Go back and recover the face normals
   // Average over all faces intersecting sphere
   FacesInRadius::P fir = FacesInRadius::getInstance();
   fir->setTestRadius(5);
   nmls.set_size(4,4);
   for (Index a = 0; a < 2; ++a)
   {
      for (Index b = 0; b < 2; ++b)
      {
         fir->reset();
         fir->setTestPoint(extremePnt[a][b]);

         searcher->search(fir);

         std::vector<PointIndexWeights>& found = fir->getFaces();
         std::vector<PointIndexWeights>::iterator it = found.begin();
         std::vector<PointIndexWeights>::iterator en = found.end();

         Point nml; nml.fill(0);
         do
         {
            nml += calculateFaceNormal(mesh, it->i);
         }
         while (++it != en);

         PRINTVBL(nml.normalize());
         nmls.set_column(2*a + b, nml.normalize());
      }
   }
}

Transformation absoluteOrientation(const Points& ps, const Points& qs)
{
   Matrix covar(4,4, 0);
   Point  sum_p; sum_p.fill(0);
   Point  sum_q; sum_q.fill(0);
   Coordinate var_p(0);
   Coordinate var_q(0);

   const Index N = ps.cols();
   const Coordinate NN = static_cast<Coordinate>(N);

   for (Index i = 0; i < N; ++i)
   {
      Point pc = ps.get_column(i);
      Point qc = qs.get_column(i);

      pc(3) = qc(3) = 0;

      covar += outer_product<Coordinate>(pc, qc);

      sum_p += pc;
      sum_q += qc;

      var_p += inner_product<Coordinate>(pc, pc);
      var_q += inner_product<Coordinate>(qc, qc);
   }

   // Correct these quantities
   covar -= outer_product<Coordinate>(sum_p, sum_q) / static_cast<Coordinate>(N);
   var_p -= inner_product<Coordinate>(sum_p, sum_p) / static_cast<Coordinate>(N);
   var_q -= inner_product<Coordinate>(sum_q, sum_q) / static_cast<Coordinate>(N);

   // Calculate the scale
   const Coordinate sc = sqrt(var_p / var_q);

   // Calculate the (scaled) rotation
   vnl_svd<Coordinate> svd(covar);
   Transformation R = sc * svd.V() * svd.U().transpose(); R(3,3) = 1;

   // Calculate the translation
   Point t = -(R*sum_p - sum_q) / static_cast<Coordinate>(N);
   t(3) = 1;
   R.set_column(3,t);

   return R;
}

int main(int argc, char** argv)
{
   const Index nmodelSamples = 1000;
   const Index nmeshSamples = 50;
   const double delta = 3;

   random::timeSeed();

   DEBUGMACRO("Loading mesh from " << argv[1]);
   Mesh::P mesh = MeshIO::loadMesh(std::ifstream(argv[1]));

   DEBUGMACRO("Loading model from " << argv[2]);
   ShapeModel::P shapeModel = MeshIO::loadModel(std::ifstream(argv[2]));

   DEBUGMACRO("Constructing searchers...");
   Search::P faceSearcher = Search::getInstance(mesh, true);

   Points quad, nmls;
   FPCSTargets targets;

   do
   {
      DEBUGMACRO("Picking coplanar points...");
      pickCoplanarPoints(faceSearcher, quad, nmls);
      calculateFPCSTargets(quad, nmls, targets);
   }
   while (targets.sinAngle < 0.707);

   // Draw samples from the mesh; these are used to establish the support
   Points msn;
   Mesh::P meshSampleMesh = sampleMesh(mesh, nmeshSamples, msn);

   // Create an instance of the shape model
   Vector ws(shapeModel->getNumberOfModes());
   ws.fill(0);
   ws[0] = 1;
   Mesh::P shapeModelInst = shapeModel->getMesh(ws);
   // This is used to check the support
   Search::P modelFaceSearcher = Search::getInstance(shapeModelInst, true);
   FacesInRadius::P fir = FacesInRadius::getInstance();

   // Now draw samples from it and prepare to search it
   Points normals;
   Mesh::P   modelSampleMesh     = sampleMesh(shapeModelInst, nmodelSamples, normals);
   Search::P modelSampleSearcher = Search::getInstance(modelSampleMesh, false);

   IntersectionPoints isect[2];

   /*
   constructSearchSet(modelSampleSearcher, normals, targets.targets[0], delta, isect[0]);
   constructSearchSet(modelSampleSearcher, normals, targets.targets[1], delta, isect[1]);
   */

   {
      Search::P modelVertexSearcher = Search::getInstance(shapeModelInst, false);
      Points normals;
      calculateVertexNormals(shapeModelInst, normals, true);

      DEBUGLINE;
      constructSearchSet(modelVertexSearcher, normals, targets.targets[0], delta, isect[0]);
      DEBUGLINE;
      constructSearchSet(modelVertexSearcher, normals, targets.targets[1], delta, isect[1]);
      DEBUGLINE;
   }

   /*
   DEBUGLINE;
   Search::P normalSearcher = constructNormalSearcher(shapeModelInst);
   DEBUGLINE;
   constructSearchSet(normalSearcher, shapeModelInst, targets.targets[0], delta, isect[0]);
   DEBUGLINE;
   constructSearchSet(normalSearcher, shapeModelInst, targets.targets[1], delta, isect[1]);
   */

   PRINTVBL(isect[0].size());
   PRINTVBL(isect[1].size());
   PAUSE;

   // Construct a mesh from the second set
   Mesh::P intersectionSearchMesh = Mesh::getInstance(isect[1].size(), 0);
   {
      Points& vs = intersectionSearchMesh->getVertices();

      IntersectionPoints::iterator it = isect[1].begin();
      IntersectionPoints::iterator en = isect[1].end();
      Index i = 0;
      do
      {
         vs.set_column(i++, it->p);
      }
      while (++it != en);
   }
   Search::P intersectionSearch = Search::getInstance(intersectionSearchMesh, false);

   // Now, for each point in the first set, search for nearby points in the second
   Transformation bestTrans;
   {
      IntersectionPoints::iterator it = isect[0].begin();
      IntersectionPoints::iterator en = isect[0].end();

      Points& vs = intersectionSearchMesh->getVertices();

      VerticesInRadius::P agent = VerticesInRadius::getInstance();
      agent->setInnerRadius(0);
      agent->setOuterRadius(delta);

      const double deltaTheta = 5 * DEG_TO_RAD;

      const double maxSinTheta = sin(std::min<double>(PI/2, asin(targets.sinAngle) + deltaTheta));
      const double minSinTheta = sin(std::max<double>(   0, asin(targets.sinAngle) - deltaTheta));

      Points qs(4,4);

      Index foundCount = 0;

      // Set up the mesh searcher
      fir->setTestRadius(delta);
      fir->setStopAfterFirst(true);

      // Look for the transformation which gives the best support
      Index bestSupport = 0;
      double bestFro2 = std::numeric_limits<double>::max();

      do
      {
         agent->reset();
         agent->setTestPoint(it->p);

         intersectionSearch->search(agent);

         Point ends1[] = { modelSampleMesh->getVertex(it->vs[0]), modelSampleMesh->getVertex(it->vs[1]) };

         qs.set_column(0, modelSampleMesh->getVertex(it->vs[0]));
         qs.set_column(1, modelSampleMesh->getVertex(it->vs[1]));

         if (!agent->getVertices().empty())
         {
            typedef std::vector<Index> Indexes;
            Indexes::iterator iit = agent->getVertices().begin();
            Indexes::iterator ien = agent->getVertices().end();
            do
            {
               // Work out what these two intersections relate to
               Point ends2[] = { modelSampleMesh->getVertex(isect[1][*iit].vs[0]), modelSampleMesh->getVertex(isect[1][*iit].vs[1]) };

               qs.set_column(2, modelSampleMesh->getVertex(isect[1][*iit].vs[0]));
               qs.set_column(3, modelSampleMesh->getVertex(isect[1][*iit].vs[1]));

               const double sinTheta = calculateSinAngle(ends1, ends2);
               if (sinTheta >= minSinTheta && sinTheta <= maxSinTheta)
               {
                  Transformation trans = absoluteOrientation(quad, qs);

                  Index support = 0;
                  double fro2 = 0;

                  // Transform the mesh samples one-by-one and search the model mesh for support
                  for (Index s = 0; s < nmeshSamples; ++s)
                  {
                     // If there aren't enough points left to beat the
                     // current support, stop!
                     const Index remaining = nmeshSamples - s;
                     if (support + remaining < bestSupport)
                        break;

                     // Transform the point
                     Point p = trans * meshSampleMesh->getVertex(s);

                     fir->reset();
                     fir->setTestPoint(p);
                     modelFaceSearcher->search(fir);

                     if (!fir->getFaces().empty())
                     {
                        ++support;
                        fro2 += (fir->getFaces()[0].p - p).squared_magnitude();
                     }
                  }

                  if (support > bestSupport || (support == bestSupport && fro2 < bestFro2))
                  {
                     bestSupport = support;
                     bestFro2 = fro2;
                     bestTrans = trans;

                     DEBUGMACRO(bestSupport << "\t" << (bestFro2/bestSupport));
                  }
               }
            }
            while (++iit != ien);
         }

      }
      while (++it != en);

      PRINTVBLNL(bestTrans);
      PRINTVBL(foundCount);
   }

   DEBUGMACRO("Done.");
   PRINTVBL(quad);

   {
      vnl_matlab_filewrite fw("fpcs.mat");
      fw.write(quad, "exes");
      fw.write(bestTrans, "R");
      fw.write(meshSampleMesh->getVertices(), "meshSamples");
   }
}
#endif