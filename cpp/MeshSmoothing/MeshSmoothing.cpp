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
#include "MeshSmoothing.h"

#include "Mesh/MeshImpl.h"

#include "SmoothingCallback.h"
#include "MeshSearching/FacesInRadiusSearch.h"
#include "Mesh/AreaAveragedNormalCalculator.h"

#include <limits>

#include "Useful/Exception.h"

template <class T>
typename AWT::Mesh<T>::P AWT::MeshSmoothing<T>::smooth(typename Mesh<T>::P m, T spatialSigma, T influenceSigma)
{
   Mesh<T>::P mollifiedMesh = smoothImpl(m, spatialSigma/2, std::numeric_limits<T>::infinity(), true,  0               );
   
   Tuples<T>::P mollifiedNormals = mollifiedMesh->getFaceNormals();

   T pnt[3];

   Mesh<T>::P ret           = smoothImpl(m, spatialSigma,   influenceSigma,                     false, mollifiedNormals);

   return ret;
}

template <class T>
typename AWT::Mesh<T>::P AWT::MeshSmoothing<T>::smoothImpl(typename AWT::Mesh<T>::P m, T spatialSigma, T influenceSigma, bool mollify, typename AWT::Tuples<T>::P n)
{
   FacesInRadiusSearch<T>::P searcher = FacesInRadiusSearch<T>::getInstance();
   SmoothingCallback<T>::P   callback = SmoothingCallback<T>::getInstance();

   if (mollify)
      n = m->getFaceNormals();
   
   if (*n == 0)
      AWTEXCEPTIONTHROW("Mesh has no normals!");

   T def[] = { 0, 0, 0 };
   typename AWT::Mesh<T>::P newMesh = MeshImpl<T>::getInstance(m);

   searcher->setCallback(callback);
   searcher->setRadius(2 * spatialSigma);

   // Set up the callback into the right mode
   callback->setMollifying(mollify);
   callback->setSpatialSigma(spatialSigma);
   callback->setInfluenceSigma(influenceSigma);
   callback->setMesh(m);

   T vtx[3];

   MESH_EACHVERTEX(m, v)
   {
      if (v % 100 == 0)
         std::cerr << "v = " << v << " / " << m->getNumberOfVertices() << std::endl;

      m->getVertex(v, vtx);

      callback->reset();
      searcher->reset();

      callback->setTestPoint(vtx);
      searcher->setTestPoint(vtx);
      
      m->searchFaces(searcher);

      callback->getPredictedPoint(vtx);

      newMesh->setVertex(v, vtx);
   }

   AreaAveragedNormalCalculator<T>::getInstance()->calculateNormalsAndSet(newMesh);

   return newMesh;
}

template class AWT::MeshSmoothing<double>;
template class AWT::MeshSmoothing<float>;