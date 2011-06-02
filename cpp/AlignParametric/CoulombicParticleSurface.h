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
#ifndef __CoulombicParticleSurface_H__
#define __CoulombicParticleSurface_H__

#include "ParticleSurface.h"
#include "AlignParametric/CoulombicRegularizer.h"
#include "AlignParametric/TypeDefs.h"
#include "Mesh/Mesh.h"
#include "MeshSearching/FacesNearestPointSearch.h"

#include <iostream>

namespace AWT
{
   namespace AlignParametric
   {
      class CoulombicParticleSurface : public ParticleSurface
      {
         friend class CoulombicRegularizer;

      public:
         typedef ManagedAutoPointer<CoulombicParticleSurface> P;

      protected:
         CoulombicParticleSurface(MeshType::P mesh, const TuplesType::P samples, const Idx ntake);
         virtual ~CoulombicParticleSurface();

      public:
         static P getInstance(MeshType::P mesh, const TuplesType::P samples, const Idx ntake);
         virtual std::string getClassName() const;

         // Distance to the closest neighbour particle
         virtual T getMinDistance(const Idx p);

         // Calculate the regularization cost
         virtual T regularizationCost();

         // Calculate the regularization update
         virtual void regularizationGradient(MatrixType& reg, const Transformation& trans);

         virtual CoulombicRegularizer::P getRegularizer();

      protected:
         CoulombicRegularizer::P regularizer;

         virtual int updatePointLocation(const Idx i, const T* vtx, FacesNearestPointSearch<T>::P searcher);
      };
   }
}

#endif // __CoulombicParticleSurface_H__