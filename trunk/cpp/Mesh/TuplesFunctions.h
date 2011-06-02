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
#ifndef __TUPLESFUNCTIONS_H__
#define __TUPLESFUNCTIONS_H__

#include "ObjectManagement/ManagedObject.h"

#include "Tuples.h"

#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_matrix.h>

#include <ostream>

namespace AWT
{
   template <class T>
   class TuplesFunctions : public virtual ManagedObject
   {
   public:
      typedef ManagedAutoPointer<TuplesFunctions<T> > P;

   protected:
      TuplesFunctions();
      virtual ~TuplesFunctions();

   public:
      static void calculateBounds(const typename Tuples<T>::P tuples, double* out_Bounds);
      static void expandBounds(const typename Tuples<T>::P mesh, double* out_Bounds);
      static void expandBounds(const double* bounds, double* out_Bounds);

      static void transformPoints(typename Tuples<T>::P pts, const vnl_matrix_fixed<T,4,4> r, const bool asDirections = false);
      static void transformPoints(vnl_vector<T>& pts, const vnl_matrix_fixed<T,4,4> r, const bool asDirections = false);

      static typename Tuples<T>::P transform(typename Tuples<T>::P pts, const vnl_matrix<T> R);

      static T calculateRss(const typename Tuples<T>::P ptsX, const typename Tuples<T>::P ptsY);
      static T calculateRms(const typename Tuples<T>::P ptsX, const typename Tuples<T>::P ptsY);

      static T calculateRmsOrtho(const typename Tuples<T>::P ptsX, const typename Tuples<T>::P ptsY, const typename Tuples<T>::P normals, 
                                  const T sigmaParallel, const T sigmaPerp,
                                  T& maxParallel, T& maxPerp);

      static T calculateRssOrtho(const typename Tuples<T>::P ptsX, const typename Tuples<T>::P ptsY, const typename Tuples<T>::P normals, 
                                  const T sigmaParallel, const T sigmaPerp,
                                  T& maxParallel, T& maxPerp);

      static vnl_matrix_fixed<T,4,4> absoluteOrientation(const vnl_vector<T> ptsX, const typename vnl_vector<T> ptsY, const bool scale = false);
      static vnl_matrix_fixed<T,4,4> absoluteOrientation(const typename Tuples<T>::P ptsX, const typename Tuples<T>::P ptsY, const bool scale = false);
      
      static vnl_matrix_fixed<T,4,4> rotationTranslationAboutPoint(const T axis[3], const T theta, const T centre[3], const T translate[3]);

      static void fill(typename Tuples<T>::P pts, const T scalar);

      static void copyToVNLVector(const typename Tuples<T>::P pts, vnl_vector<T>& mat, const MeshIndex element = 0);
      static void copyToVNLMatrix(const typename Tuples<T>::P pts, vnl_matrix<T>& mat, const bool transpose = false);

   protected:
      static vnl_matrix_fixed<T,4,4> doAbsoluteOrientation(const vnl_matrix_fixed<T,3,3>& sum_xy, const vnl_vector<T>& sum_x, const vnl_vector<T>& sum_y, 
         const T xtx, const T yty, const MeshIndex N, const T doScale);
   };
}


#endif // __TUPLESFUNCTIONS_H__