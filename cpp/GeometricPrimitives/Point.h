#ifndef __PRIMITIVES_POINT_H__
#define __PRIMITIVES_POINT_H__

#include "GeometricPrimitive.h"
#include "GeometryQueries.h"

namespace AWT
{
   namespace GeometricPrimitives
   {
      template <class T>
      class Triangle;

      template <class T>
      class Point : public GeometricPrimitive<T>
      {
         friend class GeometryQueries<T>;
         friend class Triangle<T>;

      public:
         static Point* getInstance( );
         static Point* getInstance( const T* in_p );
         static Point* getInstance( const Point<T>* in_p );

         void getPosition( T* out_p ) const;
         void getPosition( T& out_x, T& out_y, T& out_z, T& out_w ) const;
         void getPosition( Point<T>* out_p ) const;

         void setPosition( const T* in_p  );
         void setPosition( const T in_x, const T in_y, const T in_z, const T in_w = 1 );
         void setPosition( const Point<T>* in_p );

         void normalize( );

         T operator[]( unsigned int i ) const;
      protected:
         Point( const T* in_p );
         virtual ~Point( );

         T p[4];
      };
   }
}

namespace AWT
{
   namespace GeometricPrimitives
   {
      template <class T>
      Point<T>* Point<T>::getInstance( )
      {
         T p[] = { 0, 0, 0, 1 };
         return new Point<T>( p );
      }

      template <class T>
      Point<T>* Point<T>::getInstance( const T* in_p )
      {
         return new Point( in_p );
      }

      template <class T>
      Point<T>* Point<T>::getInstance( const Point<T>* in_p )
      {
         Point<T>* ret = getInstance( );
         ret->setPosition( in_p );
         return ret;
      }

      template <class T>
      Point<T>::~Point()
      {
      }

      template <class T>
      T Point<T>::operator[]( unsigned int i ) const
      {
         if ( i >= 4 || i < 0 )
            AWTEXCEPTIONTHROW("Index out of bounds!");

         return p[i];
      }

      template <class T>
      Point<T>::Point( const T* in_p )
      {
         for ( int i = 0; i < 4; ++i )
            p[i] = in_p[i];
      }

      template <class T>
      void Point<T>::getPosition( T* out_p ) const
      {
         for ( int i = 0; i < 4; ++i )
            out_p[i] = p[i];
      }

      template <class T>
      void Point<T>::getPosition( T& out_x, T& out_y, T& out_z, T& out_w ) const
      {
         out_x = p[0];
         out_y = p[1];
         out_z = p[2];
         out_w = p[3];
      }

      template <class T>
      void Point<T>::getPosition( Point<T>* out_p ) const
      {
         for ( int i = 0; i < 4; ++i )
            out_p->p[i] = this->p[i];
      }

      template <class T>
      void Point<T>::setPosition( const T* in_p )
      {
         for ( int i = 0; i < 4; ++i )
            p[i] = in_p[i];

         modified();
      }

      template <class T>
      void Point<T>::setPosition( const T in_x, const T in_y, const T in_z, const T in_w )
      {
         p[0] = in_x;
         p[1] = in_y;
         p[2] = in_z;
         p[3] = in_w;

         modified();
      }

      template <class T>
      void Point<T>::setPosition( const Point<T>* in_p )
      {
         for ( int i = 0; i < 4; ++i )
            this->p[i] = in_p->p[i];

         modified();
      }

      template <class T>
      void Point<T>::normalize( )
      {
         if ( p[3] != 0 )
         {
            for ( int i = 0; i < 4; ++i )
               p[i] /= p[3];
         }

         modified();
      }
   }
}

#endif // __PRIMITIVES_POINT_H__