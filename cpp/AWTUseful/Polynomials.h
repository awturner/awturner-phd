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
#ifndef __AWTPOLYNOMIAL_H__
#define __AWTPOLYNOMIAL_H__

#include "AWTUseful/Complex.h"

namespace AWT
{
   template <class T>
   class Polynomials
   {
   public:
      // Evaluate the real polynomial 
      //   f(in_x) =       in_coeffs[0] 
      //            + in_x*in_coeffs[1]
      //            + ...
      //            + in_x^(in_ncoeffs-1)*in_coeffs[in_ncoeffs-1]
      static AWT::Complex<T> evaluatePolynomial( AWT::Complex<T>* in_coeffs, int in_ncoeffs, AWT::Complex<T> in_x );

      // Calculate the parabola through 3 points (in_xs[0],in_ys[0]), 
      // (in_xs[1],in_ys[1]) and (in_xs[2],in_ys[2]). 
      // Return the coefficients so that
      // f(x) = out_coeffs[0] + x*out_coeffs[1] + x*x*out_coeffs[2]
      static void calculateParabola( AWT::Complex<T>* in_xs, AWT::Complex<T>* in_ys, AWT::Complex<T>* out_coeffs );

      // Solve the real polynomial 
      //   f(in_x) =       in_coeffs[0] 
      //            + in_x*in_coeffs[1]
      //            + ...
      //            + in_x^(in_ncoeffs-1)*in_coeffs[in_ncoeffs-1]
      // Returns the number of roots found

      static int solvePolynomial( T* in_coeffs, int in_ncoeffs, T* out_roots );
      static int solvePolynomial( AWT::Complex<T>* in_coeffs, int in_ncoeffs, AWT::Complex<T>* out_roots );

   protected:

      // Specific solvers for cubic case
      static int solveCubic( AWT::Complex<T>* in_coeffs, AWT::Complex<T>* out_roots );

      // Specific solvers for quadratic case
      static int solveQuadratic( T* in_coeffs, T* out_roots );
      static int solveQuadratic( AWT::Complex<T>* in_coeffs, AWT::Complex<T>* out_roots );   

      // Specific solvers for linear case
      static int solveLinear( T* in_coeffs, T* out_roots );
      static int solveLinear( AWT::Complex<T>* in_coeffs, AWT::Complex<T>* out_roots );

   };
}

#endif //__AWTPOLYNOMIAL_H__