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
#ifndef __FRAGMENTOBJECT_H__
#define __FRAGMENTOBJECT_H__

#include "AlignObject.h"
#include "MeshEdgeSearchFilter.h"

namespace AWT
{
   namespace SimpleMesh
   {
      class FragmentObject : public AlignObject
      {
      public:
         typedef ManagedAutoPointer<FragmentObject> P;

      protected:
         FragmentObject(Mesh::P frag, Sampler::P sampler, Index nsamples);
         virtual ~FragmentObject();

      public:
         static P getInstance(Mesh::P frag, Sampler::P sampler, Index nsamples);

         std::string getClassName() const;

         void setParameters(const Vector& params);

         virtual PointIndexWeights search(const Point& pnt, const Point& nml, SearchFilter::P filter);

         Transformation getTransformation() const;

         Transformation getInverseTransformation() const;

         SearchFilter::P getEdgeFilter();

         virtual void calculateJacobian(const PointIndexWeights& p, Matrix& jac);

      protected:
         struct D;
         D* m_D;
      };
   }
}

#endif // __FRAGMENTOBJECT_H__