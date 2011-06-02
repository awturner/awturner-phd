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
#ifndef __OCTREEIMAGE_H__
#define __OCTREEIMAGE_H__

#include "ObjectManagement/ManagedObject.h"
#include "Useful/Exception.h"

#include "ImageVisitor.h"

namespace AWT
{
   template <class T>
   class OctreeImage;

   template <class T>
   class Octree
   {
      friend class OctreeImage<T>;

   public:
      Octree(const T value);

      ~Octree();

      void createChildren();

      void destroyChildren();

      T minValue;
      T maxValue;

      static const unsigned int NCHILDREN = 8;
      Octree* children[NCHILDREN];

      static unsigned int instanceCount;
   };

   template <class T>
   class OctreeImage : public ManagedObject
   {
   public:
      typedef ManagedAutoPointer<OctreeImage> P;

   protected:
      OctreeImage(const unsigned int w, const unsigned int h, const unsigned int d);
      ~OctreeImage();

   public:
      static P getInstance(const unsigned int w, const unsigned int h, const unsigned int d);
      std::string getClassName() const;

      void setValue(const unsigned int x, const unsigned int y, const unsigned int z, const T value);
      T getValue(const unsigned int x, const unsigned int y, const unsigned int z) const;

      void fill(const T value);

      unsigned int getWidth() const;
      unsigned int getHeight() const;
      unsigned int getDepth() const;

      void visit(const T lower, const T upper, typename ImageVisitor<T>::P visitor);

      friend std::ostream& operator<<(std::ostream& os, const typename OctreeImage<T>::P& im)
      {
         for (unsigned int z = 0; z < im->getDepth(); ++z)
         {
            if (im->getDepth() != 1)
               os << "z = " << z << ":" << std::endl;

            for (unsigned int y = 0; y < im->getHeight(); ++y)
            {
               for (unsigned int x = 0; x < im->getWidth(); ++x)
               {
                  os << im->getValue(x, y, z) << "  ";
               }
               os << std::endl;
            }

            if (im->getDepth() != 1)
               os << std::endl;
         }

         return os;
      }

   protected:
      struct D;
      D* m_D;

   };
}

template <class T>
struct AWT::OctreeImage<T>::D
{
   D()
   {
      m_Root = 0;
      fill(0);
   }

   ~D()
   {
      delete m_Root;
   }

   // To fill the image, we can get rid of the entire tree
   // and replace it with an unbranched root with
   void fill(T value)
   {
      if (m_Root != 0)
         delete m_Root;

      m_Root = new Octree<T>(value);
   }

   void visit(const T lower, const T upper, typename AWT::ImageVisitor<T>::P visitor)
   {
      visitRecursive(lower, upper, visitor, m_Root, 0, 0, 0, m_Width, m_Height, m_Depth);
   }

   bool visitRecursive(const T lower, const T upper, typename AWT::ImageVisitor<T>::P visitor,
      Octree<T>* branch, const unsigned int x0, const unsigned int y0, const unsigned int z0, 
      const unsigned int x1, const unsigned int y1, const unsigned int z1)
   {
      bool keepGoing = true;

      if (lower <= branch->maxValue && upper >= branch->minValue)
      {
         // This means that there are points to be found...

         if (branch->children[0] != 0)
         {
            // We need to descend into all branches
            const unsigned int xs[] = { x0, (x0 + x1) / 2, x1 };
            const unsigned int ys[] = { y0, (y0 + y1) / 2, y1 };
            const unsigned int zs[] = { z0, (z0 + z1) / 2, z1 };

            for (unsigned int zi = 0; zi < 2 && keepGoing; ++zi)
               for (unsigned int yi = 0; yi < 2 && keepGoing; ++yi)
                  for (unsigned int xi = 0; xi < 2 && keepGoing; ++xi)
                  {
                     // Checking here should be more efficient, since things haven't had to be pushed on
                     // and off the stack
                     if (lower <= branch->children[ 4*zi + 2*yi + xi ]->maxValue && upper >= branch->children[ 4*zi + 2*yi + xi ]->minValue)
                     {
                        //DEBUGMACRO("Descending into ("<<xs[xi+0]<<","<<ys[yi+0]<<","<<zs[zi+0]<<") -> ("<<xs[xi+1]<<","<<ys[yi+1]<<","<<zs[zi+1]<<")");
                        keepGoing = visitRecursive(lower, upper, visitor, branch->children[ 4*zi + 2*yi + xi ], xs[xi+0], ys[yi+0], zs[zi+0], xs[xi+1], ys[yi+1], zs[zi+1]);
                     }
                     else
                     {
                        //DEBUGMACRO("Avoiding ("<<xs[xi+0]<<","<<ys[yi+0]<<","<<zs[zi+0]<<") -> ("<<xs[xi+1]<<","<<ys[yi+1]<<","<<zs[zi+1]<<")");
                     }
                  }
         }
         else
         {
            // We can't descend, this branch is "flat" valued, so we just visit everything in the region
            for (unsigned int z = z0; z < z1 && keepGoing; ++z)
               for (unsigned int y = y0; y < y1 && keepGoing; ++y)
                  for (unsigned int x = x0; x < x1 && keepGoing; ++x)
                     keepGoing = visitor->visit(x, y, z, branch->minValue);
         }
      }
      else
      {
         //DEBUGMACRO("Skipping ("<<x0<<","<<y0<<") -> ("<<x1<<","<<y1<<")");
      }

      return keepGoing;
   }

   T getValue(const unsigned int x, const unsigned int y, const unsigned int z)
   {
      // Check to make sure it is inside the image
      if (x >= m_Width || y >= m_Height || z >= m_Depth)
         AWTEXCEPTIONTHROW("Requested pixel is outside the image!");

      return getValueRecursive(x, y, z, m_Root, 0, 0, 0, m_Width, m_Height, m_Depth);
   }

   void setValue(const unsigned int x, const unsigned int y, const unsigned int z, const T value)
   {
      // Check to make sure it is inside the image
      if (x >= m_Width || y >= m_Height || z >= m_Depth)
         AWTEXCEPTIONTHROW("Requested pixel is outside the image!");

      setValueRecursive(x, y, z, value, m_Root, 0, 0, 0, m_Width, m_Height, m_Depth);
   }

   T getValueRecursive(const unsigned int x, const unsigned int y, const unsigned int z, Octree<T>* branch, 
      const unsigned int x0, const unsigned int y0, const unsigned int z0,
      const unsigned int x1, const unsigned int y1, const unsigned int z1 )
   {
      // Check to see if we need to descend further
      // If the min and maxes are the same, we don't
      if (branch->minValue == branch->maxValue)
         return branch->minValue;

      // Now, we need to work out which of the branches to descend into
      const unsigned int xs[] = { x0, (x0 + x1) / 2, x1 };
      const unsigned int ys[] = { y0, (y0 + y1) / 2, y1 };
      const unsigned int zs[] = { z0, (z0 + z1) / 2, z1 };

      // Assumed child ordering: 
      // [ 0 1 ] === [ TL TR ]
      // [ 2 3 ]     [ BL BR ]
      const unsigned int xi = (x < xs[1]) ? 0 : 1;
      const unsigned int yi = (y < ys[1]) ? 0 : 1;
      const unsigned int zi = (z < zs[1]) ? 0 : 1;

      return getValueRecursive(x, y, z, branch->children[xi + 2*yi + 4*zi], xs[0+xi], ys[0+yi], zs[0+zi], xs[1+xi], ys[1+yi], zs[1+zi]);
   }

   void setValueRecursive(const unsigned int x, const unsigned int y, const unsigned int z,
      const T value, Octree<T>* branch, 
      const unsigned int x0, const unsigned int y0, const unsigned int z0,
      const unsigned int x1, const unsigned int y1, const unsigned int z1)
   {
      // Check to see if we need to descend further
      // If the min, max and new value are the same, we don't need to do anything
      if (branch->minValue == branch->maxValue && branch->minValue == value)
         return;

      // Right.  So.  Now I need to know whether to descend further or not
      // We need to descend until we get down to a single pixel
      if (x1-x0 == 1 && y1-y0 == 1 && z1-z0 == 1)
      {
         // This is a single pixel, there is no need to descend any further
         // Set the min and max values to the new value
         branch->minValue = branch->maxValue = value;
      }
      else
      {
         // This is still not a single pixel, so we need to decide which subtree to descend into

         // In fact, we first need to make sure that the subtrees exist
         if (branch->children[0] == 0)
            branch->createChildren();

         // Now, we need to work out which of the branches to descend into
         // Find the mid-point of the box
         const unsigned int xs[] = { x0, (x0 + x1) / 2, x1 };
         const unsigned int ys[] = { y0, (y0 + y1) / 2, y1 };
         const unsigned int zs[] = { z0, (z0 + z1) / 2, z1 };

         // Assumed child ordering: 
         // [ 0 1 ] === [ TL TR ]
         // [ 2 3 ]     [ BL BR ]
         const unsigned int xi = (x < xs[1]) ? 0 : 1;
         const unsigned int yi = (y < ys[1]) ? 0 : 1;
         const unsigned int zi = (z < zs[1]) ? 0 : 1;

         setValueRecursive(x, y, z, value, branch->children[xi + 2*yi + 4*zi], xs[0+xi], ys[0+yi], zs[0+zi], xs[1+xi], ys[1+yi], zs[1+zi]);

         // Unlike getting, we don't stop after setting.  We will update the min and max values,
         
         branch->minValue = branch->children[0]->minValue;
         branch->maxValue = branch->children[0]->maxValue;

         for (unsigned int i = 1; i < Octree<T>::NCHILDREN; ++i)
         {
            branch->minValue = std::min(branch->minValue, branch->children[i]->minValue);
            branch->maxValue = std::max(branch->maxValue, branch->children[i]->maxValue);
         }

         // If the max and min values are the same, we can prune all child branches!
         if (branch->minValue == branch->maxValue)
            branch->destroyChildren();
      }
   }

   unsigned int m_Width;
   unsigned int m_Height;
   unsigned int m_Depth;

   Octree<T>* m_Root;
};

template <class T>
AWT::OctreeImage<T>::OctreeImage(const unsigned int w, const unsigned int h, const unsigned int d)
{
   m_D = new D;

   m_D->m_Width  = w;
   m_D->m_Height = h;
   m_D->m_Depth  = d;
}

template <class T>
AWT::OctreeImage<T>::~OctreeImage()
{
   delete m_D;
}

template <class T>
typename AWT::OctreeImage<T>::P AWT::OctreeImage<T>::getInstance(const unsigned int w, const unsigned int h, const unsigned int d)
{
   AUTOGETINSTANCE(AWT::OctreeImage<T>, (w, h, d));
}

template <class T>
GETNAMEMACRO(AWT::OctreeImage<T>);

template <class T>
void AWT::OctreeImage<T>::setValue(const unsigned int x, const unsigned int y, const unsigned int z, const T value)
{
   m_D->setValue(x, y, z, value);
}

template <class T>
T AWT::OctreeImage<T>::getValue(const unsigned int x, const unsigned int y, const unsigned int z) const
{
   return m_D->getValue(x, y, z);
}

template <class T>
void AWT::OctreeImage<T>::fill(const T value)
{
   m_D->fill(value);
}

template <class T>
unsigned int AWT::OctreeImage<T>::getWidth() const
{
   return m_D->m_Width;
}

template <class T>
unsigned int AWT::OctreeImage<T>::getHeight() const
{
   return m_D->m_Height;
}

template <class T>
unsigned int AWT::OctreeImage<T>::getDepth() const
{
   return m_D->m_Depth;
}

template <class T>
void AWT::OctreeImage<T>::visit(const T lower, const T upper, typename AWT::ImageVisitor<T>::P visitor)
{
   m_D->visit(lower, upper, visitor);
}

template <class T>
unsigned int AWT::Octree<T>::instanceCount = 0;

template <class T>
AWT::Octree<T>::Octree(const T value)
{
   for (unsigned int i = 0; i < NCHILDREN; ++i)
      children[i] = 0;

   minValue = maxValue = value;
   ++instanceCount;
}

template <class T>
AWT::Octree<T>::~Octree()
{
   // Recursively delete the children
   destroyChildren();
   --instanceCount;
}

template <class T>
void AWT::Octree<T>::createChildren()
{
   if (minValue != maxValue || children[0] != 0)
      AWTEXCEPTIONTHROW("Already have children!");

   for (unsigned int i = 0; i < NCHILDREN; ++i)
      children[i] = new Octree(minValue);
}

template <class T>
void AWT::Octree<T>::destroyChildren()
{
   for (unsigned int i = 0; i < NCHILDREN; ++i)
   {
      if (children[i] != 0)
         delete children[i];

      children[i] = 0;
   }
}

#endif // __OCTREEIMAGE_H__