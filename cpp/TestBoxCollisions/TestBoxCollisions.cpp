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
#include "ObjectEKDTree/AxisAlignedBoundingBox.h"
#include "Useful/PrintMacros.h"
#include "Useful/ArrayVectorFunctions.h"
#include "Mesh/Pose.h"
#include "MeshSearching/BoxQueries.h"
#include "Useful/WriteToClipboard.h"

#include <sstream>

using namespace AWT;

typedef double T;

int main(int argc, char** argv)
{
   AxisAlignedBoundingBoxImpl<T,3>::P boxA = AxisAlignedBoundingBoxImpl<T,3>::getInstance();
   Pose<T>::P poseA = Pose<T>::getInstance();

   AxisAlignedBoundingBoxImpl<T,3>::P boxB = AxisAlignedBoundingBoxImpl<T,3>::getInstance();
   Pose<T>::P poseB = Pose<T>::getInstance();

   T a[] = { 1, 2, 10 };
   T b[] = { 1, 1, 1 };
   T da[] = { -0.5, 0, 0 };
   T db[] = { 2.5, 0, 0 };

   for (int ax = 0; ax < 3; ++ax)
   {
      boxA->setMinimumBound(ax, 0 - a[ax] + da[ax]);
      boxA->setMaximumBound(ax, 0 + a[ax] + da[ax]);

      boxB->setMinimumBound(ax, 0 - b[ax] + db[ax]);
      boxB->setMaximumBound(ax, 0 + b[ax] + db[ax]);
   }

   // Use bisection to find the place where they just touch
   T thetaLow = 0;
   T thetaHigh = 90;

   T axis[]  = { 0, 0, 1 };
   T axis2[] = { 0, 1, 0 };
   T axis2Angle = 20;
   T thetaMid = (thetaHigh + thetaLow) / 2;

   poseB->rotateDeg(45, axis);

   bool intersect;

   while (thetaHigh - thetaLow > 1e-8)
   {
      thetaMid = (thetaHigh + thetaLow) / 2;

      poseA->identity();
      poseA->rotateDeg(axis2Angle, axis2);
      poseA->rotateDeg(thetaMid, axis);

      intersect = BoxQueries<T>::boxesIntersect(boxA, poseA, boxB, poseB);

      if (intersect)
         thetaHigh = thetaMid;
      else
         thetaLow = thetaMid;
   }

   PRINTVBL(thetaMid);

   while (true)
   {
      std::cerr << "Enter a theta [" << thetaMid << ", -ve to break]: ";
      T tmpTheta;

      std::cin >> tmpTheta;
      if (tmpTheta < 0)
         break;

      thetaMid = tmpTheta;

      poseA->identity(); poseA->rotateDeg(thetaMid, axis);
      bool intersect = BoxQueries<T>::boxesIntersect(boxA, poseA, boxB, poseB, true);

      PRINTVBL2(thetaMid, intersect ? "true" : "false");
   }

   poseA->identity(); 
   poseA->rotateDeg(axis2Angle, axis2);
   poseA->rotateDeg(thetaMid, axis);

   BoxQueries<T>::boxesIntersect(boxA, poseA, 
      boxB, poseB, true);

   Matrix<T> vertsProjA(3, 8);
   Matrix<T> vertsProjB(3, 8);

   T vtxOrigA[3];
   T vtxProjA[3];

   T vtxOrigB[3];
   T vtxProjB[3];

   for (int v = 0; v < 8; ++v)
   {
      for (int ax = 0; ax < 3; ++ax)
      {
         vtxOrigA[ax] = ((v >> ax) % 2 == 0) ? boxA->getMaximumBound(ax) : boxA->getMinimumBound(ax);
         vtxOrigB[ax] = ((v >> ax) % 2 == 0) ? boxB->getMaximumBound(ax) : boxB->getMinimumBound(ax);
      }

      poseA->project(vtxOrigA, vtxProjA);
      poseB->project(vtxOrigB, vtxProjB);

      for (int ax = 0; ax < 3; ++ax)
      {
         vertsProjA[ax][v] = vtxProjA[ax];
         vertsProjB[ax][v] = vtxProjB[ax];
      }
   }

   std::stringstream sstream;

   sstream << "clear all; close all; " << std::endl;
   PRINTVBL2STREAM(sstream, intersect, intersect ? "1" : "0");
   PRINTVBL2STREAM(sstream, vertsProjA, transpose(vertsProjA));
   PRINTVBL2STREAM(sstream, vertsProjB, transpose(vertsProjB));

   Matrix<T> matA = poseA->getMatrix();
   Matrix<T> matB = poseB->getMatrix();

   T A0dotD = matA[0][0]*db[0] + matA[1][0]*db[1] + matA[2][0]*db[2];
   T A1dotD = matA[0][1]*db[0] + matA[1][1]*db[1] + matA[2][1]*db[2];
   T A2dotD = matA[0][2]*db[0] + matA[1][2]*db[1] + matA[2][2]*db[2];
   
   sstream << "drawCube(vertsProjA', 'r'); hold on; drawCube(vertsProjB', 'b'); " << std::endl;
   sstream << "axis equal;" << std::endl;
   
   WriteToClipboard::copyToClipboard(sstream.str());

}