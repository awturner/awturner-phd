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
#ifndef __SAMPLINGGRIDIRREGULAR_H__
#define __SAMPLINGGRIDIRREGULAR_H__

#include "ObjectManagement/ManagedObject.h"
#include "TypeDefs.h"
#include "Maths/BSpline.h"

#include "vnl/vnl_matrix.h"
#include "vnl/vnl_sparse_matrix.h"

#include "MeshBreaker/FlattenMeshPair.h"

namespace AWT
{
   namespace AlignParametric
   {
      class SamplingGridIrregular : public ManagedObject
      {
      protected:
         SamplingGridIrregular(TuplesType::P sampleLocations, const Idx _nk[2]);
         virtual ~SamplingGridIrregular();

      protected:
         TuplesType::P originalSampleLocations;
         Idx nk[2];

         static T calculateWeight(const T spos[2], const T kpos[2], const T scale[2], const BSpline<T>& bspline);

         KernelWeights weightings;
         mutable ControlValues controlValues[2];
      public:
         typedef ManagedAutoPointer<SamplingGridIrregular> P;
         static P getInstance(TuplesType::P sampleLocations, const Idx _nk[2]);

         std::string getClassName() const;

         TuplesType::P getOriginalSampleLocations();
         void getNKernels(Idx nk[2]) const;

         void getPositions(Displacements positions[2]) const;
         void getDisplacements(Displacements positions[2]) const;
         
         const KernelWeights&  getWeights() const;
         ControlValues& getParameters(Idx axis);
      };
   }
}

#endif // __SAMPLINGGRIDIRREGULAR_H__