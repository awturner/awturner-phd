#include "Timestamp.h"

#include <windows.h>
#include <stdio.h>
#include <time.h>

std::string AWT::getTimestamp()
{
   SYSTEMTIME st;
   GetSystemTime(&st);

   char buffer[16];
   sprintf_s( buffer, "%04d%02d%02d-%02d%02d%02d", st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);

   return std::string( buffer );
}