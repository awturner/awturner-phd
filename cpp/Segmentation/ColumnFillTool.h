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
#ifndef __COLUMNFILLTOOL_H__
#define __COLUMNFILLTOOL_H__

#include "DrawingTool.h"

#include <ostream>

namespace AWT
{
   enum Direction  { DIR_UPWARDS, DIR_DOWNWARDS };

   template <class T, class ScanT, unsigned int V>
   class ColumnFillTool : public DrawingTool<T,ScanT,V>
   {
   public:
      ColumnFillTool(CommandManager* m, Direction d);

      virtual char* getName() const;

      virtual bool onMouseMove(const int x, const int y, const int z, const int button);
      virtual bool onMouseButton(const int x, const int y, const int z, const int button);
      
      virtual void printState(std::ostream& os) const
      {
         switch (errorState)
         {
         case FILL_AREATOOLARGE:
            os << "Fill area too big!";
            break;
         default:
            os << "OK";
         }
      }

   protected:
      enum ErrorState { FILL_NOERROR, FILL_AREATOOLARGE, FILL_ALREADYCOLOUR };

      bool decideRange(const CImg<T>& im, const int x, 
                                                  const T fg[V], const T bg[V],
                                                  unsigned int& ystart, unsigned int& yend);

      Direction dir;

      std::vector<bool> columnFlags;

      int mouseAtX;
      int mouseAtY;

      int lastMouseAtX;
      int lastMouseAtY;
      int lastButton;

      ErrorState errorState;
   };
}

using namespace AWT;
using namespace cimg_library;

template <class T, class ScanT, unsigned int V>
AWT::ColumnFillTool<T,ScanT,V>::ColumnFillTool(AWT::CommandManager* m, AWT::Direction dir)
: DrawingTool<T,ScanT,V>(m)
{
   errorState = FILL_NOERROR;

   lastMouseAtX = lastMouseAtY = -1;
   lastButton = -1;

   this->dir = dir;
}

template <class T, class ScanT, unsigned int V>
char* AWT::ColumnFillTool<T,ScanT,V>::getName() const
{
   switch (dir)
   {
   case DIR_UPWARDS:
   return "Upwards Column Fill";
   case DIR_DOWNWARDS:
   return "Downwards Column Fill";
   default:
      return "Unknown Column Fill";
   }
}

template <class T, class ScanT, unsigned int V>
bool AWT::ColumnFillTool<T,ScanT,V>::decideRange(const CImg<T>& im, const int x, 
                                                  const T fg[V], const T bg[V],
                                                  unsigned int& ystart, unsigned int& yend)
{
   switch (dir)
   {
   case DIR_DOWNWARDS:
      {
         // From the current location, search down the column for the first foreground pixel
         while (ystart < im.dimy() && im(x, ystart) != fg[0])
            ++ystart;

         if (ystart == im.dimy())
            return false;

         // From the current location, search down the column for the first background pixel
         while (ystart < im.dimy() && im(x, ystart) != bg[0])
            ++ystart;

         if (ystart == im.dimy())
            return false;
         
         yend = ystart + 1;

         while (yend < im.dimy() && im(x, yend) == bg[0])
            ++yend;

         if (yend == im.dimy())
            return false;
      }
      break;
   case DIR_UPWARDS:
      {
         // From the current location, search up the column for the first foreground pixel
         while (im(x, ystart) != fg[0])
         {
            if (ystart == 0) return false;

            --ystart;
         }

         // From the current location, search up the column for the first background pixel
         while (im(x, ystart) != bg[0])
         {
            if (ystart == 0) return false;

            --ystart;
         }

         yend = ystart;

         while (im(x, yend) == bg[0])
         {
            if (yend == 0) return false;

            --yend;
         }
         
         unsigned int tmp = ystart;
         ystart = yend;
         yend = tmp;
      }
      break;
   }

   return true;
}

template <class T, class ScanT, unsigned int V>
bool AWT::ColumnFillTool<T,ScanT,V>::onMouseMove(const int mx, const int my, const int mz, const int button)
{
   if (m_CurrentImage != 0 && m_OverlayImage != 0)
   {
      CImg<T> im = m_CurrentImage->get_shared_plane(mz);
      
      T curr[V];
      cimg_forV(im, v)
         curr[v] = im(mx, my, 0, v);

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

      for (unsigned int x = 0; x < im.dimx(); ++x)
      {
         unsigned int ystart, yend;

         ystart = my;
         if (decideRange(im, x, fg, bg, ystart, yend))
         {
            bool xbetween = 
               (mx >= lastMouseAtX && x > lastMouseAtX && x <= mx) ||  // Moved right
               (mx <= lastMouseAtX && x < lastMouseAtX && x >= mx);     // Moved left

            if (button == 1 && xbetween && ((lastButton == 1 && mx != lastMouseAtX) || lastButton != 1) && !columnFlags[x])
            {
               for (unsigned int y = ystart; y <= yend; ++y)
                  cimg_forV(im, v)
                     im(x, y, 0, v) = fg[v];

               columnFlags[x] = true;
            }
            else
            {
               for (unsigned int y = ystart; y <= yend; ++y)
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
bool AWT::ColumnFillTool<T,ScanT,V>::onMouseButton(const int mx, const int my, const int mz, const int button)
{
   lastMouseAtX = mx;
   lastMouseAtY = my;

   columnFlags.clear();

   if (m_CurrentImage)
      for (unsigned int i = 0; i < m_CurrentImage->dimx(); ++i)
         columnFlags.push_back(false);

   bool ret = onMouseMove(mx, my, mz, button);

   lastButton = button;

   return ret;
}

#endif // __COLUMNFILLTOOL_H__