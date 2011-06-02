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
#ifndef __IMAGEMAXIMUMTREE_H__
#define __IMAGEMAXIMUMTREE_H__

#include "CImg.h"
#include <limits>

namespace AWT
{
   template <class T>
   class ImageMaximumTree
   {
   public:
      static ImageMaximumTree<T>* buildTree(cimg_library::CImg<T>& im);
      ~ImageMaximumTree();

      T getValue() const;

      unsigned int getAxis() const;
      double getMinimumBound(unsigned char ax) const;
      double getMaximumBound(unsigned char ax) const;

      unsigned int getNumberOfChildren() const;
      ImageMaximumTree<T>* getChild(unsigned char ax) const;

   protected:
      static ImageMaximumTree<T>* buildTreeRecursive(cimg_library::CImg<T>& im, int xfrom, int xto, int yfrom, int yto, int zfrom, int zto, int ax, int depth = 0 );
      ImageMaximumTree(int xfrom, int xto, int yfrom, int yto, int zfrom, int zto, int ax);

      T m_Value;
      ImageMaximumTree<T>* m_Children[2];
      int m_Bounds[6];
      int m_Axis;
   };
}

template <class T>
AWT::ImageMaximumTree<T>* AWT::ImageMaximumTree<T>::buildTree(cimg_library::CImg<T>& im)
{
   return buildTreeRecursive(im, 0, im.dimx(), 0, im.dimy(), 0, im.dimz(), 0);
}

#include <iostream>

template <class T>
AWT::ImageMaximumTree<T>* AWT::ImageMaximumTree<T>::buildTreeRecursive(cimg_library::CImg<T>& im, int xfrom, int xto, int yfrom, int yto, int zfrom, int zto, int ax, int depth )
{
   /*
   for (int i = 0; i < depth; ++i)
      std::cerr << " ";
   std::cerr << "> " << xfrom << "," << yfrom << "," << zfrom << " -> " << xto << "," << yto << "," << zto << " (" << ax << ")" << std::endl;
   */

   AWT::ImageMaximumTree<T>* ret = new ImageMaximumTree(xfrom, xto, yfrom, yto, zfrom, zto, ax);

   if (xto==xfrom || yto==yfrom || zto==zfrom)
   {
      ret->m_Value = -std::numeric_limits<T>::max();
      return ret;
   }
   else if (xto-xfrom == 1 && yto-yfrom == 1 && zto-zfrom == 1)
   {
      ret->m_Value = im(xfrom, yfrom, zfrom);
      return ret;
   }
   else
   {
      int half;
      switch (ax)
      {
      case 0:
         half = (xfrom+xto) / 2;
         ret->m_Children[0] = buildTreeRecursive(im, xfrom, half, yfrom, yto, zfrom, zto, 1, depth+1);
         ret->m_Children[1] = buildTreeRecursive(im, half,  xto,  yfrom, yto, zfrom, zto, 1, depth+1);
         break;
      case 1:
         half = (yfrom+yto) / 2;
         ret->m_Children[0] = buildTreeRecursive(im, xfrom, xto, yfrom, half, zfrom, zto, 2, depth+1);
         ret->m_Children[1] = buildTreeRecursive(im, xfrom, xto, half, yto, zfrom, zto, 2, depth+1);
         break;
      case 2:
         half = (zfrom+zto) / 2;
         ret->m_Children[0] = buildTreeRecursive(im, xfrom, xto, yfrom, yto, zfrom, half, 0, depth+1);
         ret->m_Children[1] = buildTreeRecursive(im, xfrom, xto, yfrom, yto, half, zto, 0, depth+1);
         break;
      }
   }

   // Check for NaNs
   if (ret->m_Children[0]->m_Value != ret->m_Children[0]->m_Value)
      ret->m_Value = ret->m_Children[1]->m_Value;
   else if (ret->m_Children[1]->m_Value != ret->m_Children[1]->m_Value)
      ret->m_Value = ret->m_Children[0]->m_Value;
   else
      ret->m_Value = (ret->m_Children[0]->m_Value > ret->m_Children[1]->m_Value)
         ? ret->m_Children[0]->m_Value
         : ret->m_Children[1]->m_Value;

   /*
   for (int i = 0; i < depth; ++i)
      std::cerr << " ";
   std::cerr << "< " << xfrom << "," << yfrom << "," << zfrom << " -> " << xto << "," << yto << "," << zto << " (" << ax << ")" << std::endl;
   */

   /*
   // Check the invariant
   for (int c = 0; c < 2; ++c)
   {
      AWT::ImageMaximumTree<T>* child = ret->m_Children[c];

      T maxx = -std::numeric_limits<T>::max();

      for (int z = child->m_Bounds[4]; z < child->m_Bounds[5]; ++z)
         for (int y = child->m_Bounds[2]; y < child->m_Bounds[3]; ++y)
            for (int x = child->m_Bounds[0]; x < child->m_Bounds[1]; ++x)
            {
               if (im(x, y, z) > maxx)
                  maxx = im(x, y, z);
            }

      if (child->m_Value != maxx)
         std::cerr << "Invariant Error: " << (1*child->m_Value) << " " << (1*maxx) << std::endl;
   }
   */

   return ret;
}

template <class T>
AWT::ImageMaximumTree<T>::ImageMaximumTree(int xfrom, int xto, int yfrom, int yto, int zfrom, int zto, int ax)
{
   m_Children[0] = 0;
   m_Children[1] = 0;

   m_Bounds[0] = xfrom;
   m_Bounds[1] = xto;
   m_Bounds[2] = yfrom;
   m_Bounds[3] = yto;
   m_Bounds[4] = zfrom;
   m_Bounds[5] = zto;

   m_Axis = ax;
}

template <class T>
AWT::ImageMaximumTree<T>::~ImageMaximumTree()
{
   if (m_Children[0] != 0) delete m_Children[0];
   if (m_Children[1] != 0) delete m_Children[1];
}

template <class T>
T AWT::ImageMaximumTree<T>::getValue() const
{
   return m_Value;
}

template <class T>
unsigned int AWT::ImageMaximumTree<T>::getAxis() const
{
   return m_Axis;
}

template <class T>
double AWT::ImageMaximumTree<T>::getMinimumBound(unsigned char ax) const
{
   return m_Bounds[2*ax + 0] - 0.5;
}

template <class T>
double AWT::ImageMaximumTree<T>::getMaximumBound(unsigned char ax) const
{
   return m_Bounds[2*ax + 1] - 0.5;
}

template <class T>
unsigned int AWT::ImageMaximumTree<T>::getNumberOfChildren() const
{
   return 2;
}

template <class T>
AWT::ImageMaximumTree<T>* AWT::ImageMaximumTree<T>::getChild(unsigned char ax) const
{
   return m_Children[ax];
}

#endif // __IMAGEMAXIMUMTREE_H__