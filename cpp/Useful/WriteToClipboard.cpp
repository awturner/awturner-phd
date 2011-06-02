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
#include "WriteToClipboard.h"

#ifdef _WIN32

#include "windows.h"

AWT::WriteToClipboard::WriteToClipboard()
{
}

AWT::WriteToClipboard::~WriteToClipboard()
{
}

void AWT::WriteToClipboard::copyToClipboard(const std::string& string)
{
   int ok = OpenClipboard(NULL);
   
   if (!ok)
   {
      DEBUGMACRO("Could not open clipboard");
   }
   /* else */

	HGLOBAL clipbuffer;
	char * buffer;
	EmptyClipboard();
	clipbuffer = GlobalAlloc(GMEM_DDESHARE, string.length() + 1);
	buffer = (char*)GlobalLock(clipbuffer);
	strcpy(buffer, string.c_str());
	GlobalUnlock(clipbuffer);
	SetClipboardData(CF_TEXT,clipbuffer);
	CloseClipboard();

   DEBUGMACRO("Data copied to clipboard clipboard");
}

#endif