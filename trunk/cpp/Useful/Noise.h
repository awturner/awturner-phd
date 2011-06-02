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
#ifndef __GAUSSIANNOISE_H__
#define __GAUSSIANNOISE_H__

#include <ctime>
#include <cstdlib>

namespace AWT
{
   template <class T>
   class Noise
   {
      const static int q;
      const static T c1;
      const static T c2;
      const static T c3;

   public:
      static void timeSeed();
      static T randn(T mu = 0, T std = 1);
      static T randu(T lower = 0, T upper = 1);
   };
}

template <class T>
void AWT::Noise<T>::timeSeed()
{
   srand(static_cast<unsigned int>(time(0)));
}

template <class T>
T AWT::Noise<T>::randu(T lower = 0, T upper = 1)
{
   return static_cast<T>(rand()) / RAND_MAX * (upper - lower) + lower;
}

template <class T>
const int AWT::Noise<T>::q = 15;

template <class T>
const T AWT::Noise<T>::c1 = static_cast<T>((1 << AWT::Noise<T>::q) - 1);

template <class T>
const T AWT::Noise<T>::c2 = static_cast<T>(((int)(AWT::Noise<T>::c1 / 3)) + 1);

template <class T>
const T AWT::Noise<T>::c3 = (1 + (T)0) / AWT::Noise<T>::c1;

#include "Maths/Constants.h"

template <class T>
T AWT::Noise<T>::randn(T mu, T std)
{
   T rand1 = randu();
   T rand2 = randu();

   return mu + std*sqrt(-2 * log(rand1)) * cos(2*Constants<T>::Pi*rand2);

   /*
   T random = (static_cast<T>(rand()) / static_cast<T>(RAND_MAX + 1));
   return mu + std*((static_cast<T>(2) * ((random * c2) + (random * c2) + (random * c2)) - static_cast<T>(3) * (c2 - static_cast<T>(1))) * c3);
   */
}

#endif __GAUSSIANNOISE_H__