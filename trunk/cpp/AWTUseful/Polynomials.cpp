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

#include "AWTUseful/Polynomials.h"

#include "AWTUseful/Complex.h"
#include "AWTUseful/Matrices.h"

template <class T>
void AWT::Polynomials<T>::calculateParabola(AWT::Complex<T>* in_xs, AWT::Complex<T>* in_ys, AWT::Complex<T>* out_coeffs)
{
   // Calculate the coefficients of the parabola in the form
   // y = out_coeffs[0] + x*out_coeffs[1] + x*x*out_coeffs[2]

   AWT::Complex<T>* matrix = new AWT::Complex<T>[9];
   AWT::Complex<T>* x2     = new AWT::Complex<T>[3];
   
   x2[0] = in_xs[0] * in_xs[0];
   x2[1] = in_xs[1] * in_xs[1];
   x2[2] = in_xs[2] * in_xs[2];
   
   matrix[0] = 1;
   matrix[1] = 1;
   matrix[2] = 1;

   matrix[3] = in_xs[0];
   matrix[4] = in_xs[1];
   matrix[5] = in_xs[2];

   matrix[6] = x2[0];
   matrix[7] = x2[1];
   matrix[8] = x2[2];

   AWT::Complex<T> d = AWT::Matrix<T>::determ3x3(matrix);

   matrix[0] = in_ys[0];
   matrix[1] = in_ys[1];
   matrix[2] = in_ys[2];

   out_coeffs[0] = AWT::Matrix<T>::determ3x3(matrix) / d;

   matrix[3] = 1;
   matrix[4] = 1;
   matrix[5] = 1;

   out_coeffs[1] = - AWT::Matrix<T>::determ3x3(matrix) / d;

   matrix[6] = in_xs[0];
   matrix[7] = in_xs[1];
   matrix[8] = in_xs[2];

   out_coeffs[2] = AWT::Matrix<T>::determ3x3(matrix) / d;

   delete matrix;
   delete x2;
}

template <class T>
AWT::Complex<T> AWT::Polynomials<T>::evaluatePolynomial(AWT::Complex<T>* in_coeffs, int in_ncoeffs, AWT::Complex<T> x)
{
   AWT::Complex<T> ret = in_coeffs[in_ncoeffs-1];

   for (int i = in_ncoeffs-2; i >= 0; i--)
      ret = x*ret + in_coeffs[i];

   return ret;
}

template <class T>
int AWT::Polynomials<T>::solvePolynomial(T* in_coeffs, int in_ncoeffs, T* out_roots)
{
   switch (in_ncoeffs)
   {
   case 1:
      return 0;
   case 2:
      return solveLinear(in_coeffs, out_roots);
   case 3:
      return solveQuadratic(in_coeffs, out_roots);
   default:
      throw "No solver for n > 3";
   }
}

template <class T>
int AWT::Polynomials<T>::solvePolynomial(AWT::Complex<T>* in_coeffs, int in_ncoeffs, AWT::Complex<T>* out_roots)
{
   switch (in_ncoeffs)
   {
   case 1:
      return 0;
   case 2:
      return solveLinear(in_coeffs, out_roots);
   case 3:
      return solveQuadratic(in_coeffs, out_roots);
   case 4:
      return solveCubic(in_coeffs, out_roots);
   default:
      throw "No solver for n > 4";
   }
}

template <class T>
int AWT::Polynomials<T>::solveLinear(T* in_coeffs, T* out_roots)
{
   if (in_coeffs[1] == 0)
   {
      return 0;
   }
   else
   {
      out_roots[0] = -in_coeffs[1] / in_coeffs[0];
      return 1;
   }
}

template <class T>
int AWT::Polynomials<T>::solveLinear(AWT::Complex<T>* in_coeffs, AWT::Complex<T>* out_roots)
{
   if (in_coeffs[1] == AWT::Complex<T>(0))
   {
      return 0;
   }
   else
   {
      out_roots[0] = -in_coeffs[1] / in_coeffs[0];
      return 1;
   }
}

template <class T>
int AWT::Polynomials<T>::solveQuadratic(T* in_coeffs, T* out_roots)
{
   // Solve the quadratic formula
   // 0 = a + b*x + c*x*x
   // x = (-b +/- sqrt(b*b - 4*c*a)) / 2*c

   if (in_coeffs[2] == 0)
      return solveLinear(in_coeffs, out_roots);

   T determ = in_coeffs[1]*in_coeffs[1] - in_coeffs[0]*in_coeffs[2]*static_cast<T>(4);
   
   if (determ > 0)
      return 0;

   determ = sqrt(determ);

   out_roots[0] = (-in_coeffs[1] + determ) / (in_coeffs[2] * 2);
   out_roots[1] = (-in_coeffs[1] - determ) / (in_coeffs[2] * 2);
      
   return 2;
}

template <class T>
int AWT::Polynomials<T>::solveQuadratic(AWT::Complex<T>* in_coeffs, AWT::Complex<T>* out_roots)
{
   // Solve the quadratic formula
   // 0 = a + b*x + c*x*x
   // x = (-b +/- sqrt(b*b - 4*c*a)) / 2*c

   if (in_coeffs[2] == AWT::Complex<T>(0))
      return solveLinear(in_coeffs, out_roots);

   AWT::Complex<T> determ = in_coeffs[1]*in_coeffs[1] - in_coeffs[0]*in_coeffs[2]*static_cast<T>(4);
   AWT::Complex<T> denom  = in_coeffs[2].power(-1.0) * static_cast<T>(0.5);

   out_roots[0] = (-in_coeffs[1] + determ.power(0.5, 0)) / (in_coeffs[2] * 2);
   out_roots[1] = (-in_coeffs[1] + determ.power(0.5, 1)) / (in_coeffs[2] * 2);
      
   return 2;
}

template <class T>
int AWT::Polynomials<T>::solveCubic(AWT::Complex<T>* in_coeffs, AWT::Complex<T>* out_roots)
{
   // Solve the cubic formula using Cardano's method... 
   // See http://en.wikipedia.org/wiki/Cubic_formula
   // x^3 + a*x^2 + b*x + c = 0

   AWT::Complex<T> a = in_coeffs[2] / in_coeffs[3];
   AWT::Complex<T> b = in_coeffs[1] / in_coeffs[3];
   AWT::Complex<T> c = in_coeffs[0] / in_coeffs[3];

   AWT::Complex<T> p = b - a.power(2.0) / 3.0;
   AWT::Complex<T> q = c + (a.power(3.0) * 2.0 - a * b * 9.0) / 27.0 ;

   if (q == static_cast<T>(0) && p == static_cast<T>(0))
   {
      // Special case, all roots are -a/3
      out_roots[0] = out_roots[1] = out_roots[2] = -a / 3.0;
   }
   else
   {
      AWT::Complex<T> u = q.power(2) / 4.0 + p.power(3) / 27.0;
      u = u.power(0.5);
      u = q / 2 + u;
      u = u.power(static_cast<T>(1.0/3.0));

      AWT::Complex<T> otherroots(-0.5, static_cast<T>(sqrt(3.0) / 2.0));
      otherroots = otherroots * 1;
      
      out_roots[0] = (p / u / 3.0) - u - (a / 3.0);
      u = u * otherroots;
      out_roots[1] = (p / u / 3.0) - u - (a / 3.0);
      u = u * otherroots;
      out_roots[2] = (p / u / 3.0) - u - (a / 3.0);
   }

   return 3;
}

template AWT::Polynomials<double>;
template AWT::Polynomials<float>;