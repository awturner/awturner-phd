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
#ifndef __HUEWHEELSEQUENCE_H__
#define __HUEWHEELSEQUENCE_H__

#include "ColourSequence.h"
#include "ColourConversions.h"

namespace AWT
{
   class HueWheelSequence : public ColourSequence
   {
   public:
      typedef ManagedAutoPointer<HueWheelSequence> P;

   protected:
      HueWheelSequence( const unsigned int ncolours, const double saturation = 1, const double value = 1, const double alpha = 1 );

      virtual ~HueWheelSequence( );

   public:
      static HueWheelSequence::P getInstance(const unsigned int ncolours, const double saturation = 1, const double value = 1, const double alpha = 1 );

      virtual std::string getClassName( ) const;

      virtual void nextColour( float* colour );

      virtual void nextColour( double* colour );

   protected:
      struct D;
      D* m_D;
   };
}

#endif // __HUEWHEELSEQUENCE_H__