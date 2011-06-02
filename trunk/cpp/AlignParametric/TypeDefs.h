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