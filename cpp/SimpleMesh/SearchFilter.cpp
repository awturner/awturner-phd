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
#include "SearchFilter.h"

#include "MeshFunctions.h"

using namespace AWT::SimpleMesh;

AWT::SimpleMesh::SearchFilter::SearchFilter()
{
}

AWT::SimpleMesh::SearchFilter::~SearchFilter()
{
}

AWT::SimpleMesh::SearchFilter::P AWT::SimpleMesh::SearchFilter::getInstance( )
{
   AUTOGETINSTANCE( AWT::SimpleMesh::SearchFilter, ( ) );
}

GETNAMEMACRO( AWT::SimpleMesh::SearchFilter );

bool AWT::SimpleMesh::SearchFilter::handle( Point p, Index i )
{
   if ( check( p, i ) )
   {
      accept( p, i );
      return true;
   }
   else
   {
      return false;
   }
}

bool AWT::SimpleMesh::SearchFilter::check( Point p, Index i ) const
{
   // Accept by default
   return true;
}

void AWT::SimpleMesh::SearchFilter::accept( Point p, Index i )
{
   // Do nothing by default
}
