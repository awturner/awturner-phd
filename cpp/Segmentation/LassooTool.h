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
#ifndef __LASSOOTOOL_H__
#define __LASSOOTOOL_H__

#include "DrawingTool.h"

#include <vector>

namespace AWT
{
   template <class T, class ScanT, unsigned int V>
   class LassooTool : public DrawingTool<T,ScanT,V>
   {
   public:
      LassooTool(CommandManager* m);

      virtual char* getName() const;

      virtual bool onMouseButton(const int x, const int y, const int z, const int button);
      virtual bool onMouseMove(const int x, const int y, const int z, const int button);

   protected:
      std::vector<int> xs;
      std::vector<int> ys;

      int stipple;
   };
}

using namespace AWT;
using namespace cimg_library;

template <class T, class ScanT, unsigned int V>
AWT::LassooTool<T,ScanT,V>::LassooTool(AWT::CommandManager* m)
: AWT::DrawingTool<T,ScanT,V>::DrawingTool(m)
{
   stipple = 0x55555555;
}

template <class T, class ScanT, unsigned int V>
char* AWT::LassooTool<T,ScanT,V>::getName() const
{
   return "Lassoo";
}

template <class T, class ScanT, unsigned int V>
bool AWT::LassooTool<T,ScanT,V>::onMouseButton(const int mx, const int my, const int mz, const int button)
{
   if (m_CurrentImage != 0)
   {
      if (button)
      {
         // Make sure the loop is clear
         xs.clear();
         ys.clear();

         xs.push_back(mx);
         ys.push_back(my);
      }
      else
      {
         CImg<T> tmp(m_CurrentImage->dimx(), m_CurrentImage->dimy());
         tmp.fill(0);
         T col[] = { 1 };

         for (int i = 1; i < xs.size(); ++i)
         {
            tmp.draw_line(xs[i-1], ys[i-1], xs[i], ys[i], col);
         }
         tmp.draw_line(xs[xs.size()-1], ys[ys.size()-1], xs[0], ys[0], col);

         cimg_forXY(tmp, x, y)
         {
            if (tmp(x, y) != col[0])
            {
               col[0] = 2;
               tmp.draw_fill(x, y, col);

               // Now we have 3 values of pixel:
               // 1 is the boundary we have painted
               // 0 is on one side of the boundary, 2 is on the other
               // Need to determine which of these is the one which has been
               // lassooed.  The boundary is always closed by construction.

               {
                  cimg_for_borderXY(tmp, x, y, 1)
                  {
                     if (tmp(x, y) != 1)
                     {
                        col[0] = tmp(x, y);
                        x = tmp.dimx();
                        y = tmp.dimy();
                        break;
                     }
                  }
               }
               
               // Get out of the loop
               x = tmp.dimx();
               y = tmp.dimy();
               break;
            }
         }

         int x, y, v;
         cimg_forXY(tmp, x, y)
         {
            if (tmp(x, y) != col[0])
            {
               cimg_forV(tmp, v)
               {
                  if ((*m_CurrentImage)(x,y,mz,v) == m_Background[v])
                  {
                     // If it isn't, then paint it
                     cimg_forV(tmp, v)
                        (*m_CurrentImage)(x,y,mz,v) = m_Foreground[v];
                     break;
                  }
               }
            }
         }

         xs.clear();
         ys.clear();

         if (m_OverlayImage)
            m_OverlayImage->fill(0);

         return true;
      }
   }
   
   return false;
}

template <class T, class ScanT, unsigned int V>
bool AWT::LassooTool<T,ScanT,V>::onMouseMove(const int x, const int y, const int z, const int button)
{
   if (button)
   {
      xs.push_back(x);
      ys.push_back(y);

      if (m_OverlayImage != 0)
      {
         unsigned char white[] = { 255 };

         if (xs.size() > 1)
         {
            int i = xs.size()-1;
            m_OverlayImage->draw_line(xs[i-1], ys[i-1], xs[i], ys[i], white, 1.f, stipple);
         }

         return true;
      }
   }

   return false;
}


#endif // __LASSOOTOOL_H__