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

      ColouredConsole( const int fg, const int bg = COL_BLACK );
      ~ColouredConsole( );

      template <typename T>
      AWT::ColouredConsole& operator<<( T val )
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