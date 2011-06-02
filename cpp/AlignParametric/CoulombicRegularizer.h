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
#ifndef __COULOMBICREGULARIZER_H__
#define __COULOMBICREGULARIZER_H__

#include "ObjectManagement/ManagedObject.h"

#include "Mesh/Mesh.h"
#include "Mesh/Tuples.h"
#include "Useful/ValueRange.h"
#include "TypeDefs.h"

#include "Cates/ParticlesAndMesh.h"

namespace AWT
{
   namespace AlignParametric
   {
      class CoulombicParticleSurface;

      class CoulombicRegularizer : public ManagedObject
      {
      public:
         typedef ManagedAutoPointer<CoulombicRegularizer> P;

      protected:
         CoulombicRegularizer(CoulombicParticleSurface* surf);
         virtual ~CoulombicRegularizer();

      public:
         static P getInstance(CoulombicParticleSurface* surf);
         virtual std::string getClassName() const;

         T getCutoffDistance() const;
         void setCutoffDistance(const T v);

         T getThreshold() const;
         void setThreshold(const T v);

         virtual T getMinDistance(const Idx p);

         // Calculate the force which tries to push these particles apart
         void calculateUpdate(MatrixType& reg);

         T calculateCost(const bool debug = true);

         void associateSampleToFace(const MeshIndex i, const MeshIndex f);

      protected:
         struct D;
         D* m_D;

      };
   }
}

#endif // __COULOMBICREGULARIZER_H__