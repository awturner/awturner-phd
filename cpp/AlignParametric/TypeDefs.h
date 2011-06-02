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
#ifndef __TYPEDEFS_H__
#define __TYPEDEFS_H__

#include "vnl/vnl_vector.h"
#include "vnl/vnl_matrix.h"
#include "vnl/vnl_matrix_fixed.h"
#include "vnl/vnl_sparse_matrix.h"
#include "Mesh/Mesh.h"
#include "Mesh/VNLTuples.h"

#include <vector>
#include <string>
#include <limits>

namespace AWT
{
   namespace AlignParametric
   {
      typedef double               T;
      typedef vnl_matrix<T>        MatrixType;
      typedef vnl_vector<T>        VectorType;

      typedef vnl_matrix_fixed<T,4,4> Transformation;

      typedef unsigned int         Idx;

      typedef Mesh<T>              MeshType;
      typedef Tuples<MeshIndex>    FaceType;
      typedef Tuples<T>            TuplesType;
      typedef VNLTuples<T>         VNLTuplesType;

      const Idx INVALID_INDEX = std::numeric_limits<Idx>::max();
   }
}

#define TYPED_DIV( Ty, x, y ) ( static_cast<Ty>(x) / static_cast<Ty>(y) )

#define ENSURESIZE

#endif // __TYPEDEFS_H__