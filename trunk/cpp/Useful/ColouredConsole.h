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
#ifndef __COLOUREDCONSOLE_H__
#define __COLOUREDCONSOLE_H__

#include "windows.h"
#include <vector>

namespace AWT
{
   class ColouredConsole
   {
   public:
      enum Col {
         COL_BLACK = 0,
         COL_BLUE = 1,
         COL_GREEN = 2,
         COL_CYAN = 3,
         COL_RED = 4,
         COL_MAGENTA = 5,
         COL_YELLOW = 6,
         COL_WHITE = 7,
         COL_BRIGHT = 8,
      };

      ColouredConsole(const int fg, const int bg = COL_BLACK);
      ~ColouredConsole();

      template <typename T>
      AWT::ColouredConsole& operator<<(T val)
      {
         std::cerr << val;
         return *this;
      }

   protected:
      HANDLE hConsole;
      static std::vector< int > colStack;
   };

   
}

#endif // __COLOUREDCONSOLE_H__