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
#ifndef __ROWFILLTOOL_H__
#define __ROWFILLTOOL_H__

#include "DrawingTool.h"

#include <ostream>

namespace AWT
{
   enum DirectionLR  { DIR_LEFT, DIR_RIGHT };

   template <class T, class ScanT, unsigned int V>
   class RowFillTool : public DrawingTool<T,ScanT,V>
   {
   public:
      RowFillTool(CommandManager* m, DirectionLR d);

      virtual char* getName() const;

      virtual bool onMouseMove(const int x, const int y, const int z, const int button);
      virtual bool onMouseButton(const int x, const int y, const int z, const int button);
      
      virtual void printState(std::ostream& os) const
      {
         os << "OK";
      }

   protected:
      bool decideRange(const CImg<T>& im, const int y, 
                          const T fg[V], const T bg[V],
                          unsigned int& xstart, unsigned int& xend);

      DirectionLR dir;

      std::vector<bool> rowFlags;

      int mouseAtX;
      int mouseAtY;

      int lastMouseAtX;
      int lastMouseAtY;
      int lastButton;
   };
}

using namespace AWT;
using namespace cimg_library;

template <class T, class ScanT, unsigned int V>
AWT::RowFillTool<T,ScanT,V>::RowFillTool(AWT::CommandManager* m, AWT::DirectionLR dir)
: DrawingTool<T,ScanT,V>(m)
{
   lastMouseAtX = lastMouseAtY = -1;
   lastButton = -1;

   this->dir = dir;
}

template <class T, class ScanT, unsigned int V>
char* AWT::RowFillTool<T,ScanT,V>::getName() const
{
   switch (dir)
   {
   case DIR_LEFT:
      return "Left Row Fill";
   case DIR_RIGHT:
      return "Right Row Fill";
   default:
      return "Unknown Row Fill";
   }
}

template <class T, class ScanT, unsigned int V>
bool AWT::RowFillTool<T,ScanT,V>::decideRange(const CImg<T>& im, const int y, 
                                                  const T fg[V], const T bg[V],
                                                  unsigned int& xstart, unsigned int& xend)
{
   switch (dir)
   {
   case DIR_RIGHT:
      {
         // From the current location, search down the column for the first foreground pixel
         while (xstart < im.dimx() && im(xstart, y) != fg[0])
            ++xstart;

         if (xstart == im.dimx())
            return false;

         // From the current location, search down the column for the first background pixel
         while (xstart < im.dimx() && im(xstart, y) != bg[0])
            ++xstart;

         if (xstart == im.dimx())
            return false;
         
         xend = xstart + 1;

         while (xend < im.dimx() && im(xend, y) == bg[0])
            ++xend;

         if (xend == im.dimx())
            return false;
      }
      break;
   case DIR_LEFT:
      {
         // From the current location, search up the column for the first foreground pixel
         while (im(xstart, y) != fg[0])
         {
            if (xstart == 0) return false;

            --xstart;
         }

         // From the current location, search up the column for the first background pixel
         while (im(xstart, y) != bg[0])
         {
            if (xstart == 0) return false;

            --xstart;
         }

         xend = xstart;

         while (im(xend, y) == bg[0])
         {
            if (xend == 0) return false;

            --xend;
         }
         
         unsigned int tmp = xstart;
         xstart = xend;
         xend = tmp;
      }
      break;
   }

   return true;
}

template <class T, class ScanT, unsigned int V>
bool AWT::RowFillTool<T,ScanT,V>::onMouseMove(const int mx, const int my, const int mz, const int button)
{
   if (m_CurrentImage != 0 && m_OverlayImage != 0)
   {
      CImg<T> im = m_CurrentImage->get_shared_plane(mz);
      
      T fg[V], bg[V];
      getForegroundColour(fg);
      getBackgroundColour(bg);

      m_OverlayImage->fill(0);

      // Paint a line to show where the mouse is
      cimg_forX(im, x)
         (*m_OverlayImage)(x, my) = 64;

      cimg_forY(im, y)
         (*m_OverlayImage)(mx, y) = 64;

      if (abs(mx - lastMouseAtX) > 10)
         lastMouseAtX = (mx > lastMouseAtX) ? (mx-1) : (mx+1);

      for (unsigned int y = 0; y < im.dimy(); ++y)
      {
         unsigned int xstart, xend;

         xstart = mx;
         if (decideRange(im, y, fg, bg, xstart, xend))
         {
            bool xbetween = 
               (my >= lastMouseAtY && y > lastMouseAtY && y <= my) ||  // Moved right
               (my <= lastMouseAtY && y < lastMouseAtY && y >= my);     // Moved left

            if (button == 1 && xbetween && ((lastButton == 1 && my != lastMouseAtY) || lastButton != 1) && !rowFlags[y])
            {
               for (unsigned int x = xstart; x <= xend; ++x)
                  cimg_forV(im, v)
                     im(x, y, 0, v) = fg[v];

               rowFlags[y] = true;
            }
            else
            {
               for (unsigned int x = xstart; x <= xend; ++x)
                  (*m_OverlayImage)(x, y) = xbetween ? 255 : 192;
            }
         }
      }

      lastMouseAtX = mx;
      lastMouseAtY = my;
      lastButton   = button;

      return true;
   }

   lastMouseAtX = mx;
   lastMouseAtY = my;
   lastButton   = button;

   return false;
}

template <class T, class ScanT, unsigned int V>
bool AWT::RowFillTool<T,ScanT,V>::onMouseButton(const int mx, const int my, const int mz, const int button)
{
   lastMouseAtX = mx;
   lastMouseAtY = my;

   rowFlags.clear();

   if (m_CurrentImage)
      for (unsigned int i = 0; i < m_CurrentImage->dimy(); ++i)
         rowFlags.push_back(false);

   bool ret = onMouseMove(mx, my, mz, button);

   lastButton = button;

   return ret;
}

#endif // __ROWFILLTOOL_H__