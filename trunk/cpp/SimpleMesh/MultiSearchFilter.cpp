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
#include "MultiSearchFilter.h"

AWT::SimpleMesh::MultiSearchFilter::MultiSearchFilter(const unsigned int alloc)
{
   filters.reserve(alloc);
}

AWT::SimpleMesh::MultiSearchFilter::~MultiSearchFilter()
{
}

AWT::SimpleMesh::MultiSearchFilter::P AWT::SimpleMesh::MultiSearchFilter::getInstance(const unsigned int alloc)
{
   AUTOGETINSTANCE(AWT::SimpleMesh::MultiSearchFilter, (alloc));
}

GETNAMEMACRO(AWT::SimpleMesh::MultiSearchFilter);

bool AWT::SimpleMesh::MultiSearchFilter::check(const Point p, const Index i) const
{
   typedef std::vector<SearchFilter::P> Filters;

   for (Filters::const_iterator it = filters.begin(), en = filters.end(); it != en; ++it)
   {
      if (!(*it)->check(p, i))
         return false;
   }

   return true;
}

void AWT::SimpleMesh::MultiSearchFilter::accept(const Point p, const Index i)
{
   typedef std::vector<SearchFilter::P> Filters;

   for (Filters::iterator it = filters.begin(), en = filters.end(); it != en; ++it)
   {
      (*it)->accept(p, i);
   }
}

void AWT::SimpleMesh::MultiSearchFilter::addFilter(SearchFilter::P filter)
{
   filters.push_back(filter);
}