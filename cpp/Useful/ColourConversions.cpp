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
#include "ColourConversions.h"

#include <algorithm>
#include <cmath>

void AWT::ColourConversions::rgbToHsv(const double* rgb, double* hsv)
{
   double r = std::max(std::min(rgb[0], 1.0), 0.0);
   double g = std::max(std::min(rgb[1], 1.0), 0.0);
   double b = std::max(std::min(rgb[2], 1.0), 0.0);

   double maxx = r;
   double minn = r;

   for (int i = 1; i < 3; ++i)
   {
      maxx = std::max(maxx, rgb[i]);
      minn = std::min(minn, rgb[i]);
   }

   if (maxx == minn)
   {
      hsv[0] = 0;
   }
   else if (maxx == r)
   {
      hsv[0] = (g - b) / (maxx - minn);

      if (g < b)
      {
         hsv[0] += 1;
      }
   }
   else if (maxx == g)
      hsv[0] = (b - r) / (maxx - minn) + 1.0/3.0;
   else
      hsv[1] = (r - g) / (maxx - minn) + 2.0/3.0;

   hsv[1] = (maxx == 0) ? 0 : (1 - minn/maxx);
   hsv[2] = maxx;

   hsv[3] = rgb[3];
}

void AWT::ColourConversions::hsvToRgb(const double* hsv, double* rgb)
{
   double h = std::max(std::min(hsv[0], 1.0), 0.0);
   double s = std::max(std::min(hsv[1], 1.0), 0.0);
   double v = std::max(std::min(hsv[2], 1.0), 0.0);

   int hi = static_cast<int>(floor(h * 6)) % 6;

   double f = h*6 - hi;
   double p = v * (1 - s);
   double q = v * (1 - f*s);
   double t = v * (1 - (1-f)*s);

   switch (hi)
   {
   case 0:
      rgb[0] = v;
      rgb[1] = t;
      rgb[2] = p;
      break;
   case 1:
      rgb[0] = q;
      rgb[1] = v;
      rgb[2] = p;
      break;
   case 2:
      rgb[0] = p;
      rgb[1] = v;
      rgb[2] = t;
      break;
   case 3:
      rgb[0] = p;
      rgb[1] = q;
      rgb[2] = v;
      break;
   case 4:
      rgb[0] = t;
      rgb[1] = p;
      rgb[2] = v;
      break;
   case 5:
      rgb[0] = v;
      rgb[1] = p;
      rgb[2] = q;
      break;
   }

   for (int i = 0; i < 3; ++i)
      rgb[i] = 0.5*rgb[i] + 0.5;

   rgb[3] = hsv[3];
}