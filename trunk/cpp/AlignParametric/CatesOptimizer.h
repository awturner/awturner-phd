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
#ifndef __CatesOptimizer_H__
#define __CatesOptimizer_H__

#include "ModelEigenvalueOptimizer.h"
#include "Useful/ValueRange.h"
#include "CatesParticleSurface.h"
#include "ParticleOptimizer.h"

namespace AWT
{
   namespace AlignParametric
   {
      class CatesOptimizer : public ParticleOptimizer
      {
      public:
         typedef ManagedAutoPointer<CatesOptimizer> P;

      protected:
         CatesOptimizer(MeshIndex N, ValueRange<T> _sigmaRange);
         virtual ~CatesOptimizer();

      public:
         static P getInstance(MeshIndex N, ValueRange<T> _sigmaRange);
         virtual std::string getClassName() const;

         virtual void setSigmaRange(ValueRange<T> range);
         virtual ValueRange<T> getSigmaRange() const;

         virtual void setDistanceType(const CatesRegularizer::DistanceType dt);
         virtual CatesRegularizer::DistanceType getDistanceType() const;

         virtual ParticleSurface::P addMesh(MeshType::P mesh, TuplesType::P particles);

      protected:
         ValueRange<T> sigmaRange;
         CatesRegularizer::DistanceType distanceType;
      };
   }
}

#endif // __CatesOptimizer_H__