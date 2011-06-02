#include "WriteToClipboard.h"

#ifdef _WIN32

#include "windows.h"

AWT::WriteToClipboard::WriteToClipboard( )
{
}

AWT::WriteToClipboard::~WriteToClipboard( )
{
}

void AWT::WriteToClipboard::copyToClipboard( const std::string& string )
{
   int ok = OpenClipboard(NULL);
   
   if (!ok)
   {
      DEBUGMACRO( "Could not open clipboard" );
   }
   /* else */

	HGLOBAL clipbuffer;
	char * buffer;
	EmptyClipboard( );
	clipbuffer = GlobalAlloc(GMEM_DDESHARE, string.length( ) + 1);
	buffer = (char*)GlobalLock(clipbuffer);
	strcpy(buffer, string.c_str( ));
	GlobalUnlock(clipbuffer);
	SetClipboardData(CF_TEXT,clipbuffer);
	CloseClipboard( );

   DEBUGMACRO( "Data copied to clipboard clipboard" );
}

#endif