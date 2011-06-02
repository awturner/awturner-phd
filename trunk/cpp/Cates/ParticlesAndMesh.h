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
#ifndef __PARTICLESANDMESH_H__
#define __PARTICLESANDMESH_H__

#include "ObjectManagement/ManagedObject.h"

#include "Mesh/Mesh.h"
#include "Mesh/Tuples.h"
#include "Useful/ValueRange.h"

namespace AWT
{
   template <class T>
   class ParticlesAndMesh : public ManagedObject
   {
   public:
      typedef ManagedAutoPointer<ParticlesAndMesh<T> > P;

   protected:
      ParticlesAndMesh(typename Mesh<T>::P mesh, typename Tuples<T>::P particles);
      virtual ~ParticlesAndMesh();

   public:
      static P getInstance(typename Mesh<T>::P mesh, typename Tuples<T>::P particles);
      virtual std::string getClassName() const;

      typename Mesh<T>::P getMesh();
      typename Tuples<T>::P getParticles();
      typename Tuples<T>::P getSigma();

      // Add the change on to the particles, and then reproject the points back onto the surface
      void updateParticles(const typename Tuples<T>::P delta, T mult, typename Tuples<T>::P actualDelta);

      void setSigmaRange(const ValueRange<T>& range);

      void splitParticles();
      void splitParticle(const MeshIndex i);

      T getMinSigma() const;

      // Calculate the force which tries to push these particles apart
      T calculateInternalForceUpdate(const ValueRange<T>& sigmaRange, typename Tuples<T>::P delta, T& maxTimestep);

      void debugSigmas(const ValueRange<T>& sigmaRange);

   protected:
      struct D;
      D* m_D;

   };
}

#endif // __PARTICLESANDMESH_H__