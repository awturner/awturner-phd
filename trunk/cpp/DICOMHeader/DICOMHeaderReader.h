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
#ifndef __DICOMHEADERREADER_H__
#define __DICOMHEADERREADER_H__

#include <istream>

namespace AWT
{
   typedef unsigned short GroupType;
   typedef unsigned short ElementType;

   class DICOMTagIdentifier
   {
   public:
      DICOMTagIdentifier();
      DICOMTagIdentifier(GroupType _group, ElementType _element);

      bool compare(GroupType g, ElementType e) const;
      
      GroupType group;
      ElementType element;
   };

   class DICOMTagIdentifierConsumer
   {
   public:
      virtual bool wantTag(DICOMTagIdentifier ge) = 0;

      virtual bool implicitDataCallback(DICOMTagIdentifier ge, char* buffer, int length) { return true; }
      virtual bool explicitDataCallback(DICOMTagIdentifier ge, char* vr, char* buffer, int length) { return true; }
   };

   class DICOMHeaderReader
   {
   public:
      static bool readStream(std::istream& in, DICOMTagIdentifierConsumer* consumer);
   };
}

#endif __DICOMHEADERREADER_H__