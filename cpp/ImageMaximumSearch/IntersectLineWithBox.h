#ifndef __INTERSECTLINEWITHBOX_H__
#define __INTERSECTLINEWITHBOX_H__

#include "ImageMaximumTree.h"

namespace AWT
{
   double intersectLineWithPlane( double* pointOnPlane, double* planeNormal, double* pointOnLine, double* lineDirection, double* out_Point );

   void intersectLineWithBox( double* bounds, double* point, double* direction, double** out_Points, unsigned int& out_NPoints );
}

#endif __INTERSECTLINEWITHBOX_H__