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
#ifndef __PRIMITIVES_TRIANGLE_H__
#define __PRIMITIVES_TRIANGLE_H__

#include "GeometricPrimitive.h"

#include "Point.h"

namespace AWT
{
   namespace GeometricPrimitives
   {
      template <class T>
      class Triangle : public GeometricPrimitive<T>
      {
         friend class GeometryQueries<T>;

      public:
         static Triangle* getInstance( Point<T>* in_a, Point<T>* in_b, Point<T>* in_c );

         const Point<T>* getPoint( unsigned int i ) const;
         const Point<T>* getNormal( );

         virtual void refresh( );

      protected:
         Triangle( Point<T>* in_a, Point<T>* in_b, Point<T>* in_c );
         virtual ~Triangle( );

         Point<T>* points[3];
         Point<T>* normal;
      };
   }
}

namespace AWT
{
   namespace GeometricPrimitives
   {
      template <class T>
      Triangle<T>* Triangle<T>::getInstance(Point<T> *in_a, Point<T> *in_b, Point<T> *in_c)
      {
         return new Triangle<T>( in_a, in_b, in_c );
      }

      template <class T>
      Triangle<T>::Triangle(Point<T> *in_a, Point<T> *in_b, Point<T> *in_c)
      {         
         if ( in_a->p[3] == 0 && in_b->p[3] == 0 && in_c->p[3] == 0 )
            AWTEXCEPTIONTHROW("This triangle is infinite!");

         points[0] = in_a;
         points[1] = in_b;
         points[2] = in_c;

         normal = 0;
      }

      template <class T>
      Triangle<T>::~Triangle( )
      {
         if ( normal != 0 )
            normal->unregisterUse();

         for ( int i = 0; i < 3; ++i )
            points[i]->unregisterUse();
      }

      template <class T>
      void Triangle<T>::refresh()
      {
         if ( normal != 0 )
            normal->unregisterUse();

         normal = Point<T>::getInstance( );

         T pa[3];
         T pb[3];
         T pc[4];

         for ( int j = 0; j < 3; ++j )
         {
            pa[j] = points[1]->p[j] - points[0]->p[j];
            pb[j] = points[2]->p[j] - points[0]->p[j];
         }

         T len = 0;

         for ( int j = 0; j < 3; ++j )
         {
            pc[j] = pa[(j+1)%3]*pb[(j+2)%3] - pa[(j+2)%3]*pb[(j+1)%3];
            len += pc[j]*pc[j];
         }

         if ( len != 0 )
         {
            len = sqrt( len );

            for ( int j = 0; j < 3; ++j )
            {
               pc[j] /= len;
            }
         }

         pc[3] = 0;

         normal->setPosition( pc );
      }

      template <class T>
      const Point<T>* Triangle<T>::getPoint( unsigned int i ) const
      {
         if ( i < 0 || i >= 3 )
            AWTEXCEPTIONTHROW("Index out of bounds!");

         return points[i];
      }

      template <class T>
      const Point<T>* Triangle<T>::getNormal( )
      {
         if ( normal == 0 )
            refresh( );

         return normal;
      }
   }
}

#endif // __PRIMITIVES_TRIANGLE_H__