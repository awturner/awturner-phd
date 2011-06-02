
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