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
#include "FlattenMeshPair.h"

#include "vnl/vnl_matlab_filewrite.h"

#include "Mesh/MeshImpl.h"
#include "Mesh/MeshGenerator.h"
#include "Mesh/MeshFunctions.h"

#include "MeshSearching/FacesRayIntersectionSearch.h"
#include "MeshSearching/FacesNearestPointSearch.h"

#include "Useful/WalkInASpiral.h"

template <class T>
struct AWT::FlattenMeshPair<T>::D
{
   typename Mesh<T>::P mesh;
   typename Mesh<T>::P flat;

   void mapMeshToFlattened(const T* meshCoords, T* flatCoords, MeshIndex& faceIndex)
   {
      // We have to work out what the closest face to the test point is
      FacesNearestPointSearch<T>::P nearestSearch = FacesNearestPointSearch<T>::getInstance();
      nearestSearch->reset();
      nearestSearch->setTestPoint(meshCoords);
      mesh->search(nearestSearch);

      T pnt[3];
      int nearestPoint = nearestSearch->getNearestPoint(pnt);

      if (nearestPoint == -1)
         AWTEXCEPTIONTHROW("No point found!");

      faceIndex = nearestPoint;

      // Get the projection of the point into the triangle
      T vtx2d[2];
      TriangleProjection<T>::projectOntoTriangle(mesh, nearestPoint, pnt, vtx2d);

      // Now, project it back into the flattened mesh
      TriangleProjection<T>::projectFromTriangle(flat, nearestPoint, vtx2d, flatCoords);

      // Wrap the values into the range [0..1]
      //flatCoords[0] = wrap(flatCoords[0], 0, 1);
      //flatCoords[1] = wrap(flatCoords[1], 0, 1);
      flatCoords[2] = 0;
   }

   void mapFlattenedToMesh(const T* flatCoords, T* meshCoords, MeshIndex& faceIndex)
   {
      T testPoint[] = { flatCoords[0], flatCoords[1], -1 };
      T zz[] = { 0, 0, 1 };

      T isCoords[3];
      int isFace = -1;

      flat->prepareToSearchFaces();

      const T wrapFlat[] = {
         flatCoords[0] - floor(flatCoords[0]),
         flatCoords[1] - floor(flatCoords[1]),
      };

      // Search for intersections using either a nearest point or ray intersection search
      if (true)
      {
         FacesNearestPointSearch<T>::P nearestSearch = FacesNearestPointSearch<T>::getInstance();
         nearestSearch->reset();

         int x = 0, y = 0;
         do
         {
            testPoint[0] = wrapFlat[0] + x;
            testPoint[1] = wrapFlat[1] + y;
            testPoint[2] = 0;

            nearestSearch->reset();
            nearestSearch->setTestPoint(testPoint);
            flat->search(nearestSearch);

            isFace = nearestSearch->getNearestPoint(isCoords);

            if (deltaNormSquared(isCoords, testPoint, 3) < 1e-8)
               break;

            isFace = -1;
            walkInASpiral(x, y);
         }
         while (abs(x) < 3 || abs(y) < 3);
      }
      else
      {
         FacesRayIntersectionSearch<T>::P raySearch = FacesRayIntersectionSearch<T>::getInstance();
         raySearch->reset();
         raySearch->setTestNormal(zz);

         int x = 0, y = 0;
         while (isFace == -1 && (abs(x) < 3 || abs(y) < 3))
         {
            testPoint[0] = wrapFlat[0] + x;
            testPoint[1] = wrapFlat[1] + y;

            raySearch->reset();
            raySearch->setTestNormal(zz);
            raySearch->setTestPoint(testPoint);

            flat->search(raySearch);

            if ((isFace = raySearch->getNearestPoint(isCoords)) != -1)
               break;

            walkInASpiral(x, y);
         }
      }

      if (isFace == -1)
      {
         AWTEXCEPTIONTHROW("Could not find intersection");
      }

      faceIndex = isFace;

      // Get the projection of the point into the triangle
      T vtx2d[2];
      TriangleProjection<T>::projectOntoTriangle(flat, faceIndex, isCoords, vtx2d);

      // Now, project it back into the non-flattened mesh
      TriangleProjection<T>::projectFromTriangle(mesh, faceIndex, vtx2d, meshCoords);
   }

};

template <class T>
AWT::FlattenMeshPair<T>::FlattenMeshPair(typename Mesh<T>::P mesh, typename Mesh<T>::P flat)
{
   m_D = new D;

   m_D->mesh = mesh;
   m_D->flat = flat;
}

template <class T>
AWT::FlattenMeshPair<T>::~FlattenMeshPair()
{
   delete m_D;
}

template <class T>
typename AWT::FlattenMeshPair<T>::P AWT::FlattenMeshPair<T>::getInstance(typename Mesh<T>::P mesh, typename Mesh<T>::P flat)
{
   AUTOGETINSTANCE(AWT::FlattenMeshPair<T>, (mesh, flat));
}

template <class T>
GETNAMEMACRO(AWT::FlattenMeshPair<T>);

template <class T>
typename AWT::Mesh<T>::P AWT::FlattenMeshPair<T>::getMesh()
{
   return m_D->mesh;
}

template <class T>
typename AWT::Mesh<T>::P AWT::FlattenMeshPair<T>::getFlattenedMesh()
{
   return m_D->flat;
}

template <class T>
AWT::MeshIndex AWT::FlattenMeshPair<T>::mapMeshToFlattened(const T* meshCoords, T* flatCoords)
{
   MeshIndex ret;
   m_D->mapMeshToFlattened(meshCoords, flatCoords, ret);
   return ret;
}

template <class T>
AWT::MeshIndex AWT::FlattenMeshPair<T>::mapFlattenedToMesh(const T* flatCoords, T* meshCoords)
{
   MeshIndex ret;
   m_D->mapFlattenedToMesh(flatCoords, meshCoords, ret);
   return ret;
}

template <class T>
typename AWT::Mesh<T>::P AWT::FlattenMeshPair<T>::toShapeImage(const MeshIndex nu, const MeshIndex nv)
{
   // Generate a torus to give the topology
   Mesh<T>::P ret = MeshImpl<T>::getInstance(0, 0);
   MeshGenerator<T>::generateTorus(ret, 3, 1, nu, nv);
   
   T vtxMesh[3];
   T vtxFlat[2];

   for (MeshIndex v = 0; v < nv; ++v)
   {
      vtxFlat[1] = (0.0 + v) / (0.0 + nv);
      for (MeshIndex u = 0; u < nu; ++u)
      {
         vtxFlat[0] = (0.0 + u) / (0.0 + nu);

         mapFlattenedToMesh(vtxFlat, vtxMesh);

         ret->setVertex(v*nu + u, vtxMesh);
      }
   }

   return ret;
}

template class AWT::FlattenMeshPair<double>;