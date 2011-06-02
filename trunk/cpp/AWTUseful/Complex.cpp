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
#include "AWTUseful/Complex.h"

template <class T>
const T AWT::Complex<T>::PI = static_cast<T>(atan(1.0) * 4);

template <class T>
AWT::Complex<T>::Complex(AWT::Complex<T> &other)
{
   x = other.x;
   y = other.y;
   rep = other.rep;
}

template <class T>
AWT::Complex<T>::Complex()
{
   x = 0;
   y = 0;
   rep = CARTESIAN;
}

template <class T>
AWT::Complex<T>::Complex(T _x)
{
   x = _x;
   y = 0;
   rep = CARTESIAN;
}

template <class T>
AWT::Complex<T>::Complex(T _x, T _y)
{
   x = _x;
   y = _y;
   rep = CARTESIAN;
}

template <class T>
AWT::Complex<T>::Complex(T _x, T _y, Representation _rep)
{
   x = _x;
   y = _y;
   rep = _rep;
}

template <class T>
T AWT::Complex<T>::real()
{
   switch (rep)
   {
   case CARTESIAN:
      return x;
   case POLAR:
      return x*cos(y);
   default:
      throw "Invalid representation";
   }
}

template <class T>
T AWT::Complex<T>::imag()
{
   switch (rep)
   {
   case CARTESIAN:
      return y;
   case POLAR:
      return x*sin(y);
   default:
      throw "Invalid representation";
   }
}

template <class T>
T AWT::Complex<T>::abs()
{
   switch (rep)
   {
   case CARTESIAN:
      return sqrt(x*x + y*y);
   case POLAR:
      return x;
   default:
      throw "Invalid representation";
   }
}

template <class T>
T AWT::Complex<T>::arg()
{
   switch (rep)
   {
   case CARTESIAN:
      return atan2(y, x);
   case POLAR:
      return y;
   default:
      throw "Invalid representation";
   }
}

template <class T>
AWT::Complex<T> AWT::Complex<T>::conj()
{
   return Complex<T>(x, -y, rep);
}

template <class T>
T AWT::Complex<T>::clampArgument(T newArg)
{
   while (newArg > PI)
      newArg -= 2*PI;

   while (newArg < -PI)
      newArg += 2*PI;

   return newArg;
}

template <class T>
AWT::Complex<T> AWT::Complex<T>::power(T in_pow)
{
   T newArg = clampArgument(in_pow * arg());   
   return Complex<T>(pow(abs(), in_pow), newArg, POLAR);
}

template <class T>
AWT::Complex<T> AWT::Complex<T>::power(T in_pow, int in_n)
{
   T newArg = clampArgument(static_cast<T>(in_pow * arg() + in_n * 8 * atan(1.0)*in_pow));
   return Complex<T>(pow(abs(), in_pow), newArg, POLAR);
}

template <class T>
AWT::Complex<T> AWT::Complex<T>::operator+(AWT::Complex<T>& other)
{
   return Complex<T>(this->real() + other.real(), this->imag() + other.imag(), CARTESIAN);
}

template <class T>
AWT::Complex<T> AWT::Complex<T>::operator+(T other)
{
   return Complex<T>(this->real() + other, this->imag(), CARTESIAN);
}

template <class T>
AWT::Complex<T> AWT::Complex<T>::operator-(AWT::Complex<T>& other)
{
   return Complex<T>(this->real() - other.real(), this->imag() - other.imag(), CARTESIAN);
}

template <class T>
AWT::Complex<T> AWT::Complex<T>::operator-(T other)
{
   return Complex<T>(this->real() - other, this->imag(), CARTESIAN);
}

template <class T>
AWT::Complex<T> AWT::Complex<T>::operator*(AWT::Complex<T>& other)
{
   return Complex<T>(this->abs() * other.abs(), clampArgument(this->arg() + other.arg()), POLAR);
}

template <class T>
AWT::Complex<T> AWT::Complex<T>::operator*(T other)
{
   return Complex<T>(this->abs() * other, this->arg(), POLAR);
}

template <class T>
AWT::Complex<T> AWT::Complex<T>::operator/(AWT::Complex<T>& other)
{
   return Complex<T>(this->abs() / other.abs(), clampArgument(this->arg() - other.arg()), POLAR);
}

template <class T>
AWT::Complex<T> AWT::Complex<T>::operator/(T other)
{
   return Complex<T>(this->abs() / other, this->arg(), POLAR);
}

template <class T>
bool AWT::Complex<T>::operator==(AWT::Complex<T>& other)
{
   if (rep == other.rep)
      return (x == other.x) && (y == other.y);
   else
      return (real() == other.real()) && (imag() == other.imag())
         || (abs() == other.abs()) && (arg() == other.arg());

}

template <class T>
bool AWT::Complex<T>::operator==(T other)
{
   return other == real() && imag() == 0;
}

template <class T>
AWT::Complex<T> AWT::Complex<T>::operator-()
{
   return Complex<T>(-this->real(), -this->imag(), CARTESIAN);
}

template AWT::Complex<double>;
template AWT::Complex<float>;