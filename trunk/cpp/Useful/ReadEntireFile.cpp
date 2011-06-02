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
#include "ReadEntireFile.h"

#include "Useful/Exception.h"

#include <fstream>
#include <sstream>

AWT::ReadEntireFile::ReadEntireFile()
{
}

AWT::ReadEntireFile::~ReadEntireFile()
{
}

std::string AWT::ReadEntireFile::read(const std::string& filename)
{
   std::ifstream is(filename.c_str());

   if (is.bad())
      AWTEXCEPTIONTHROW_STREAM("Could not read from " << filename);

   std::stringstream sstr;
   char ch;

   while (true)
   {
      is.get(ch);

      if (!is.good())
         break;

      sstr << ch;
   }

   is.close();

   return sstr.str();
}