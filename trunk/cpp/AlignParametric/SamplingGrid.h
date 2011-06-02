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
#ifndef __SAMPLINGGRID_H__
#define __SAMPLINGGRID_H__

#include "ObjectManagement/ManagedObject.h"
#include "TypeDefs.h"

#include "vnl/vnl_matrix.h"
#include "vnl/vnl_sparse_matrix.h"

#include "MeshBreaker/FlattenMeshPair.h"

namespace AWT
{
   namespace AlignParametric
   {
      class SamplingGrid : public ManagedObject
      {
      protected:
         SamplingGrid(const Idx _ns[2], const Idx _nk[2]);
         virtual ~SamplingGrid();

      protected:
         Idx ns[2];
         Idx nk[2];

         KernelWeights weightings;
         ControlValues controlValues[2];
      public:
         typedef ManagedAutoPointer<SamplingGrid> P;
         static P getInstance(const Idx _ns[2], const Idx _nk[2]);

         std::string getClassName() const;

         void getNSamples(Idx ns[2]) const;
         void getNKernels(Idx nk[2]) const;

         const KernelWeights&  getWeights() const;
         ControlValues& getParameters(Idx axis);

         void getPositions(Displacements positions[2]) const;
         void getDisplacements(Displacements positions[2]) const;

         // Work out the control values necessary to yield the same displacements
         void calculateFromOther(const SamplingGrid& other);

      protected:
         static void getPointPosition(const Idx ns[2], const Idx i, double pos[2]);
      };
   }
}

#endif // __SAMPLINGGRID_H__