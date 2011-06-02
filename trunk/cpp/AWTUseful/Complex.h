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

#ifndef __AWTCOMPLEX_H__
#define __AWTCOMPLEX_H__

#include <cmath>
#include <iostream>

namespace AWT
{
   template <class T>
   class Complex
   {
   public:
      static const T PI;

      Complex( Complex& other );
      Complex( );
      Complex( T _x );
      Complex( T _x, T _y );

      T real( );
      T imag( );
      T abs( );
      T arg( );

      Complex conj( );
      Complex power( T pow );
      Complex power( T pow, int n );

      Complex operator+( T scalar );
      Complex operator+( Complex& other );

      Complex operator-( T scalar );
      Complex operator-( Complex& other );

      Complex operator*( T scalar );
      Complex operator*( Complex& other );

      Complex operator/( T scalar );
      Complex operator/( Complex& other );

      bool    operator==( T scalar );
      bool    operator==( Complex& other );

      Complex operator-( );

      friend std::ostream& operator<<(std::ostream & os, Complex& c )
      {
         double x, y;

         switch (c.rep)
         {
         case CARTESIAN:
            x = c.real( );
            y = c.imag( );

            if ( x == 0 && y == 0 )
            {
               os << x;
            }
            else
            {
               if ( x != 0.0 )
                  os << x;

               if ( y != 0.0 )
               {
                  if ( x != 0.0 && y > 0 )
                     os << "+";

                  os << y << "*j";
               }
            }
            break;
         case POLAR:
            x = c.abs( );
            y = c.arg( );

            os << x << "*exp(" << y << "j)";
            break;
         }

        return os;
      }
   protected:
      enum Representation { CARTESIAN, POLAR };

      Complex( T _x, T _y, Representation _rep );

      static T clampArgument( T newArg );

      T x;
      T y;
      Representation rep;
   };

}

#endif //__AWTCOMPLEX_H__