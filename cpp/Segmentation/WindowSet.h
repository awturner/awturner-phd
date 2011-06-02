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
#ifndef __WINDOWSET_H__
#define __WINDOWSET_H__

#include "CImg.h"
#include <vector>

using namespace cimg_library;

namespace AWT
{
   class WindowSet
   {
   public:
      void add(CImgDisplay* display);

      bool isShown() const;

      void show();
      void hide();
   protected:
      std::vector<CImgDisplay*> m_Displays;
   };
}

bool AWT::WindowSet::isShown() const
{
   for (unsigned int i = 0; i < m_Displays.size(); ++i)
      if (!m_Displays[i]->is_closed)
         return true;

   return false;
}

void AWT::WindowSet::add(cimg_library::CImgDisplay *display)
{
   m_Displays.push_back(display);
}

void AWT::WindowSet::show()
{
   for (unsigned int i = 0; i < m_Displays.size(); ++i)
   {
      if (m_Displays[i]->is_closed)
         m_Displays[i]->show();
   }
}

void AWT::WindowSet::hide()
{
   for (unsigned int i = 0; i < m_Displays.size(); ++i)
   {
      if (! m_Displays[i]->is_closed)
         m_Displays[i]->close();
   }
}

#endif // __WINDOWSET_H__