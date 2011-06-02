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
