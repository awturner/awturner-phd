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
#ifndef __AWT_LIST_H__
#define __AWT_LIST_H__

#include "AWTUseful/Iterable.h"

namespace AWT
{
   namespace Container
   {
      template <class T>
      class List : public Iterable<T>
      {
      public:
         static List* New( );

         // Information methods
         virtual const bool isEmpty( )    = 0;
         virtual const int  size( )       = 0;
         virtual const T    get( int )    = 0;

         // List structure changing methods
         virtual void add( const T )      = 0;
         virtual void add( int, const T ) = 0;
         virtual T    remove( int )       = 0;
         virtual void clear( )            = 0;

         // For the iterable interface
         virtual Iterator<T>* iterator( ) = 0;
      };
   }
}

#endif // __AWT_LIST_H__