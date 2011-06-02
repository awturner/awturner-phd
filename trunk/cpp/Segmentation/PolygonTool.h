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
#ifndef __POLYGONTOOL_H__
#define __POLYGONTOOL_H__

#include "DrawingTool.h"
#include <vector>

namespace AWT
{
   template <class T, class ScanT, unsigned int V>
   class PolygonTool : public DrawingTool<T,ScanT,V>
   {
   public:
      PolygonTool(CommandManager* manager);

      virtual char* getName() const;

      virtual bool onMouseButton(const int x, const int y, const int z, const int button);
      virtual bool onMouseMove(const int x, const int y, const int z, const int button);

   protected:
      virtual void drawHandle(const int x, const int y);
      virtual int  findCurrentHandle(const int x, const int y);

      int mouseAtX;
      int mouseAtY;
      int mouseAtZ;

      std::vector<int> xs;
      std::vector<int> ys;
      std::vector<int> zs;
      int currentHandle;
   };
}

using namespace AWT;
using namespace cimg_library;

template <class T, class ScanT, unsigned int V>
AWT::PolygonTool<T,ScanT,V>::PolygonTool(AWT::CommandManager* m)
: DrawingTool<T,ScanT,V>(m)
{
   currentHandle = -1;
}

template <class T, class ScanT, unsigned int V>
char* AWT::PolygonTool<T,ScanT,V>::getName() const
{
   return "Polygon";
}

template <class T, class ScanT, unsigned int V>
int AWT::PolygonTool<T,ScanT,V>::findCurrentHandle(const int x, const int y)
{
   currentHandle = -1;

   for (int i = 0; i < xs.size(); ++i)
   {
      if (abs(xs[i]-x) <= 1 && abs(ys[i]-y) <= 1)
      {
         currentHandle = i;
         break;
      }
   }

   return currentHandle;
}

template <class T, class ScanT, unsigned int V>
bool AWT::PolygonTool<T,ScanT,V>::onMouseButton(const int x, const int y, const int z, const int button)
{
   currentHandle = -1;

   if (button)
   {
      findCurrentHandle(x, y);

      if (currentHandle == -1)
      {
         currentHandle = xs.size();

         xs.push_back(x);
         ys.push_back(y);
         zs.push_back(z);
      }

      return onMouseMove(mouseAtX = x, mouseAtY = y, mouseAtZ = z, button);
   }
   else if (xs.size() > 2)
   {
      if (abs(xs[0]-xs[xs.size()-1])<=1 && abs(ys[0]-ys[ys.size()-1])<=1)
      {
         xs[xs.size()-1] = xs[0];
         ys[ys.size()-1] = ys[0];

         T ofg = 127;
         T fg[V];

         getForegroundColour(&fg[0]);
         
         // Do the filling
         m_OverlayImage->draw_fill(0, 0, &ofg);

         cimg_forXY(*m_CurrentImage, xx, yy)
            if ((*m_OverlayImage)(xx,yy) != ofg)
               cimg_forV(*m_CurrentImage, v)
                  (*m_CurrentImage)(xx,yy,z,v) = fg[v];

         // Reset the overlay image
         m_OverlayImage->fill(0);

         // Delete the current points
         xs.clear();
         ys.clear();
         zs.clear();

         return true;
      }
   }

   return false;
}

#include <iostream>

template <class T, class ScanT, unsigned int V>
bool AWT::PolygonTool<T,ScanT,V>::onMouseMove(const int x, const int y, const int z, const int button)
{
   if (currentHandle != -1)
   {
      xs[currentHandle] = x;
      ys[currentHandle] = y;
      zs[currentHandle] = z;
   }

   if (m_CurrentImage != 0 && m_OverlayImage != 0)
   {
      // Clear the overlay image
      m_OverlayImage->fill(0);

      if (xs.size() > 0)
      {
         T fg = 255;

         for (int ii = 0; ii < xs.size()-1; ++ii)
         {
            //std::cerr << "(" << xs[ii+0] << "," << ys[ii+0] << ") -> (" << xs[ii+1] << "," << ys[ii+1] << ")" << std::endl;
            m_OverlayImage->draw_line(xs[ii+0], ys[ii+0], xs[ii+1], ys[ii+1], &fg);
            
            /*
            for (int yy = std::max(0,ys[ii+1]-1); yy <= std::min(m_OverlayImage->dimy(),ys[ii+1]+1); ++yy)
               for (int xx = std::max(0,xs[ii+1]-1); xx <= std::min(m_OverlayImage->dimx(),xs[ii+1]+1); ++xx)
                  (*m_OverlayImage)(xx,yy) = fg;
            */
         }
         
         if (xs.size() == 1)
            drawHandle(xs[0], ys[0]);
         else if (currentHandle != -1)
            drawHandle(xs[currentHandle], ys[currentHandle]);
      }

      return true;
   }
   return false;
}

template <class T, class ScanT, unsigned int V>
void AWT::PolygonTool<T,ScanT,V>::drawHandle(const int x, const int y)
{
   for (int yy = std::max(0,y-1); yy <= std::min(m_OverlayImage->dimy(),y+1); ++yy)
      for (int xx = std::max(0,x-1); xx <= std::min(m_OverlayImage->dimx(),x+1); ++xx)
         (*m_OverlayImage)(xx,yy) = 255;
}

#endif // __POLYGONTOOL_H__