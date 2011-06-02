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
#ifndef __HOLEFILLTOOL_H__
#define __HOLEFILLTOOL_H__

#include "DrawingTool.h"

#include <vector>

namespace AWT
{
   template <class T, class ScanT, unsigned int V>
   class HoleFillTool : public DrawingTool<T,ScanT,V>
   {
   public:
      HoleFillTool(CommandManager* m);

      virtual char* getName() const;

      virtual bool onMouseButton(const int x, const int y, const int z, const int button);
   protected:
   };
}

using namespace AWT;
using namespace cimg_library;

template <class T, class ScanT, unsigned int V>
AWT::HoleFillTool<T,ScanT,V>::HoleFillTool(AWT::CommandManager* m)
: AWT::DrawingTool<T,ScanT,V>::DrawingTool(m)
{
}

template <class T, class ScanT, unsigned int V>
char* AWT::HoleFillTool<T,ScanT,V>::getName() const
{
   return "Hole Fill";
}

template <class T, class ScanT, unsigned int V>
bool AWT::HoleFillTool<T,ScanT,V>::onMouseButton(const int mx, const int my, const int mz, const int button)
{
   //DEBUGMACRO("onMouseButton");
   if (m_CurrentImage != 0 && !button)
   {
      CImg<T> im = m_CurrentImage->get_shared_plane(mz);

      CImg<T> tmp   (im.dimx(), im.dimy(), im.dimz());
      CImg<T> region(im.dimx(), im.dimy(), im.dimz());

      tmp.fill(0);

      T fg[V], bg[V];
      getForegroundColour(fg);
      getBackgroundColour(bg);

      bool isSame;
      cimg_forXY(tmp, x, y)
      {
         isSame = true;

         cimg_forV(im, v)
         {
            if (im(x, y, 0, v) != bg[v])
            {
               isSame = false;
               break;
            }
         }

         if (isSame)
            tmp(x, y) = 1;
      }

      T fill[] = { 0 };
      tmp.draw_fill(mx, my, 0, fill, region);

      cimg_forXY(tmp, x, y)
      {
         if (tmp(x, y))
            cimg_forV(im, v)
               im(x, y, 0, v) = fg[v];
      }

      return true;
   }
   
   return false;
}

#endif // __HOLEFILLTOOL_H__