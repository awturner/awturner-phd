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
#ifndef __NORMALSPACESAMPLER_H__
#define __NORMALSPACESAMPLER_H__

#include "ObjectManagement/ManagedObject.h"

#include "Sampler.h"

namespace AWT
{
   namespace SimpleMesh
   {
      class NormalSpaceSampler : public Sampler
      {
      public:
         typedef ManagedAutoPointer<NormalSpaceSampler> P;

      protected:
         NormalSpaceSampler(Mesh::P mesh, const unsigned int latdivs);
         virtual ~NormalSpaceSampler();

         virtual void calculateCumAreas();

      public:
         static P getInstance(Mesh::P mesh, const unsigned int latdivs);
         virtual std::string getClassName() const;

      protected:
         const unsigned int latdivs;
      };

      class NormalSpaceSamplerFactory : public SamplerFactory
      {
      protected:
         NormalSpaceSamplerFactory(const unsigned int latdivs);
         virtual ~NormalSpaceSamplerFactory();

      public:
         typedef ManagedAutoPointer<NormalSpaceSamplerFactory> P;
         static P getInstance(const unsigned int latdivs);
         std::string getClassName() const;

         virtual Sampler::P createInstance(Mesh::P mesh);

      protected:
         const unsigned int latdivs;
      };
   }
}

#endif // __NORMALSPACESAMPLER_H__