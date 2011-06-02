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
#ifndef __ALIGNOBJECT_H__
#define __ALIGNOBJECT_H__

#include "ObjectManagement/ManagedObject.h"

#include "Mesh.h"
#include "TypeDefs.h"
#include "Search.h"
#include "Sampler.h"

namespace AWT
{
   namespace SimpleMesh
   {
      class AlignObject : public ManagedObject
      {
      protected:
         Mesh::P mesh;
         Vector params;
         Search::P searcher;

         Sampler::P sampler;
         PIWs samples;

         const Index nsamples;

         AlignObject(const Index nsamples);

      public:
         typedef ManagedAutoPointer<AlignObject> P;

         Vector getParameters() const;

         void incrementParameters(const Vector& inc);

         virtual void setParameters(const Vector& params);

         virtual Mesh::P getMesh();

         void resample();

         Index getMaxNumberOfSamples();

         PIWs& getSamples();

         virtual PointIndexWeights search(const Point& pnt, const Point& nml, SearchFilter::P filter) = 0;

         virtual void calculateJacobian(const PointIndexWeights& p, Matrix& jac) = 0;
      };
   }
}

#endif // __ALIGNOBJECT_H__