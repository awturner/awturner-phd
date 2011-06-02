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