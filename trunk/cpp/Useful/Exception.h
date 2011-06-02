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
#ifndef __AWTEXCEPTION_H__
#define __AWTEXCEPTION_H__

#include <string>
#include <ostream>
#include <sstream>
#include <exception>

#define AWTEXCEPTIONTHROWCLASS(c, m) { throw m; }
#define AWTEXCEPTIONTHROW(m) AWTEXCEPTIONTHROWCLASS(AWT::Exception, m)

#define AWTEXCEPTIONTHROWCLASS_STREAM(c, m) { std::stringstream ss; ss << m; throw c(ss.str(), __FILE__, __LINE__); }
#define AWTEXCEPTIONTHROW_STREAM(m) AWTEXCEPTIONTHROWCLASS_STREAM(AWT::Exception, m)

namespace AWT
{
   class Exception : public std::exception
   {
   public:
      Exception(std::string _m, std::string _f, int _l)
         : className("AWT::Exception"), message(_m), file(_f), line(_l)
      {
      }

      static friend std::ostream& operator<<(std::ostream& os, AWT::Exception& ex)
      {
         os << ex.className << " in file " << ex.file << " at line " << ex.line << ": " << ex.message;
         return os;
      }

      const std::string getClassName() const
      {
         return className;
      }

      const std::string getMessage() const
      {
         return message;
      }

      const int getLineNumber() const
      {
         return line;
      }

   protected:
      const std::string className;
      const std::string message;
      const std::string file;
      const int line;
   };
}

#endif // __AWTEXCEPTION_H__