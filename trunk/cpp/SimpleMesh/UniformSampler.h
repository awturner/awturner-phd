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
#ifndef __UNIFORMSAMPLER_H__
#define __UNIFORMSAMPLER_H__

#include "ObjectManagement/ManagedObject.h"

#include "Sampler.h"

namespace AWT
{
   namespace SimpleMesh
   {
      class UniformSampler : public Sampler
      {
      public:
         typedef ManagedAutoPointer<UniformSampler> P;

      protected:
         UniformSampler( Mesh::P mesh );
         virtual ~UniformSampler( );

         virtual void calculateCumAreas( );

      public:
         static P getInstance( Mesh::P mesh );
         virtual std::string getClassName( ) const;
      };

      class UniformSamplerFactory : public SamplerFactory
      {
      protected:
         UniformSamplerFactory( );
         virtual ~UniformSamplerFactory( );

      public:
         typedef ManagedAutoPointer<UniformSamplerFactory> P;
         static P getInstance( );
         std::string getClassName( ) const;

         virtual Sampler::P createInstance( Mesh::P mesh );
      };
   }
}

#endif // __UNIFORMSAMPLER_H__