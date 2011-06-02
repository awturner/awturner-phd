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
#ifndef __DERIVATIVES_H__
#define __DERIVATIVES_H__

#include "CImg.h"
#include "Exception.h"

template <class T>
T delta_x(const cimg_library::CImg<T>& img, const int x, const int y, const int z, const T spacing[3])
{
   if (!img.contains(x, y, z))
      AWTEXCEPTIONTHROW("Point outside image!");

   if (img.dimx() == 1)
      return 0;

   T ret;

   if (x == 0)
      ret = img(x+1, y, z) - img(x, y, z);
   else if (x == img.dimx()-1)
      ret = img(x, y, z) - img(x-1, y, z);
   else
      ret = (img(x+1, y, z) - img(x-1, y, z)) / 2;

   return ret / spacing[0];
}

template <class T>
T delta_y(const cimg_library::CImg<T>& img, const int x, const int y, const int z, const T spacing[3])
{
   if (!img.contains(x, y, z))
      AWTEXCEPTIONTHROW("Point outside image!");

   if (img.dimy() == 1)
      return 0;

   T ret;

   if (y == 0)
      ret = img(x, y+1, z) - img(x, y, z);
   else if (y == img.dimy()-1)
      ret = img(x, y, z) - img(x, y-1, z);
   else
      ret = (img(x, y+1, z) - img(x, y-1, z)) / 2;
   
   return ret / spacing[1];
}

template <class T>
T delta_z(const cimg_library::CImg<T>& img, const int x, const int y, const int z, const T spacing[3])
{
   if (!img.contains(x, y, z))
      AWTEXCEPTIONTHROW("Point outside image!");

   if (img.dimz() == 1)
      return 0;

   T ret;

   if (z == 0)
      ret = img(x, y, z+1) - img(x, y, z);
   else if (z == img.dimz()-1)
      ret = img(x, y, z) - img(x, y, z-1);
   else
      ret = (img(x, y, z+1) - img(x, y, z-1)) / 2;
   
   return ret / spacing[2];
}

template <class T>
T delta_xx(const cimg_library::CImg<T>& img, const int x, const int y, const int z, const T spacing[3])
{
   if (!img.contains(x, y, z))
      AWTEXCEPTIONTHROW("Point outside image!");

   if (img.dimx() == 1 || x == 0 || x == img.dimx()-1)
      return 0;

   return (img(x+1,y,z) - 2*img(x,y,z) + img(x-1,y,z)) / (spacing[0]*spacing[0]);
}

template <class T>
T delta_yy(const cimg_library::CImg<T>& img, const int x, const int y, const int z, const T spacing[3])
{
   if (!img.contains(x, y, z))
      AWTEXCEPTIONTHROW("Point outside image!");

   if (img.dimy() == 1 || y == 0 || y == img.dimy()-1)
      return 0;

   return (img(x,y+1,z) - 2*img(x,y,z) + img(x,y-1,z)) / (spacing[1]*spacing[1]);
}

template <class T>
T delta_zz(const cimg_library::CImg<T>& img, const int x, const int y, const int z, const T spacing[3])
{
   if (!img.contains(x, y, z))
      AWTEXCEPTIONTHROW("Point outside image!");

   if (img.dimz() == 1 || z == 0 || z == img.dimz()-1)
      return 0;

   return (img(x,y,z+1) - 2*img(x,y,z) + img(x,y,z-1)) / (spacing[2]*spacing[2]);
}

template <class T>
T delta_xy(const cimg_library::CImg<T>& img, const int x, const int y, const int z, const T spacing[3])
{
   if (!img.contains(x, y, z))
      AWTEXCEPTIONTHROW("Point outside image!");

   if (img.dimx() == 1 || x == 0 || x == img.dimx()-1 || img.dimy() == 1 || y == 0 || y == img.dimy()-1)
      return 0;

   return (img(x+1,y+1,z) - img(x-1,y+1,z) - img(x+1,y-1,z) + img(x-1,y-1,z)) / (4 * spacing[0] * spacing[1]);
}

template <class T>
T delta_xz(const cimg_library::CImg<T>& img, const int x, const int y, const int z, const T spacing[3])
{
   if (!img.contains(x, y, z))
      AWTEXCEPTIONTHROW("Point outside image!");

   if (img.dimx() == 1 || x == 0 || x == img.dimx()-1 || img.dimz() == 1 || z == 0 || z == img.dimz()-1)
      return 0;

   return (img(x+1,y,z+1) - img(x-1,y,z+1) - img(x+1,y,z-1) + img(x-1,y,z-1)) / (4 * spacing[0] * spacing[2]);
}

template <class T>
T delta_yz(const cimg_library::CImg<T>& img, const int x, const int y, const int z, const T spacing[3])
{
   if (!img.contains(x, y, z))
      AWTEXCEPTIONTHROW("Point outside image!");

   if (img.dimy() == 1 || y == 0 || y == img.dimy()-1 || img.dimz() == 1 || z == 0 || z == img.dimz()-1)
      return 0;

   return (img(x,y+1,z+1) - img(x,y-1,z+1) - img(x,y+1,z-1) + img(x,y-1,z-1)) / (4 * spacing[1] * spacing[2]);
}

template <class T>
T imageCurvature(const cimg_library::CImg<T>& img, const int x, const int y, const int z, const T spacing[3])
{
   // First order derivatives
   T dx = delta_x(img, x, y, z, spacing);
   T dy = delta_y(img, x, y, z, spacing);
   T dz = delta_z(img, x, y, z, spacing);

   // Second order derivatives
   T dxx = delta_xx(img, x, y, z, spacing);
   T dxy = delta_xy(img, x, y, z, spacing);
   T dxz = delta_xz(img, x, y, z, spacing);
   T dyx = dxy;
   T dyy = delta_yy(img, x, y, z, spacing);
   T dyz = delta_yz(img, x, y, z, spacing);
   T dzx = dxz;
   T dzy = dyz;
   T dzz = delta_zz(img, x, y, z, spacing);

   T den = dx*dx + dy*dy + dz*dz;
   if (den == 0)
      return 0;

   T num = (dy*dy + dz*dz) * dxx + (dx*dx + dz*dz) * dyy + (dx*dx + dy*dy) * dzz - 2*(dx*dy*dxy + dx*dz*dxz + dy*dz*dyz);
   
   return num / pow(den, static_cast<T>(1.5));
}

#endif // __DERIVATIVES_H__