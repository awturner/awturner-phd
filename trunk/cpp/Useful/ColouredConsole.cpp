#include "Useful/ColouredConsole.h"

std::vector<int> AWT::ColouredConsole::colStack;

AWT::ColouredConsole::ColouredConsole( const int fg, const int bg )
{
   hConsole = GetStdHandle(STD_ERROR_HANDLE);

   if ( colStack.empty() )
      colStack.push_back( COL_WHITE );

   int col = bg << 4 | fg;
   SetConsoleTextAttribute( hConsole, col );

   colStack.push_back( col );
}

AWT::ColouredConsole::~ColouredConsole( )
{
   colStack.pop_back();
   SetConsoleTextAttribute( hConsole, colStack.back() );
}