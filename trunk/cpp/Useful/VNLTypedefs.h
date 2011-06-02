#ifndef __VNLTYPEDEFS_H__
#define __VNLTYPEDEFS_H__

#include <vector>

#include "vnl/vnl_matrix.h"
#include "vnl/vnl_vector.h"
#include "vnl/vnl_vector_fixed.h"

namespace AWT
{
   typedef vnl_vector<double>            DoubleVector;
   typedef vnl_vector_fixed<double,4>    DoublePoint;
   typedef vnl_matrix<double>            DoubleMatrix;

   typedef std::vector<DoublePoint>      DoublePointList;
   typedef std::vector<DoublePointList*> DoublePointListList;
   typedef std::vector<DoubleVector>     DoubleVectorList;
}

#endif // VNLTYPEDEFS_H__