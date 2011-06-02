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
#ifndef __SEARCHFILTER_H__
#define __SEARCHFILTER_H__

#include "ObjectManagement/ManagedObject.h"

#include "Mesh.h"
#include <vector>

namespace AWT
{
   namespace SimpleMesh
   {
      class SearchFilter : public ManagedObject
      {
      public:
         typedef ManagedAutoPointer<SearchFilter> P;

      protected:
         SearchFilter( );
         virtual ~SearchFilter( );

      public:
         static P getInstance( );
         std::string getClassName( ) const;

         bool handle( const Point p, const Index i );

      public:
         virtual bool check( const Point p, const Index i ) const;
         virtual void accept( const Point p, const Index i );
      };
   }
}

#endif // __SEARCHFILTER_H__