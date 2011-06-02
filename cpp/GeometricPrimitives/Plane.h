#ifndef __PRIMITIVES_PLANE_H__
#define __PRIMITIVES_PLANE_H__

#include "GeometricPrimitive.h"

namespace AWT
{
   namespace GeometricPrimitives
   {
      template <class T>
      class Plane : public GeometricPrimitive<T>
      {
         friend class GeometryQueries<T>;

      public:
         static Plane* getInstance( );
         static Plane* getInstance( const T* in_p );

         void getCoefficients( T* out_p ) const;
         void getCoefficients( T& out_a, T& out_b, T& out_c, T& out_d ) const;

         void setCoefficients( const T* in_p  );
         void setCoefficients( const T in_a, const T in_b, const T in_c, const T in_d = 1 );

         void normalizeCoefficients( );

         T operator[]( unsigned int i ) const;

      protected:
         Plane( const T* in_p );
         virtual ~Plane( );

         static T getNormalLengthSquared( const T* vals );
         static T getNormalLengthSquared( const T in_a, const T in_b, const T in_c );

         T p[4];
      };
   }
}

#include <cmath>

namespace AWT
{
   namespace GeometricPrimitives
   {
      template <class T>
      Plane<T>* Plane<T>::getInstance( )
      {
         T p[] = { 0, 0, 1, 0 };
         return new Plane<T>( p );
      }

      template <class T>
      Plane<T>* Plane<T>::getInstance( const T* in_p )
      {
         return new Plane( in_p );
      }

      template <class T>
      Plane<T>::~Plane()
      {
      }

      template <class T>
      T Plane<T>::getNormalLengthSquared( const T* p )
      {
         return p[0]*p[0] + p[1]*p[1] + p[2]*p[2];
      }

      template <class T>
      T Plane<T>::getNormalLengthSquared( const T a, const T b, const T c )
      {
         return a*a + b*b + c*c;
      }

      template <class T>
      Plane<T>::Plane( const T* in_p )
      {
         for ( int i = 0; i < 4; ++i )
            p[i] = in_p[i];
      }

      template <class T>
      void Plane<T>::getCoefficients( T* out_p ) const
      {
         for ( int i = 0; i < 4; ++i )
            out_p[i] = p[i];
      }

      template <class T>
      T Plane<T>::operator[]( unsigned int i ) const
      {
         if ( i < 0 || i >= 4 )
            AWTEXCEPTIONTHROW("Index out of bounds!");

         return p[i];
      }

      template <class T>
      void Plane<T>::getCoefficients( T& out_a, T& out_b, T& out_c, T& out_d ) const
      {
         out_a = p[0];
         out_b = p[1];
         out_c = p[2];
         out_d = p[3];
      }

      template <class T>
      void Plane<T>::setCoefficients( const T* in_p )
      {
         if ( !checkVector( in_p, 4 ) )
            AWTEXCEPTIONTHROW( "Infinity or NaN in input" );

         if ( getNormalLengthSquared( in_p ) == 0 )
            AWTEXCEPTIONTHROW( "Coefficients make plane degenerate!" );

         for ( int i = 0; i < 4; ++i )
            p[i] = in_p[i];

         modified();
      }

      template <class T>
      void Plane<T>::setCoefficients( const T in_a, const T in_b, const T in_c, const T in_d )
      {
         if ( !checkValue( in_a ) || !checkValue( in_b ) || !checkValue( in_c ) || !checkValue( in_d ) )
            AWTEXCEPTIONTHROW( "Infinity or NaN in input" );

         if ( getNormalLengthSquared( in_a, in_b, in_c ) == 0 )
            AWTEXCEPTIONTHROW( "Coefficients make plane degenerate!" );

         p[0] = in_a;
         p[1] = in_b;
         p[2] = in_c;
         p[3] = in_d;

         modified();
      }

      template <class T>
      void Plane<T>::normalizeCoefficients( )
      {
         T len = sqrt( getNormalLengthSquared( &p[0] ) );

         p[0] /= len;
         p[1] /= len;
         p[2] /= len;
         p[3] /= len;

         modified();
      }
   }
}

#endif // __PRIMITIVES_PLANE_H__