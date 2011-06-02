#define _WIN32_WINDOWS 0x501
#define _WIN32_WINNT 0x0501
#include <windows.h>
#include <stdio.h>

#include "Useful/PrintMacros.h"
//
HHOOK g_hMouseHook;

struct Calibration
{
   double offset;
   double scale;
};

Calibration calibX;
Calibration calibY;
Calibration calibW;

int mouseX = 0, mouseY = 0, wheel = 0;

void report( )
{
   double mx = mouseX * calibX.scale + calibX.offset;
   double my = mouseY * calibY.scale + calibY.offset;
   double mw = wheel  * calibW.scale + calibW.offset;

   std::cout << mx << "\t" << my << "\t" << mw << std::endl;
   std::cerr << mx << "\t" << my << "\t" << mw << std::endl;
}

//
LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
   if (nCode >= 0) {
      PMSLLHOOKSTRUCT pmll = (PMSLLHOOKSTRUCT) lParam;

      switch ( wParam )
      {
      case WM_MOUSEWHEEL:
         wheel += (short) HIWORD(pmll->mouseData);
      case WM_MOUSEMOVE:
         mouseX = pmll->pt.x;
         mouseY = pmll->pt.y;
         report( );
         break;
      }
   }
   return CallNextHookEx(g_hMouseHook, nCode, wParam, lParam);
}

void usage( )
{
   std::cerr << "Usage: MouseHook <x offset> <x scale> <y offset> <y scale> <wheel offset> <wheel scale>" << std::endl;
   std::cerr << std::endl;
   std::cerr << "\t"   << "Offset/Scale pairs should be specified such that the output value is given by:-" << std::endl;
   std::cerr << "\t\t" << "output = Offset + input*scale" << std::endl;
   std::cerr << std::endl;
}
//
int main( int argc, char** argv ) {
   if ( argc != 7 )
   {
      usage( );
      exit( 1 );
   }

   calibX.offset = atof( argv[1] );
   calibX.scale  = atof( argv[2] );
   if ( calibX.scale == 0 )
   {
      std::cerr << "Warning: X scale factor is zero." << std::endl;
   }

   calibY.offset = atof( argv[3] );
   calibY.scale  = atof( argv[4] );
   if ( calibY.scale == 0 )
   {
      std::cerr << "Warning: Y scale factor is zero." << std::endl;
   }

   calibW.offset = atof( argv[5] );
   calibW.scale  = atof( argv[6] );
   if ( calibW.scale == 0 )
   {
      std::cerr << "Warning: Wheel scale factor is zero." << std::endl;
   }


   MSG msg;
   g_hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, GetModuleHandle(NULL), 0);
   if (!g_hMouseHook) { printf("Error: %d\n", GetLastError()); }
   while(GetMessage(&msg, NULL, 0, 0)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
   }
   UnhookWindowsHookEx(g_hMouseHook);
   return (int) msg.wParam;
}
