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
#ifndef __CIMGGRADIENT_H__
#define __CIMGGRADIENT_H__

template <class PhiType, class IndexType>
PhiType grad_x(const CImg<PhiType>& phi, const IndexType& pos)
{
   if (phi.dimx() == 1)
      return 0;

   if (pos.x == 0)
   {
      return phi(pos.x+1, pos.y, pos.z) - phi(pos.x, pos.y, pos.z);
   }
   else if (pos.x == phi.dimx()-1)
   {
      return phi(pos.x, pos.y, pos.z) - phi(pos.x-1, pos.y, pos.z);
   }
   else
   {
      return (phi(pos.x+1, pos.y, pos.z) - phi(pos.x-1, pos.y, pos.z)) / 2;
   }
}

template <class PhiType, class IndexType>
PhiType grad_y(const CImg<PhiType>& phi, const IndexType& pos)
{
   if (phi.dimy() == 1)
      return 0;

   if (pos.y == 0)
   {
      return phi(pos.x, pos.y+1, pos.z) - phi(pos.x, pos.y, pos.z);
   }
   else if (pos.y == phi.dimy()-1)
   {
      return phi(pos.x, pos.y, pos.z) - phi(pos.x, pos.y-1, pos.z);
   }
   else
   {
      return (phi(pos.x, pos.y+1, pos.z) - phi(pos.x, pos.y-1, pos.z)) / 2;
   }
}

template <class PhiType, class IndexType>
PhiType grad_z(const CImg<PhiType>& phi, const IndexType& pos)
{
   if (phi.dimz() == 1)
      return 0;

   if (pos.z == 0)
   {
      return phi(pos.x, pos.y, pos.z+1) - phi(pos.x, pos.y, pos.z);
   }
   else if (pos.z == phi.dimz()-1)
   {
      return phi(pos.x, pos.y, pos.z) - phi(pos.x, pos.y, pos.z-1);
   }
   else
   {
      return (phi(pos.x, pos.y, pos.z+1) - phi(pos.x, pos.y, pos.z-1)) / 2;
   }
}

template <class PhiType, class IndexType>
PhiType grad_xx(const CImg<PhiType>& phi, const IndexType& pos)
{
   if (pos.x <= 0 || pos.x >= phi.dimx()-1)
      return 0;

   return phi(pos.x+1, pos.y, pos.z) - 2*phi(pos.x, pos.y, pos.z) + phi(pos.x-1, pos.y, pos.z);
}

template <class PhiType, class IndexType>
PhiType grad_yy(const CImg<PhiType>& phi, const IndexType& pos)
{
   if (pos.y <= 0 || pos.y >= phi.dimy()-1)
      return 0;

   return phi(pos.x, pos.y+1, pos.z) - 2*phi(pos.x, pos.y, pos.z) + phi(pos.x, pos.y-1, pos.z);
}

template <class PhiType, class IndexType>
PhiType grad_zz(const CImg<PhiType>& phi, const IndexType& pos)
{
   if (pos.z <= 0 || pos.z >= phi.dimz()-1)
      return 0;

   return phi(pos.x, pos.y, pos.z+1) - 2*phi(pos.x, pos.y, pos.z) + phi(pos.x, pos.y, pos.z-1);
}

template <class PhiType, class IndexType>
PhiType grad_xy(const CImg<PhiType>& phi, const IndexType& pos)
{
   if (phi.dimx() == 0 || phi.dimy() == 0)
      return 0;

   IndexType tmp = pos;

   if (pos.x == 0)
   {
      tmp.x += 1;

      const PhiType gyp = grad_y(phi, tmp);
      const PhiType gy0 = grad_y(phi, pos);

      return gyp - gy0;
   }
   else if (pos.x == phi.dimx()-1)
   {
      tmp.x -= 1;

      const PhiType gy0 = grad_y(phi, pos);
      const PhiType gym = grad_y(phi, tmp);

      return gy0 - gym;
   }
   else
   {
      tmp.x = pos.x + 1;
      const PhiType gyp = grad_y(phi, tmp);

      tmp.x = pos.x - 1;
      const PhiType gym = grad_y(phi, tmp);

      //PRINTVBL(gyp);
      //PRINTVBL(gym);

      return (gyp - gym) / 2;
   }
}

template <class PhiType, class IndexType>
PhiType grad_xz(const CImg<PhiType>& phi, const IndexType& pos)
{
   if (phi.dimx() == 0 || phi.dimz() == 0)
      return 0;

   IndexType tmp = pos;

   if (pos.x == 0)
   {
      tmp.x += 1;

      const PhiType gzp = grad_z(phi, tmp);
      const PhiType gz0 = grad_z(phi, pos);

      return gzp - gz0;
   }
   else if (pos.x == phi.dimx()-1)
   {
      tmp.x -= 1;

      const PhiType gz0 = grad_z(phi, pos);
      const PhiType gzm = grad_z(phi, tmp);

      return gz0 - gzm;
   }
   else
   {
      tmp.x = pos.x + 1;
      const PhiType gzp = grad_z(phi, tmp);

      tmp.x = pos.x - 1;
      const PhiType gzm = grad_z(phi, tmp);

      return (gzp - gzm) / 2;
   }
}

template <class PhiType, class IndexType>
PhiType grad_yz(const CImg<PhiType>& phi, const IndexType& pos)
{
   if (phi.dimy() == 0 || phi.dimz() == 0)
      return 0;

   IndexType tmp = pos;

   if (pos.y == 0)
   {
      tmp.y += 1;

      const PhiType gzp = grad_z(phi, tmp);
      const PhiType gz0 = grad_z(phi, pos);

      return gzp - gz0;
   }
   else if (pos.y == phi.dimx()-1)
   {
      tmp.y -= 1;

      const PhiType gz0 = grad_z(phi, pos);
      const PhiType gzm = grad_z(phi, tmp);

      return gz0 - gzm;
   }
   else
   {
      tmp.y = pos.y + 1;
      const PhiType gzp = grad_z(phi, tmp);

      tmp.y = pos.y - 1;
      const PhiType gzm = grad_z(phi, tmp);

      return (gzp - gzm) / 2;
   }
}

template <class PhiType, class IndexType>
PhiType meancurvature(const CImg<PhiType>& phi, const IndexType& pos)
{
   const PhiType gx = grad_x(phi, pos);
   const PhiType gy = grad_y(phi, pos);
   const PhiType gz = grad_z(phi, pos);

   const PhiType gxx = grad_xx(phi, pos);
   const PhiType gyy = grad_yy(phi, pos);
   const PhiType gzz = grad_zz(phi, pos);

   const PhiType gxy = grad_xy(phi, pos);
   const PhiType gxz = grad_xz(phi, pos);
   const PhiType gyz = grad_yz(phi, pos);

   const PhiType num = (gy*gy + gz*gz) * gxx + (gx*gx + gz*gz) * gyy + (gx*gx + gy*gy) * gzz
      - 2*(gx*gy*gxy + gx*gz*gxz + gy*gz*gyz);

   return num;

   const PhiType den = pow(gx*gx + gy*gy + gz*gz, static_cast<PhiType>(1.5));

   return num / den;
}

#endif // __CIMGGRADIENT_H__