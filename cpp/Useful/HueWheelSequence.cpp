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
#include "HueWheelSequence.h"

struct AWT::HueWheelSequence::D
{
   unsigned int m_ColN;
   unsigned int m_ColourCount;
   double m_Saturation;
   double m_Value;
   double m_Alpha;
};

AWT::HueWheelSequence::HueWheelSequence(const unsigned int ncolours, const double saturation, const double value, const double alpha)
{
   m_D = new D;

   m_D->m_Saturation = saturation;
   m_D->m_Value = value;
   m_D->m_Alpha = alpha;
   m_D->m_ColourCount = ncolours;
   m_D->m_ColN = 0;
}

AWT::HueWheelSequence::~HueWheelSequence()
{
   delete m_D;
}

AWT::HueWheelSequence::P AWT::HueWheelSequence::getInstance(const unsigned int ncolours, const double saturation, const double value, const double alpha)
{
   AUTOGETINSTANCE(AWT::HueWheelSequence, (ncolours, saturation, value, alpha));
}

GETNAMEMACRO(AWT::HueWheelSequence);

void AWT::HueWheelSequence::nextColour(float* colour)
{
   double dcol[4];
   nextColour(dcol);

   for (int i = 0; i < 4; ++i)
      colour[i] = static_cast<float>(dcol[i]);
}

void AWT::HueWheelSequence::nextColour(double* colour)
{
   double hsva[] = { (m_D->m_ColN+0.0) / m_D->m_ColourCount, m_D->m_Saturation, m_D->m_Value, m_D->m_Alpha };

   AWT::ColourConversions::hsvToRgb(&hsva[0], colour);

   m_D->m_ColN = ++m_D->m_ColN % m_D->m_ColourCount;
}