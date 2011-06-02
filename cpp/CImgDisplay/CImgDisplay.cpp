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
#include "CImg.h"

#include <iostream>
#include <fstream>
#include <vector>

using namespace cimg_library;

typedef std::vector<unsigned char> RiskMapping;
typedef CImg<unsigned char> Image;

RiskMapping loadRiskLabels(char* filename)
{
   RiskMapping ret;

   ret.push_back(0);

   std::ifstream is(filename);

   int segment;
   int label;

   while (true)
   {
      is >> segment >> label;
      if (!is.good())
         break;

      ret.push_back(label);
   }

   return ret;
}

unsigned char cols[7][3] = {
   { 0, 0, 0 },
   { 0, 176, 240 },
   { 120, 120, 222 },
   { 146, 208, 80 },
   { 255, 255, 0 },
   { 254, 170, 2 },
   { 255, 0, 0 },
};

void paintOutputImage(Image& labelImage, Image& outputImage, const RiskMapping& mapping)
{
   cimg_forXY(outputImage, x, y)
   {
      int label = labelImage(x,y);
      int mapped = label; //mapping[label];

      cimg_forV(outputImage, v)
         outputImage(x, y, 0, v) = cols[ mapped ][v];

   }
}

void prepareLabelImage(const Image& labelImage, const RiskMapping& riskMapping, Image& preparedImage)
{
   // Fill the prepared image with zeros
   preparedImage.fill(0);

   // Create a temporary image which will be dilated
   Image tempImage(labelImage);

   // Create the structuring element to dilate by
   Image structEl(3, 3, 3);
   structEl.fill(1);

   CImgList<> allPoints;
   CImgList<> allPrimitives;

   CImgList<unsigned char> allColors;
   CImgList<float>         allOpacities;

   for (int label = 6; label > 0; --label)
   {
      std::cerr << "Doing label " << label << std::endl;
      tempImage.fill(0);

      cimg_forXYZ(labelImage, x, y, z)
      {
         if (riskMapping[labelImage(x,y,z)] == label)
            tempImage(x,y,z) = label;
      }

      // Now dilate the tempImage
      tempImage.dilate(structEl);

      cimg_forXYZ(tempImage, x, y, z)
      {
         if (tempImage(x, y, z) > preparedImage(x, y, z))
         {
            preparedImage(x, y, z) = tempImage(x, y, z);
         }
         else
         {
            tempImage(x, y, z) = 0;
         }
      }

      // Now do the marching cubes in order to extract the surface
      CImgList<> points;
      CImgList<> primitives;

      std::cerr << "Marching cubes..." << std::endl;
      tempImage.marching_cubes((label+0.f)/2.f, 1.f/0.9375f, 1.f/0.9375f, 1.f/1.2f, points, primitives);
      std::cerr << "Done." << std::endl;

      CImgList<unsigned char> colors(points.size,3,1,1,1,255);
      CImgList<float>         opacities(points.size,1,1,1,1,0.5f);

      cimglist_for(colors, c)
      {
         colors(c, 0) = cols[label][0];
         colors(c, 1) = cols[label][1];
         colors(c, 2) = cols[label][2];
      }

      std::cerr << "#points = " << points.size << "; #primitives = " << primitives.size << std::endl;

      // Offset the primitive indices
      cimglist_for(primitives, p)
      {
         primitives(p, 0, 0) += allPoints.size;
         primitives(p, 0, 1) += allPoints.size;
         primitives(p, 0, 2) += allPoints.size;
      }

      allPoints.insert(points, allPoints.size);
      allPrimitives.insert(primitives, allPrimitives.size);
      allColors.insert(colors, allColors.size);
      allOpacities.insert(opacities, allOpacities.size);
   }

   std::cerr << "All points size = " << allPoints.size << std::endl;

   CImgDisplay disp3d(tempImage);
   disp3d.display_object3d(allPoints, allPrimitives, allColors, true, 4, 1, true);

   while (!disp3d.is_closed)
      cimg::wait(20);
}

int main(int argc, char** argv)
{
   Image image;
   image.load(argv[1]);

   std::cerr << "Loaded image from " << argv[1] << std::endl;
   std::cerr << "Dimensions: " << image.dimx() << " x " << image.dimy() << " x " << image.dimz() << std::endl;

   RiskMapping riskLabels = loadRiskLabels(argv[2]);
   std::cerr << "Loaded risk labels from from " << argv[1] << std::endl;

   Image mappedImage(image);

   std::cerr << "Mapping image" << std::endl;
   prepareLabelImage(image, riskLabels, mappedImage);
   std::cerr << "Created mapped image" << std::endl;

   Image outputImage(image.dimx(), image.dimy(), 1, 3);

   int slice = 0;

   CImgDisplay disp(outputImage, argv[1]);

   slice = 20;

   // Force the update
   --slice;
   disp.wheel = 1;

   while (!disp.is_closed)
   {
      if (disp.wheel != 0)
      {
         slice += disp.wheel;

         if (slice < 0) slice = 0;
         if (slice >= image.dimz()) slice = image.dimz() - 1;

         std::cerr << "slice = " << slice << std::endl;

         disp.wheel = 0;
         disp.flush();

         paintOutputImage(mappedImage.get_shared_plane(slice), outputImage, riskLabels);

         disp.display(outputImage);
      }
      cimg::wait(20);
   }
}