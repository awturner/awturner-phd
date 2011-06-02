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
#ifndef __FILLTOOL_H__
#define __FILLTOOL_H__

#include "DrawingTool.h"

#include <ostream>

namespace AWT
{
   template <class T, class ScanT, unsigned int V>
   class FillTool : public DrawingTool<T,ScanT,V>
   {
   public:
      FillTool(CommandManager* m);

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

      int mouseAtX;
      int mouseAtY;

      ErrorState errorState;
   };
}

using namespace AWT;
using namespace cimg_library;

template <class T, class ScanT, unsigned int V>
AWT::FillTool<T,ScanT,V>::FillTool(AWT::CommandManager* m)
: DrawingTool<T,ScanT,V>(m)
{
   errorState = FILL_NOERROR;
}

template <class T, class ScanT, unsigned int V>
char* AWT::FillTool<T,ScanT,V>::getName() const
{
   return "Fill";
}

template <class T, class ScanT, unsigned int V>
bool AWT::FillTool<T,ScanT,V>::onMouseMove(const int mx, const int my, const int mz, const int button)
{
   if (m_CurrentImage != 0 && m_OverlayImage != 0)
   {
      CImg<T> im = m_CurrentImage->get_shared_plane(mz);
      
      T curr[V];
      cimg_forV(im, v)
         curr[v] = im(mx, my, 0, v);

      T fg[V];
      getForegroundColour(fg);

      m_OverlayImage->fill(0);

      im.draw_fill(mx, my, 0, fg, *m_OverlayImage);

      cimg_forXY((*m_OverlayImage), xx, yy)
         if ((*m_OverlayImage)(xx,yy))
         {
            (*m_OverlayImage)(xx,yy) = 255;
            cimg_forV(im, v)
               im(xx,yy,0,v) = curr[v];
         }
      
      return true;
   }

   return false;
}

template <class T, class ScanT, unsigned int V>
bool AWT::FillTool<T,ScanT,V>::onMouseButton(const int mx, const int my, const int mz, const int button)
{
   if (m_CurrentImage != 0 && button == 1)
   {
      CImg<T> im = m_CurrentImage->get_shared_plane(mz);

      int x, y, z, v;
      bool flag = true;

      T fg[V];
      getForegroundColour(fg);

      cimg_forV(im, v)
      {
         if (im(mx,my,0,v) != fg[v])
         {
            flag = false;
            break;
         }
      }

      if (flag)
      {
         errorState = FILL_ALREADYCOLOUR;
         return true;
      }
      
      CImg<T> region(im.dimx(), im.dimy(), im.dimz(), V);

      im.draw_fill(mx, my, 0, fg, region, 0, 0);

      /*
      unsigned long count = 0;
      const unsigned long maxcount = region.size();
      cimg_forXYZ(region, x, y, z)
      {
         if (region(x, y, z))
            if (++count > maxcount)
            {
               flag = true;
               x = region.dimx();
               y = region.dimy();
               z = region.dimz();
               break;
            }
      }
      */

      if (!flag)
      {
         cimg_forXYZ(region, x, y, z)
         {
            if (region(x,y,z))
            {
               cimg_forV(im, v)
               {
                  im(x,y,z,v) = fg[v];
               }
            }
         }

         errorState = FILL_NOERROR;
         return true;
      }
      else
      {
         errorState = FILL_AREATOOLARGE;
         return true;
      }
   }

   errorState = FILL_NOERROR;
   return false;
}

#endif // __FILLTOOL_H__