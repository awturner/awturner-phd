#include "Cls.h"

#include "windows.h"
// http://support.microsoft.com/kb/99261

/* Standard error macro for reporting API errors */ 
#define PERR(bSuccess, api){if(!(bSuccess)) printf("%s:Error %d from %s on line %d\n", __FILE__, GetLastError(), api, __LINE__);}

void AWT::cls( )
{
   // Top left corner coordinates
   COORD coord = {0};           

   // Contains information about screen buffer     
   CONSOLE_SCREEN_BUFFER_INFO cBufferInfo;    

   // Characters written in buffer
   DWORD dwI;        

   // Characters to clear ("area" of console, so to speak)
   DWORD dwSize;                

   // Handle to console
   HANDLE hI = GetStdHandle(STD_OUTPUT_HANDLE); 

   // Just a bit error handling
   if(hI == INVALID_HANDLE_VALUE)  throw "Could not get console handle";

   // GetConsoleScreenBufferInfo returns 0 on error
   if (GetConsoleScreenBufferInfo(hI, &cBufferInfo))      
   {
      // Calculate characters to clear
      dwSize = cBufferInfo.dwSize.X * cBufferInfo.dwSize.Y;

      // Write space characters to buffer as many times as needed, i.e. dwSize    
      FillConsoleOutputCharacter(hI, TEXT(' '), dwSize, coord, &dwI );

      // Set console cursor to top left coord
      SetConsoleCursorPosition(hI, coord); 

   } else
      throw "Could not clear screen";

}
