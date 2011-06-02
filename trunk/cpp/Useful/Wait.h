#ifndef __WAIT_H__
#define __WAIT_H__

#ifndef WIN32
 #include <unistd.h>
#endif

#include <iostream>
 #ifndef WIN32
  #include <stdio.h>
 #endif
using namespace std;

#define WAIT_MSG "Press enter to continue..."

namespace AWT
{
   // http://cpp.codenewbie.com/articles/cpp/1437/systemPAUSE-Page_1.html
   void pause( )
   {
     int c;

     cout << WAIT_MSG << endl;
     /* eat up characters until a newline or eof */
     do
     {
       c = getchar( );
       if(c == EOF) break;
     } while(c != '\n');
   }
}

#endif // __WAIT_H__