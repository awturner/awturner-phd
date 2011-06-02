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
#ifndef __MESHFUNCTIONS_H__
#define __MESHFUNCTIONS_H__

#include "ObjectManagement/ManagedObject.h"

#include "MeshTypeDefs.h"

#include "Mesh.h"

#include <vnl/vnl_matrix_fixed.h>

namespace AWT
{
   template <class T>
   class MeshFunctions : public virtual ManagedObject
   {
   public:
      typedef ManagedAutoPointer<MeshFunctions<T> > P;

   protected:
      MeshFunctions();
      virtual ~MeshFunctions();

   public:
      static T getFaceArea(const typename Mesh<T>::P mesh, const MeshIndex in_Index);
      static T getSurfaceArea(const typename Mesh<T>::P mesh);

      static int getEulerCharacteristic(const typename Mesh<T>::P mesh);

      static void fixHoles(typename Mesh<T>::P mesh);

      static void calculateFaceCentroid(const typename Mesh<T>::P mesh, const MeshIndex in_Face, T* out_Centroid);

      static void calculateVertexCentroid(const typename Mesh<T>::P mesh, T* out_Centroid);
      static void calculateFaceCentroid(const typename Mesh<T>::P mesh, T* out_Centroid);

      static T calculateVariance(const typename Mesh<T>::P mesh);

      static void constructOrthonormalAxes(const T in_Z[3], const T* in_Y, T out_Axes[3][3]);

      static void calculatePrincipalAxes(const typename Mesh<T>::P mesh, T out_Axes[3][3]);
      static void calculatePrincipalAxesGivenZ(const typename Mesh<T>::P mesh, T in_Z[3], T out_Axes[3][3]);

      static void transformMesh(typename Mesh<T>::P mesh, const vnl_matrix_fixed<T,4,4> r);
      
      static void transformMesh(typename Mesh<T>::P mesh, const T in_R[3][3], const T in_t[3]);
      static void untransformMesh(typename Mesh<T>::P mesh, const T in_R[3][3], const T in_t[3]);

      static T meshToMeshDistance(typename Mesh<T>::P meshA, typename Mesh<T>::P meshB, const bool surface = true);

      /*
      static void calculateBounds(const typename Mesh<T>::P mesh, double* out_Bounds);

      static void expandBounds(const typename Mesh<T>::P mesh, double* out_Bounds);
      static void expandBounds(const double* bounds, double* out_Bounds);
      */

      static typename Tuples<T>::P calculatePrincipalCurvature(const typename Mesh<T>::P mesh);
   };
}



#endif // __MESHFUNCTIONS_H__