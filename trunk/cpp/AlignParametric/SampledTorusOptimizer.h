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
#ifndef __SAMPLEDTORUSOPTIMIZER_H__
#define __SAMPLEDTORUSOPTIMIZER_H__

#include "ModelEigenvalueOptimizer.h"
#include "SampledTorusMesh.h"

namespace AWT
{
   namespace AlignParametric
   {
      class SampledTorusOptimizer : public ModelEigenvalueOptimizer
      {
      public:
         typedef ManagedAutoPointer<SampledTorusOptimizer> P;

      protected:
         SampledTorusOptimizer();
         virtual ~SampledTorusOptimizer();

      public:
         static P getInstance();
         virtual std::string getClassName() const;

         virtual void setSamplingMesh(MeshType::P mesh);
         MeshType::P getSamplingMesh();

         virtual void setNumberOfKernels(const MeshIndex nk);
         virtual void setNumberOfKernels(const MeshIndex nk[2]);
         virtual void getNumberOfKernels(MeshIndex nk[2]);

         virtual SampledTorusMesh::P addMesh(MeshType::P mesh);

      protected:
         virtual void calculateSampleWeights(const MatrixType& meanShape, VectorType& sampleAreas);
         virtual FaceType::P getFaces();

         MeshType::P samplingMesh;
         MeshIndex nk[2];
      };
   }
}

#endif // __SAMPLEDTORUSOPTIMIZER_H__