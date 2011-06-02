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
#ifndef __RUNLENGTHENCODE_H__
#define __RUNLENGTHENCODE_H__

#include <CImg.h>
#include <ostream>
#include <istream>

#include "Exception.h"
#include "Useful/PrintMacros.h"

namespace AWT
{
   template <class Ty>
   class WriteRunLengthCallback
   {
   public:
      virtual void start() = 0;
      virtual void setDataType(const unsigned char type) = 0;
      virtual void setDimensions(const unsigned int w, const unsigned int h, const unsigned int d, const unsigned int v) = 0;

      virtual void handle(const unsigned long runLength, const Ty* values, const int nvalues) = 0;
   };

   template <class Ty>
   class OutputStreamWriteRunLengthCallback : public WriteRunLengthCallback<Ty>
   {
   public:
      OutputStreamWriteRunLengthCallback(std::ostream* os);

      virtual void start();
      virtual void setDataType(const unsigned char type);
      virtual void setDimensions(const unsigned int w, const unsigned int h, const unsigned int d, const unsigned int v);

      virtual void handle(const unsigned long runLength, const Ty* values, const int nvalues);

   protected:
      static void writeRun(std::ostream& os, unsigned char type, unsigned long runLength);

      std::ostream* os;
      unsigned char type;
      unsigned long maxLength;
   };

   template <class Ty>
   class FooRunLengthCallback : public WriteRunLengthCallback<Ty>
   {
   public:
      virtual void start() {}
      virtual void setDataType(const unsigned char type) {}
      virtual void setDimensions(const unsigned int w, const unsigned int h, const unsigned int d, const unsigned int v) {}

      virtual void handle(const unsigned long runLength, const Ty* values, const int nvalues)
      {
         std::cerr << runLength;
         
         for (int i = 0; i < nvalues; ++i)
            std::cerr << " " << (1*values[i]);

         std::cerr << std::endl;
      }
   };

   template <class Ty>
   void writeToStream(std::ostream& os, Ty v, const unsigned int size, const bool binary = false);

   template <class Ty>
   void writeToStream(std::ostream& os, Ty v, const bool binary = false);

   template <class Ty>
   void readFromStream(std::istream& is, Ty& v, const unsigned int size);

   template <class Ty>
   void readFromStream(std::istream& is, Ty& v);


   template <class Ty>
   bool runLengthEncode(cimg_library::CImg<Ty>& img, std::ostream& os);

   template <class Ty>
   bool runLengthEncode(cimg_library::CImg<Ty>& img, WriteRunLengthCallback<Ty>* cb);

   template <class Ty>
   cimg_library::CImg<Ty> runLengthDecode(std::istream& is);

   template <class Ty>
   int analyzeRuns(cimg_library::CImg<Ty>& img);
   
   unsigned long readRunLength(std::istream& is, unsigned char type);
}

using namespace cimg_library;
using namespace std;
using namespace AWT;

template <class Ty>
void AWT::writeToStream(ostream& os, Ty v, const unsigned int size, const bool binary)
{
   if (binary)
      os.write((char*)&v, size);
   else
      os << v;
}

template <class Ty>
void AWT::writeToStream(ostream& os, Ty v, const bool binary)
{
   writeToStream(os, v, sizeof(v), binary);
}

template <class Ty>
void AWT::readFromStream(std::istream& is, Ty& v, const unsigned int size)
{
   is.read((char*)(&v), size);
}

template <class Ty>
void AWT::readFromStream(std::istream& is, Ty& v)
{
   readFromStream<Ty>(is, v, sizeof(Ty));
}

template <class Ty>
CImg<Ty> AWT::runLengthDecode(std::istream& is)
{
   char buffer[8];
   readFromStream<char>(is, buffer[0], 8);

   if (memcmp(buffer, "CIMGRLEN", 8))
   {
      AWTEXCEPTIONTHROW("Wrong Magic Number!");
   }
   
   unsigned int w, h, d, p;
   unsigned char type;
   readFromStream(is, type);
   readFromStream(is, w);
   readFromStream(is, h);
   readFromStream(is, d);
   readFromStream(is, p);

   PRINTVBL(1*type);
   PRINTVBL(1*w);
   PRINTVBL(1*h);
   PRINTVBL(1*d);
   PRINTVBL(1*p);

   CImg<Ty> im(w, h, d, p);
   im.fill(0);

   unsigned int x, y, z;

   unsigned long pixels = w*h*d;
   unsigned long runLength;
   unsigned long runPos;

   //char padding;

   Ty currentValue;

   cimg_forV(im, v)
   {
      x = y = z = 0;
      runPos = 0;

      while (runPos < pixels)
      {
         // I've got a padding char in there...
         //readFromStream(is, padding);

         runLength = readRunLength(is, type);
         if (!is.good())
         {
            std::cerr << "Error reading run length from stream" << std::endl;
            break;
         }

         readFromStream(is, currentValue);
         if (!is.good())
         {
            std::cerr << "Error reading value from stream" << std::endl;
            break;
         }

         //std::cerr << "Handling run of length " << runLength << " of value " << (1*currentValue) << std::endl;

         while (runLength-- > 0 && runPos < pixels)
         {
            x = runPos % w;
            y = (runPos / w) % h;
            z = runPos / (w*h);

            im(x, y, z, v) = currentValue;

            ++runPos;
         }
      }
   }

   return im;
}

template <class Ty>
int AWT::analyzeRuns(CImg<Ty>& img)
{
   unsigned long counts[4];

   counts[0] = counts[1] = counts[2] = counts[3] = 0;

   unsigned long runLength;
   Ty            currentValue;

   const double LOG2 = log(2.0);

   const int bytesPerRun = img.dimv() * sizeof(Ty);

   // Calculate the file size in advance for different encodings
   cimg_forV(img, v)
   {
      runLength = 0;
      currentValue = img(0, 0, 0, v);

      cimg_forXYZ(img, x, y, z)
      {
         if (img(x, y, z, v) == currentValue)
         {
            ++runLength;
         }
         else
         {
            counts[0] += (bytesPerRun + sizeof(unsigned char) ) * static_cast<int>(ceil((0.0 + runLength) /        256.0));
            counts[1] += (bytesPerRun + sizeof(unsigned short)) * static_cast<int>(ceil((0.0 + runLength) /      65536.0));
            counts[2] += (bytesPerRun + sizeof(unsigned int)  ) * static_cast<int>(ceil((0.0 + runLength) / 4294967296.0));

            counts[3] += (bytesPerRun + sizeof(unsigned long));

            runLength = 1;
            currentValue = img(x, y, z, v);
         }
      }

      counts[0] += (bytesPerRun + sizeof(unsigned char) ) * static_cast<int>(ceil((0.0 + runLength) /        256.0));
      counts[1] += (bytesPerRun + sizeof(unsigned short)) * static_cast<int>(ceil((0.0 + runLength) /      65536.0));
      counts[2] += (bytesPerRun + sizeof(unsigned int)  ) * static_cast<int>(ceil((0.0 + runLength) / 4294967296.0));

      counts[3] += (bytesPerRun + sizeof(unsigned long));
   }

   int mini = 0;
   for (int i = 1; i < 4; ++i)
      if (counts[i] < counts[mini])
         mini = i;

   return mini;
}

template <class Ty>
bool AWT::runLengthEncode(CImg<Ty>& img, std::ostream& os)
{
   OutputStreamWriteRunLengthCallback<Ty>* cb = new OutputStreamWriteRunLengthCallback<Ty>(&os);

   bool ret = runLengthEncode(img, cb);

   delete cb;

   return ret;
}

template <class Ty>
bool AWT::runLengthEncode(CImg<Ty>& img, WriteRunLengthCallback<Ty>* cb)
{
   unsigned char dataType = analyzeRuns<Ty>(img);
   std::cerr << "Recommended data type = " << (1*dataType) << std::endl;

   unsigned long runLength;

   cb->start();
   cb->setDataType(dataType);
   cb->setDimensions(img.dimx(), img.dimy(), img.dimz(), img.dimv());

   Ty* values        = new Ty[ img.dimv() ];
   Ty* currentValues = new Ty[ img.dimv() ];
   bool different    = false;

   runLength = 0;
   cimg_forV(img, v)
      currentValues[v] = img(0, 0, 0, v) + 1; // Force a new run on the first pixel

   cimg_forXYZ(img, x, y, z)
   {
      different = false;

      // Check to see if this pixel is different from the last one
      cimg_forV(img, v)
      {
         if (img(x, y, z, v) != currentValues[v])
         {
            values[v] = img(x, y, z, v);
            different = true;
         }
      }

      if (!different)
      {
         ++runLength;
      }
      else
      {
         cb->handle(runLength, &currentValues[0], img.dimv());

         cimg_forV(img, v)
            currentValues[v] = values[v];

         runLength = 1;
      }
   }

   cb->handle(runLength, &currentValues[0], img.dimv());

   delete [] values;
   delete [] currentValues;

   return true;
}

template <class Ty>
AWT::OutputStreamWriteRunLengthCallback<Ty>::OutputStreamWriteRunLengthCallback(std::ostream *os)
{
   this->os = os;
}

template <class Ty>
void AWT::OutputStreamWriteRunLengthCallback<Ty>::start()
{
   *os << "CIMGRLEN";
}

template <class Ty>
void AWT::OutputStreamWriteRunLengthCallback<Ty>::setDataType(const unsigned char type)
{
   writeToStream(*os, this->type = type, true);

   switch (type)
   {
   case 0:
      maxLength = std::numeric_limits<unsigned char>::max();
      break;
   case 1:
      maxLength = std::numeric_limits<unsigned short>::max();
      break;
   case 2:
      maxLength = std::numeric_limits<unsigned int>::max();
      break;
   case 3:
      maxLength = std::numeric_limits<unsigned long>::max();
      break;
   default:
      AWTEXCEPTIONTHROW("Invalid type!");
   }
}

template <class Ty>
void AWT::OutputStreamWriteRunLengthCallback<Ty>::setDimensions(const unsigned int w, 
                                                        const unsigned int h, const unsigned int d, const unsigned int v)
{
   writeToStream(*os, w, true);
   writeToStream(*os, h, true);
   writeToStream(*os, d, true);
   writeToStream(*os, v, true);
}

#include <limits>

template <class Ty>
void AWT::OutputStreamWriteRunLengthCallback<Ty>::handle(const unsigned long runLength, const Ty* values, const int nvalues)
{
   if (runLength != 0)
   {
      unsigned long runLengthCopy = runLength;

      //std::cerr << "Handling run of length " << runLength << " of value " << (1*values[0]) << std::endl;
      //*os << "#";

      while (runLengthCopy > maxLength)
      {
         writeRun(*os, type, maxLength);
         for (int i = 0; i < nvalues; ++i)
            writeToStream(*os, values[i], true);

         runLengthCopy -= maxLength;
      }

      writeRun(*os, type, runLengthCopy);
      for (int i = 0; i < nvalues; ++i)
         writeToStream(*os, values[i], true);
   }
}

template <class Ty>
void AWT::OutputStreamWriteRunLengthCallback<Ty>::writeRun(std::ostream& os, unsigned char type, unsigned long runLength)
{
   switch (type)
   {
   case 0:
      writeToStream(os, static_cast<unsigned char>(runLength), true);
      break;
   case 1:
      writeToStream(os, static_cast<unsigned short>(runLength), true);
      break;
   case 2:
      writeToStream(os, static_cast<unsigned int>(runLength), true);
      break;
   case 3:
      writeToStream(os, static_cast<unsigned long>(runLength), true);
      break;
   }
}

#endif // __RUNLENGTHENCODE_H__