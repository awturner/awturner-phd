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
#include "ConnectedRegions.h"

#include "Useful/Exception.h"

using namespace AWT;
using namespace cimg_library;

typedef unsigned char SegmentationType;
typedef CImg<SegmentationType> SegmentationImage;

SegmentationType findFreeLabels(SegmentationImage& img, SegmentationType* out_Labels, SegmentationType maxLabels = 0)
{
   SegmentationType ret = 0;

   bool isFree[256];
   for (int i = 0; i < 256; ++i)
      isFree[i] = true;

   int x, y, z;
   cimg_forXYZ(img, x, y, z)
   {
      isFree[ img(x,y,z) ] = false;
   }

   for (int i = 0; i < 256; ++i)
   {
      if (isFree[i])
      {
         out_Labels[ret++] = i;

         if (maxLabels != 0 && ret >= maxLabels)
            break;
      }
   }

   return ret;
}

void AWT::findConnectedRegions(SegmentationImage& img, SegmentationImage& ret)
{
   // Create a random LUT palette
   SegmentationType mins[] = { 127, 127, 127 };
   SegmentationType maxs[] = { 255, 255, 255 };

   // Take a copy of the original image - we are going to use this
   // destructively to find the regions
   //SegmentationImage ret(img.dimx(), img.dimy(), img.dimz(), 1);
   ret.fill(0);

   SegmentationImage region(img.dimx(), img.dimy(), img.dimz(), 1);

   int x, y, z;

   SegmentationType freeLabel[1];
   if (findFreeLabels(img, freeLabel, 1) < 1)
      AWTEXCEPTIONTHROW("Need at least 1 free label!");

   SegmentationType currentLabel = 0;

   ret.fill(0);

   cimg_forXYZ(ret, x, y, z)
   {
      // If this point hasn't got a label yet...
      if (ret(x, y, z) == 0)
      {
         ++currentLabel;

         // Blank out the region image
         region.fill(false);

         // Fill the input image from the current point -
         // but do it with zero opacity so that the image
         // remains unchanged
         img.draw_fill(x, y, z, freeLabel, region, 0, 0);
         
         int xx, yy, zz;
         cimg_forXYZ(ret, xx, yy, zz)
         {
            if (region(xx, yy, zz))
               ret(xx, yy, zz) = currentLabel;
         }
      }
   }

   //return ret;
}

SegmentationImage AWT::findConnectedRegions(SegmentationImage& img)
{
   SegmentationImage ret(img.dimx(), img.dimy(), img.dimz(), 1);
   findConnectedRegions(img, ret);
   return ret;
}