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
#ifndef __SAMPLEPOINTS_H__
#define __SAMPLEPOINTS_H__

#include "ObjectManagement/ManagedObject.h"
#include "MeshTypeDefs.h"

namespace AWT
{
   class SamplePoints : public virtual ManagedObject
   {
   public:
      typedef ManagedAutoPointer<SamplePoints> P;

   protected:
      SamplePoints(MeshIndex npoints = 0);
   
      virtual ~SamplePoints();

   public:
      static P getInstance(MeshIndex npoints = 0);
      
      const MeshIndex operator[](MeshIndex i) const;

      MeshIndex getNumberOfSamples() const;

      MeshIndex getSampleIndex(MeshIndex i) const;

      void ensureSize(MeshIndex i);

      void add(MeshIndex i);

      void clear();

      virtual std::string getClassName() const;

   protected:
      struct D;
      D* m_D;
   };
}

#endif // __SAMPLEPOINTS_H__
