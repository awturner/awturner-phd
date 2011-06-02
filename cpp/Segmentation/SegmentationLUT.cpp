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
#include "SegmentationLUT.h"

using namespace AWT;

AWT::SegmentationLUT::SegmentationLUT( )
{
   // By default, all colours are gray
   for ( int i = 0; i < 768; ++i )
   {
      deltas[i] = 255;
      underCoat[i] = 0;
   }

   cols = 0;
}

unsigned char AWT::SegmentationLUT::getNumberOfColours( ) const
{
   return cols+1;
}

void AWT::SegmentationLUT::setColour( unsigned char label, unsigned char or, unsigned char og, unsigned char ob,
                                      unsigned char ur, unsigned char ug, unsigned char ub )
{
   this->underCoat[3*label + 0] = ur;
   this->underCoat[3*label + 1] = ug;
   this->underCoat[3*label + 2] = ub;

   this->deltas[3*label + 0] = or - ur;
   this->deltas[3*label + 1] = og - ug;
   this->deltas[3*label + 2] = ob - ub;

   if ( cols < label )
      cols = label;
}

void AWT::SegmentationLUT::setColour( unsigned char label, unsigned char or, unsigned char og, unsigned char ob )
{
   this->setColour( label, or, og, ob, or >> 2, og >> 2, ob >> 2 );
}

void AWT::SegmentationLUT::setColour(unsigned char label, unsigned char *overcoat, unsigned char *undercoat)
{
   this->setColour( label, overcoat[0], overcoat[1], overcoat[2], undercoat[0], undercoat[1], undercoat[2] );
}

void AWT::SegmentationLUT::getColour(float v, unsigned char label, unsigned char *out_Colour)
{
   if ( v < 0 ) v = 0.f;
   if ( v > 1 ) v = 1.f;

   out_Colour[0] = this->underCoat[3*label+0] + v * this->deltas[3*label+0];
   out_Colour[1] = this->underCoat[3*label+1] + v * this->deltas[3*label+1];
   out_Colour[2] = this->underCoat[3*label+2] + v * this->deltas[3*label+2];
}