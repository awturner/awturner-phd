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
#include "SampledTorusMesh.h"

#include "vnl/vnl_matlab_filewrite.h"
#include "Maths/BSpline.h"
#include "Mesh/MeshFunctions.h"
#include "Mesh/MeshImpl.h"
#include "Mesh/TriangleProjection.h"
#include "Mesh/TuplesFunctions.h"
#include "MeshSearching/FacesNearestPointSearch.h"
#include "MeshSearching/FacesRayIntersectionSearch.h"
#include "Useful/ProfTimer.h"
#include "Useful/WalkInASpiral.h"
#include "Useful/ColouredConsole.h"
#include "vnl/vnl_sparse_matrix.h"
#include "vnl/algo/vnl_svd.h"

#include <vector>
#include <algorithm>
#include <fstream>

#include "vnl/vnl_sparse_matrix_linear_system.h"
#include "vnl/algo/vnl_lsqr.h"
#include "vnl/vnl_matlab_write.h"

using namespace AWT;
using namespace AWT::AlignParametric;

struct AWT::AlignParametric::SampledTorusMesh::D
{
   MeshType::P          mesh;
   MeshType::P          textureMesh;

   MatrixType           flatSamplesOrig;
   MatrixType           flatSamples;
   MatrixType           meshSamples;

   vnl_sparse_matrix<T> weights;
   MatrixType           controls;

   Idx nk[2];

   T faceSearchOffset[2];

   std::vector<MeshIndex> sampleFaces;

   T maxMove;

   typedef vnl_matrix_fixed<T,4,2>        FlatToMeshJacobian;
   std::map<MeshIndex,FlatToMeshJacobian> faceJacobiansCache;
   ModifiedTime                           lastFaceJacobianCalculated;

   typedef std::pair<int,int> Bin;
   typedef std::map<Bin,std::vector<MeshIndex> > FaceBins;
   FaceBins facesInBin;
   int binBounds[4];
   static const int nbinsPerSquare = 256;

   //! Jacobian of the mapping between the uv plane and the original xyz space
   /*!
   \param f the face index
   \return 4x2 Jacobian matrix
   */
   FlatToMeshJacobian calculateFaceJacobian(const MeshIndex f)
   {
      TuplesType::P texCoords = mesh->getTextureCoords();

      MeshIndex vs[3];
      T vtx[3][3];
      T tex[3][2];

      mesh->getFaceIndices(f, vs);

      // Get the vertex and texture coordinates
      for (MeshIndex i = 0; i < 3; ++i)
      {
         mesh->getVertex(vs[i], vtx[i]);
         texCoords->getPoint(vs[i], tex[i]);
      }

      // Translate to be relative to first point in each face
      // After all, we only want a differential change
      for (MeshIndex ax = 0; ax < 3; ++ax)
      {
         vtx[2][ax] -= vtx[0][ax];
         vtx[1][ax] -= vtx[0][ax];
         vtx[0][ax] -= vtx[0][ax];
      }

      for (MeshIndex ax = 0; ax < 2; ++ax)
      {
         tex[2][ax] -= tex[0][ax];
         tex[1][ax] -= tex[0][ax];
         tex[0][ax] -= tex[0][ax];
      }

      // Calculate the inverse matrix
      T proj[2][2];

      const T det = tex[1][0]*tex[2][1] - tex[2][0]*tex[1][1];

      proj[0][0] =  tex[2][1] / det;
      proj[0][1] = -tex[1][1] / det;
      proj[1][0] = -tex[2][0] / det;
      proj[1][1] =  tex[1][0] / det;

      FlatToMeshJacobian jacf;
      for (Idx r = 0; r < 3; ++r)
         for (Idx c = 0; c < 2; ++c)
            jacf(r, c) = proj[c][0] * vtx[1][r] + proj[c][1] * vtx[2][r];

      // Homogeneous jacobian
      jacf[3][0] = 0;
      jacf[3][1] = 0;

      return jacf;
   }

   // Calculate the jacobian dP/du for a face
   FlatToMeshJacobian getFaceJacobian(const MeshIndex f)
   {
      const ModifiedTime meshLastModified  = mesh->getModifiedTime();

      // If the vertices or tex coords have been modified, invalidate the cache
      if (meshLastModified > lastFaceJacobianCalculated)
         faceJacobiansCache.clear();

      // If this face jacobian hasn't been stored in the cache
      if (faceJacobiansCache.find(f) == faceJacobiansCache.end())
      {
         faceJacobiansCache[f] = calculateFaceJacobian(f);

         lastFaceJacobianCalculated = mesh->getModifiedTime();
      }

      return faceJacobiansCache[f];
   }

   FlatToMeshJacobian getSampleJacobian(const MeshIndex i)
   {
      return getFaceJacobian(sampleFaces[i]);
   }

   // Which face is the sample currently located over?
   Idx getWhichFace(const Idx sampleIdx)
   {  
      return sampleFaces[sampleIdx];
   }

   void setParameters(const MatrixType& controls)
   {
      DEBUGMACRO("Setting parameters");

      this->controls = controls;

      // When parameters are set, calculate the sample positions
      // Also work out which face the sample points are now over

      VectorType displacement;
      displacement.set_size(flatSamplesOrig.rows());

      for (Idx ax = 0; ax < 2; ++ax)
      {
         weights.mult(controls.get_column(ax), displacement);
         flatSamples.set_column(ax, displacement);
      }

      // So far we have the displacements, not the position.  Add on the
      // original position.
      flatSamples += flatSamplesOrig;

      //DEBUGMACRO("Sample positions calculated");

      // Now that the new flat sample positions are known, these can
      // be mapped back to the original to find out the positions in 3d

      meshSamples.set_size(4,flatSamples.rows());

      T flatCoords[2];
      T meshCoords[3];
      MeshIndex faceIndex;

      ProfTimer prof;
      double accum = 0;

      PRINTVBL(flatSamples.rows());

      /*
      FacesRayIntersectionSearch<T>::P nearestSearch = FacesRayIntersectionSearch<T>::getInstance();
      nearestSearch->setAllowedBackwards(true);
      */

      MeshIndex avChecks = 0;
      MeshIndex nchecks;

      /*
      T rmsError = 0;
      T maxError = 0;
      */

      bool debug;

      sampleFaces.clear();
      for (Idx i = 0; i < flatSamples.rows(); ++i)
      {
         prof.start();

         flatCoords[0] = flatSamples(i,0);
         flatCoords[1] = flatSamples(i,1);

         switch (i)
         {
         case 0:
         case 1:
         case 63:
            debug = false;
            break;
         default:
            debug = false;
         }

         if (debug)
         {
            PRINTVEC(flatCoords,2);
            PRINTVBL(flatSamples.get_row(i));
         }
         binFlattenedToMesh(flatCoords, meshCoords, faceIndex, nchecks, debug);
         avChecks += nchecks;

         if (debug)
         {
            {
               vnl_matlab_filewrite fw("flat.mat");

               T vtx[3];
               MatrixType vs(3, textureMesh->getNumberOfVertices());
               for (Idx v = 0; v < textureMesh->getNumberOfVertices(); ++v)
               {
                  textureMesh->getVertex(v,vtx);
                  vs.set_column(v,vtx);
               }
               fw.write(vs, "vs");

               MeshIndex vi[3];
               MatrixType fs(3, textureMesh->getNumberOfFaces());
               for (Idx v = 0; v < textureMesh->getNumberOfFaces(); ++v)
               {
                  textureMesh->getFaceIndices(v,vi);

                  for (Idx ax = 0; ax < 3; ++ax)
                     vtx[ax] = vi[ax] + 1;

                  fs.set_column(v,vtx);
               }
               fw.write(fs, "fs");

               MatrixType pnt(2,1);
               pnt.set_column(0,flatSamples.get_row(i));
               fw.write(pnt, "pt");

               fw.write(faceIndex+1, "fi");

               PRINTVBL(textureMesh->getNumberOfVertices());
            }
            PAUSE;
         }

         if (debug)
         {
            PRINTVBL(i);
            PRINTVBL(faceIndex);
            PRINTVEC(flatCoords,2);
            PRINTVEC(meshCoords,3);

            T vs[3][3];
            textureMesh->getFace(faceIndex, vs[0], vs[1], vs[2]);
            PRINTVEC(vs[0],3);
            PRINTVEC(vs[1],3);
            PRINTVEC(vs[2],3);

            mesh->getFace(faceIndex, vs[0], vs[1], vs[2]);
            PRINTVEC(vs[0],3);
            PRINTVEC(vs[1],3);
            PRINTVEC(vs[2],3);
         }

         /*
         {
            T treeCoords[3];
            MeshIndex tFaceIndex;

            mapFlattenedToMesh(nearestSearch, flatCoords, treeCoords, tFaceIndex);

            const T thisDns = deltaNormSquared<T>(treeCoords, meshCoords, 3);
            maxError = std::max(maxError, thisDns);
            rmsError += thisDns;
         }
         */
         
         // Record the position on the mesh
         for (Idx ax = 0; ax < 3; ++ax)
            meshSamples(ax,i) = meshCoords[ax];

         // Homogeneous position
         meshSamples(3,i) = 1;

         // ...and record the face it is over
         sampleFaces.push_back(faceIndex);

         prof.stop();
         accum += prof.getDurationInSecs();
      }

      PRINTVBL2("Total search time", accum);
      PRINTVBL2("Average time per sample", accum / flatSamples.rows());
      PRINTVBL2("Average triangles checked", (0.0+avChecks) / flatSamples.rows());
      /*
      PRINTVBL(sqrt(rmsError / flatSamples.rows()));
      PRINTVBL(sqrt(maxError));

      PAUSE;
      */
   }

   void binFlattenedToMesh(const T* flatCoords, T* meshCoords, MeshIndex& faceIndex, MeshIndex& nchecks, const bool debug)
   {
      faceIndex = -1;
      {
         //TimedBlock block("binFlattenedToMesh");

         // Get the bin
         int bs[2];
         flatToBin(flatCoords, bs);

         // Work out the bins to search
         int bounds[] = { bs[0], bs[0], bs[1], bs[1] };

         //PRINTVEC(bs, 2);
         //PRINTVEC(binBounds, 4);

         // Calculate the corner where we are
         T corner[2];
         binToFlat(bs, corner);

         // Calculate how far from the corner we are
         const T flatDelta[] = { flatCoords[0]-corner[0], flatCoords[1]-corner[1] };

         // Expand until outside the total bin range, then bring it one back
         while (bounds[0] >= binBounds[0]) bounds[0] -= nbinsPerSquare; bounds[0] += nbinsPerSquare;
         while (bounds[1] <= binBounds[1]) bounds[1] += nbinsPerSquare; bounds[1] -= nbinsPerSquare;
         while (bounds[2] >= binBounds[2]) bounds[2] -= nbinsPerSquare; bounds[2] += nbinsPerSquare;
         while (bounds[3] <= binBounds[3]) bounds[3] += nbinsPerSquare; bounds[3] -= nbinsPerSquare;

         nchecks = 0;
         
         // Now, check all of the bins
         T vs[3][3];
         T deltas[3][3];

         {
            MeshIndex bestFace = std::numeric_limits<MeshIndex>::max();
            T bestMinCross = -std::numeric_limits<T>::infinity();
            T bestCorner[2];

            //PRINTVEC(bounds,4);
            //PRINTVBL(nbinsPerSquare);

            int y;
            for (y = bounds[2]; y <= bounds[3] && bestMinCross <= 0; y += nbinsPerSquare)
            {
               int x;
               for (x = bounds[0]; x <= bounds[1] && bestMinCross <= 0; x += nbinsPerSquare)
               {
                  bs[0] = x; bs[1] = y;

                  if (debug)
                  {
                     PRINTVEC(bs,2);
                     DEBUGMACRO("x=" << x << ", y=" << y);
                  }

                  std::vector<MeshIndex>& vec = facesInBin[Bin(x,y)];

                  // Go to the appropriate position in this square
                  binToFlat(bs,corner);
                  corner[0] += flatDelta[0];
                  corner[1] += flatDelta[1];

                  if (debug)
                  {
                     ColouredConsole cons(ColouredConsole::COL_RED | ColouredConsole::COL_BRIGHT);
                     PRINTVEC(corner, 2);
                     PRINTVEC(flatCoords,2);
                  }

                  if (debug)
                  {
                     ColouredConsole cons(ColouredConsole::COL_RED | ColouredConsole::COL_BRIGHT);
                     PRINTVEC(corner, 2);
                  }

                  //PRINTVBL(vec.size());

                  {
                     for (std::vector<MeshIndex>::iterator it = vec.begin(), en = vec.end(); bestMinCross < 0 && it != en; ++it)
                     {
                        ++nchecks;
                        // Work out if inside that bin
                        textureMesh->getFace(*it, vs[0], vs[1], vs[2]);

                        if (debug)
                        {
                           ColouredConsole cons(ColouredConsole::COL_GREEN | ColouredConsole::COL_BRIGHT);
                           PRINTVBL(*it+1);
                        }

                        int cnt = 0;
                        T minCross = std::numeric_limits<T>::infinity();

                        for (MeshIndex v = 0; v < 3; ++v)
                        {
                           for (MeshIndex ax = 0; ax < 2; ++ax)
                           {
                              deltas[0][ax] = vs[(v+1)%3][ax] - vs[v][ax];
                              deltas[1][ax] = corner[ax]      - vs[v][ax];
                           }
                           
                           const T cross = deltas[0][0]*deltas[1][1] - deltas[1][0]*deltas[0][1];
                           minCross = std::min<T>(minCross,cross);
                        }

                        if (debug)
                        {
                           PRINTVBL(*it);
                           PRINTVEC(vs[0], 3);
                           PRINTVEC(vs[1], 3);
                           PRINTVEC(vs[2], 3);
                           PRINTVEC(corner, 2);
                           PRINTVBL(minCross);
                           PRINTVEC(bounds, 4);
                           BLANKLINE;
                        }

                        if (minCross > bestMinCross)
                        {
                           bestMinCross = minCross;
                           bestFace = *it;
                           bestCorner[0] = corner[0];
                           bestCorner[1] = corner[1];
                        }
                     }
                  }
               }
            }

            //PRINTVBL(bestMinCross);
            faceIndex = bestFace;

            // Get the projection of the point into the triangle
            T vtx2d[2];
            TriangleProjection<T>::projectOntoTriangle(textureMesh, bestFace, bestCorner, vtx2d);

            if (debug)
            {
               BLANKLINE;
               PRINTVBL(bestFace);
               PRINTVBL(bestMinCross);
               PRINTVEC(bestCorner, 2);
               PRINTVEC(vtx2d,2);
            }

            // Now, project it back into the non-flattened mesh
            TriangleProjection<T>::projectFromTriangle(mesh, bestFace, vtx2d, meshCoords);
            //break;

            /*
            PRINTVBL(crossDelta);

            //PRINTVBL(crossDelta);
            PAUSE;
            */
         }
      }

      if (faceIndex == -1)
      {
         PAUSE;
      }
      /*
      if (faceIndex != -1)
      {
         T vs[3][3];
         textureMesh->getFace(faceIndex, vs[0], vs[1], vs[2]);

         PRINTVEC(vs[0], 2);
         PRINTVEC(vs[1], 2);
         PRINTVEC(vs[2], 2);

         PRINTVEC(flatCoords, 2);
      }

      PRINTVBL(faceIndex);
      PAUSE;
      */
   }

   void writeIntersectionNotFound(const T* flatCoords)
   {
      vnl_matrix<T> pnt(2, 1);
      pnt.set_column(0, flatCoords);

      vnl_matrix<T> vertices(3, textureMesh->getNumberOfVertices());

      T vtx[3];
      MESH_EACHVERTEX(textureMesh, v)
      {
         textureMesh->getVertex(v, vtx);
         vertices.set_column(v, vtx);
      }

      DEBUGLINE;

      MeshIndex vs[3];
      vnl_matrix<T> faces(3, textureMesh->getNumberOfFaces()*6);
      MeshIndex i = 0;
      MESH_EACHFACE(textureMesh, f)
      {
         textureMesh->getFaceIndices(f, vs);

         faces(0, i+0) = vs[0];
         faces(1, i+0) = vs[1];
         faces(2, i+0) = 1;

         faces(0, i+1) = vs[1];
         faces(1, i+1) = vs[2];
         faces(2, i+1) = 1;

         faces(0, i+2) = vs[2];
         faces(1, i+2) = vs[0];
         faces(2, i+2) = 1;

         faces(0, i+3) = vs[1];
         faces(1, i+3) = vs[0];
         faces(2, i+3) = 1;

         faces(0, i+4) = vs[2];
         faces(1, i+4) = vs[1];
         faces(2, i+4) = 1;

         faces(0, i+5) = vs[0];
         faces(1, i+5) = vs[2];
         faces(2, i+5) = 1;

         i += 6;
      }

      DEBUGLINE;
      vnl_matlab_filewrite fw("nointersection.mat");
      fw.write(vertices, "vertices");
      fw.write(faces, "faces");
      fw.write(pnt, "pnt");
   }

   void mapFlattenedToMesh(FacesRayIntersectionSearch<T>::P nearestSearch, const T* flatCoords, T* meshCoords, MeshIndex& faceIndex)
   {
      T testPoint[3];
      const T testNormal[] = { 0, 0, 1 };

      T intersectCoords[3];
      int intersectFace = -1;

      const T wrapFlat[] = {
         flatCoords[0] - floor(flatCoords[0]) + faceSearchOffset[0],
         flatCoords[1] - floor(flatCoords[1]) + faceSearchOffset[1],
      };

      int x = 0, y = 0;
      do
      {
         testPoint[0] = wrapFlat[0] + x;
         testPoint[1] = wrapFlat[1] + y;
         testPoint[2] = 0;

         nearestSearch->reset();
         nearestSearch->setTestPoint(testPoint);
         nearestSearch->setTestNormal(testNormal);
         textureMesh->search(nearestSearch);

         intersectFace = nearestSearch->getNearestPoint(intersectCoords);

         const T dns = deltaNormSquared(intersectCoords, testPoint, 3);
         
         if (intersectFace != -1)
            break;

         intersectFace = -1;
         walkInASpiral(x, y);
      }
      while (abs(x) < 3 || abs(y) < 3);
      
      if (intersectFace == -1)
      {
         DEBUGLINE;
         writeIntersectionNotFound(flatCoords);
         DEBUGLINE;
         throw "Could not find intersection";
      }

      faceIndex = intersectFace;

      // Get the projection of the point into the triangle
      T vtx2d[2];
      TriangleProjection<T>::projectOntoTriangle(textureMesh, faceIndex, intersectCoords, vtx2d);

      // Now, project it back into the non-flattened mesh
      TriangleProjection<T>::projectFromTriangle(mesh, faceIndex, vtx2d, meshCoords);

      /*
      const T d = deltaNormSquared<T>(meshCoords,bMeshCoords,3);

      if (d > 1e-16)
      {
         PRINTVBL(d);
         PAUSE;
      }
      */
   }

   void buildTextureMesh()
   {
      textureMesh = MeshImpl<T>::getInstance(0, 0);

      // Same connectivity as the mesh
      textureMesh->setFaces(mesh->getFaces());

      // Project the 2-element tex coords to 3-elements
      MatrixType proj(3, 2);
      proj.set_identity();

      TuplesType::P vertices = TuplesFunctions<T>::transform(mesh->getTextureCoords(), proj);
      textureMesh->setVertices(vertices);

      textureMesh->prepareToSearchFaces();

      T centroid[3];
      MeshFunctions<T>::calculateVertexCentroid(textureMesh, centroid);
      faceSearchOffset[0] = floor(centroid[0] + 0.5) - 1;
      faceSearchOffset[1] = floor(centroid[1] + 0.5) - 1;

      binFlatFaces();
   }

   T calculateWeight1D(const T spos, const T kpos, const T scale, const BSpline<T>& bspline)
   {
      T ret = bspline.getValue((kpos-spos)/scale);

      // Handle the wrapping automatically - keep on going until the kernel's value is zero
      int dx = 1;
      T dRet;
      do
      {
         dRet = bspline.getValue((+dx + kpos - spos) / scale) + bspline.getValue((-dx + kpos - spos) / scale);

         ret += dRet;
         ++dx;
      }
      while (dRet != 0);

      return ret;
   }

   T calculateWeight(const T spos[2], const T kpos[2], const T scale[2], const BSpline<T>& bspline)
   {
      return calculateWeight1D(spos[0], kpos[0], scale[0], bspline) * calculateWeight1D(spos[1], kpos[1], scale[1], bspline);
   }

   void buildRefinedWeightMatrix(const Idx nk[2], vnl_sparse_matrix<T>& weights)
   {
      const Idx N = flatSamplesOrig.rows();
      const Idx K = nk[0]*nk[1];

      // Samples along rows, kernels down columns
      weights.set_size(N, K);

      const double kernelscale[] = {
         TYPED_DIV(double, 1, nk[0]),
         TYPED_DIV(double, 1, nk[1]),
      };
      
      const BSpline<T> bspline(256);

      T spos[2];
      T kpos[2];
      for (Idx c = 0; c < K; ++c)
      {
         // Calculate where the kernel is
         kpos[0] = TYPED_DIV(double, c % nk[0], nk[0]);
         kpos[1] = TYPED_DIV(double, c / nk[0], nk[1]);

         for (Idx r = 0; r < N; ++r)
         {
            spos[0] = flatSamplesOrig(r,0);
            spos[1] = flatSamplesOrig(r,1);

            const T ww = calculateWeight(spos, kpos, kernelscale, bspline);

            if (ww != 0)
               weights(r, c) = ww;
         }
      }
   }

   void getDisplacements(MatrixType& displacements)
   {
      displacements.set_size(flatSamplesOrig.rows(), 2);

      VectorType dispcol(flatSamplesOrig.rows());

      for (Idx ax = 0; ax < 2; ++ax)
      {
         // This is the current displacement on this axis
         weights.mult(controls.get_column(ax), dispcol);
         displacements.set_column(ax, dispcol);
      }
   }

   void setDisplacements(const MatrixType& displacements)
   {
      const Idx NN = nk[0]*nk[1];

      controls.set_size(NN, 2);
      
      for (Idx ax = 0; ax < 2; ++ax)
      {
         // Build the linear system by which to solve this...
         VectorType dispAx = displacements.get_column(ax);
         vnl_sparse_matrix_linear_system<T> linsys(weights, dispAx);
         vnl_lsqr lsqr(linsys);

         VectorType controlAx(NN);
         lsqr.minimize(controlAx);

         controls.set_column(ax, controlAx);
      }
   }

   void binToFlat(const int* bs, T* vs)
   {
      vs[0] = TYPED_DIV(T, bs[0], nbinsPerSquare);
      vs[1] = TYPED_DIV(T, bs[1], nbinsPerSquare);
   }

   void flatToBin(const T* vs, int* bs)
   {
      bs[0] = (int)floor(vs[0] * nbinsPerSquare);
      bs[1] = (int)floor(vs[1] * nbinsPerSquare);
   }

   void binFlatFaces()
   {
      facesInBin.clear();

      T vs[3][3];
      int bounds[4];
      int bs[2];
      Bin bin;

      {
         // Calculate all the bins needed

         // Reset the bounds
         bounds[0] = bounds[2] =  std::numeric_limits<int>::max();
         bounds[1] = bounds[3] = -std::numeric_limits<int>::max();
         MESH_EACHVERTEX(textureMesh, v)
         {
            textureMesh->getVertex(v,vs[0]);

            flatToBin(vs[0], bs);

            bounds[0] = std::min(bounds[0], bs[0]);
            bounds[1] = std::max(bounds[1], bs[0]);

            bounds[2] = std::min(bounds[2], bs[1]);
            bounds[3] = std::max(bounds[3], bs[1]);
         }

         // Store the bounds of the bins
         for (int i = 0; i < 4; ++i)
            binBounds[i] = bounds[i];

         // Create all of these bins
         for (int y = bs[2]; y <= bs[3]; ++y)
            for (int x = bs[0]; x <= bs[1]; ++x)
               facesInBin[ Bin(x,y) ] = std::vector<MeshIndex>();
         
      }

      PRINTVBL(textureMesh->getNumberOfFaces());
      MESH_EACHFACE(textureMesh, f)
      {
         // Reset the bounds
         bounds[0] = bounds[2] =  std::numeric_limits<int>::max();
         bounds[1] = bounds[3] = -std::numeric_limits<int>::max();

         textureMesh->getFace(f, vs[0], vs[1], vs[2]);

         for (MeshIndex v = 0; v < 3; ++v)
         {
            flatToBin(vs[v], bs);

            bounds[0] = std::min(bounds[0], bs[0]);
            bounds[1] = std::max(bounds[1], bs[0]);

            bounds[2] = std::min(bounds[2], bs[1]);
            bounds[3] = std::max(bounds[3], bs[1]);
         }

         // Push this into any bin intersecting the bounding box
         for (bin.first = bounds[0]; bin.first <= bounds[1]; ++bin.first)
            for (bin.second = bounds[2]; bin.second <= bounds[3]; ++bin.second)
            {
               facesInBin[bin].push_back(f);
            }
      }
   }
};

AWT::AlignParametric::SampledTorusMesh::SampledTorusMesh(MeshType::P mesh, const TuplesType::P flatSamples, const Idx nk[2])
{
   m_D = new D;

   if (!mesh->hasTextureCoords())
      AWTEXCEPTIONTHROW("Mesh must have texture coordinates!");

   if (mesh->getVertices()->getTupleSize() != 3)
      AWTEXCEPTIONTHROW("Mesh must be 3-dimensional!");

   if (flatSamples->getTupleSize() != 2)
      AWTEXCEPTIONTHROW("Samples must be 2-dimensional!");

   m_D->nk[0] = nk[0];
   m_D->nk[1] = nk[1];

   const Idx N = flatSamples->getNumberOfPoints();
   const Idx K = nk[0]*nk[1];

   // Keep hold of the mesh
   m_D->mesh = mesh;

   // Change the sample locations into a vnl matrix
   m_D->flatSamplesOrig.set_size(N, 2);
   T vtx[2];
   for (MeshIndex v = 0; v < N; ++v)
   {
      flatSamples->getPoint(v, vtx);

      m_D->flatSamplesOrig(v,0) = vtx[0];
      m_D->flatSamplesOrig(v,1) = vtx[1];
   }
   m_D->flatSamples = m_D->flatSamplesOrig;

   // Build a mesh from the texture coordinates, so that you can look up which face the sample point is over
   //DEBUGMACRO("Building texture mesh");
   m_D->buildTextureMesh();

   // Build weighting matrix
   //DEBUGMACRO("Building weight matrix");
   m_D->buildRefinedWeightMatrix(nk, m_D->weights);

   // Set the control values to zero
   //DEBUGMACRO("Initializing controls");
   m_D->controls.set_size(K, 2);
   m_D->controls.fill(0);

   //DEBUGMACRO("Setting parameters");
   setParameters(m_D->controls);
   //DEBUGMACRO("Done.");

   // Calculate the 5th-%ile triangle area, use this to avoid truncation error
   std::vector<T> areas;
   MESH_EACHFACE(mesh, f)
      areas.push_back(MeshFunctions<T>::getFaceArea(mesh, f));

   sort(areas.begin(), areas.end());

   //DEBUGMACRO("Setting maxmove");
   m_D->maxMove = sqrt(areas[ areas.size() *  5 / 100 ]) / 2;

   //PAUSE;
}

AWT::AlignParametric::SampledTorusMesh::~SampledTorusMesh()
{
   delete m_D;
}

AWT::AlignParametric::SampledTorusMesh::P AWT::AlignParametric::SampledTorusMesh::getInstance(MeshType::P mesh, const TuplesType::P flatSamples, const Idx nk[2])
{
   AUTOGETINSTANCE(AWT::AlignParametric::SampledTorusMesh, (mesh, flatSamples, nk));
}

GETNAMEMACRO(AWT::AlignParametric::SampledTorusMesh);

// Get the number of parameter components describing each sample
Idx AWT::AlignParametric::SampledTorusMesh::getParameterDimensionality() const
{
   return 2;
}

// Get the number of parameters which control this sampling
Idx AWT::AlignParametric::SampledTorusMesh::getNumberOfParameters() const
{
   return m_D->controls.rows();
}

// Get the number of flatSamples on the surface
Idx AWT::AlignParametric::SampledTorusMesh::getNumberOfSamples() const
{
   return m_D->flatSamplesOrig.rows();
}

// Get the current sample locations
void AWT::AlignParametric::SampledTorusMesh::getSamples(MatrixType& flatSamples) const
{
   flatSamples = m_D->meshSamples;
}


// How far can samples be moved in order to maintain stability?
T AWT::AlignParametric::SampledTorusMesh::getMaxMove() const
{
   return m_D->maxMove;
}

// Get the current set of control values
void AWT::AlignParametric::SampledTorusMesh::getParameters(MatrixType& controls) const
{
   controls = m_D->controls;
}

// Update the current set of control values
void AWT::AlignParametric::SampledTorusMesh::setParameters(MatrixType& controls)
{
   if (controls.rows() != m_D->controls.rows() || controls.columns() != m_D->controls.columns())
      AWTEXCEPTIONTHROW("Controls matrix is wrong size!");

   //DEBUGMACRO("Controls are correct size, attempting to set");
   m_D->setParameters(controls);
}

// Calculates the jacobian matrix of the el-th sample location w.r.t. the p-th parameters
void AWT::AlignParametric::SampledTorusMesh::jacobian(const Idx el, const Idx p, MatrixType& matrix) const
{
   const T w = m_D->weights(el, p);
   if (w != 0)
   {
      matrix = m_D->getSampleJacobian(el) * w;
   }
   else
   {
      matrix.set_size(3, 2);
      matrix.fill(0);
   }
}

// Iterator functions - allows you to skip all the zero jacobians
// Takes the internal iterator back to the start
void AWT::AlignParametric::SampledTorusMesh::resetIterator()
{
   m_D->weights.reset();
}

// Advances to the next non-zero jacobian pair
bool AWT::AlignParametric::SampledTorusMesh::next(Idx& el, Idx& p)
{
   const bool ret = m_D->weights.next();

   if (ret)
   {
      el = m_D->weights.getrow();
      p = m_D->weights.getcolumn();
   }
   else
   {
      el = p = INVALID_INDEX;
   }
   
   return ret;
}

void AWT::AlignParametric::SampledTorusMesh::refine()
{
   // Firstly, record the current displacements.  
   // This is what we are trying to get back to.
   MatrixType displacements;
   m_D->getDisplacements(displacements);

   // Double the number of kernels
   Idx newNk[] = {
      m_D->nk[0] * 2,
      m_D->nk[1] * 2,
   };
   const Idx NN = newNk[0]*newNk[1];

   
   if (NN > m_D->flatSamplesOrig.rows())
   {
      DEBUGMACRO("Won't refine, since this would mean more kernels than samples");
      return;
   }

   vnl_sparse_matrix<double> newWeights;

   // Rebuild the weight matrix for the increased number of kernels
   m_D->buildRefinedWeightMatrix(newNk, newWeights);

   // Only once the new weight matrix has been built shall it be swapped in
   m_D->nk[0]   = newNk[0];
   m_D->nk[1]   = newNk[1];
   m_D->weights = newWeights;

   m_D->setDisplacements(displacements);
}

// Calculate the regularization cost
T AWT::AlignParametric::SampledTorusMesh::regularizationCost()
{
   return 0;
}

// Calculate a regularization term
void AWT::AlignParametric::SampledTorusMesh::regularizationGradient(MatrixType& reg, const Transformation& trans)
{
   // Actually, this surface type is sort-of self-regularizing, because of the smoothness of the parametrization
   // I suppose you could build in some sort of curvature regularization, but I don't want to

   // Just make it compatible for addition, and fill with zeros
   reg.set_size(m_D->controls.rows(), m_D->controls.cols());
   reg.fill(0);
}
