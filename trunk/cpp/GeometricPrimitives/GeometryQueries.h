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
#ifndef __PRIMITIVES_QUERIES_H__
#define __PRIMITIVES_QUERIES_H__

#include "Point.h"

namespace AWT
{
   namespace GeometricPrimitives
   {
      template <class T>
      class Point;

      template <class T>
      class Line;

      template <class T>
      class Sphere;

      template <class T>
      class Plane;

      template <class T>
      class GeometryQueries
      {
      public:
         static T    distanceSquared( const Point<T>* a, const Point<T>* b );
         static void direction( const Point<T>* a, const Point<T>* b, Point<T>* dir );

         static bool findNearest( Point<T>* a, Line<T>* el,       Point<T>* nearest );
         static bool findNearest( Point<T>* a, Sphere<T>* sphere, Point<T>* nearest );
         static bool findNearest( Point<T>* a, Plane<T>* plane,   Point<T>* nearest );
      };
   }
}


namespace AWT
{
   namespace GeometricPrimitives
   {
      template <class T>
      T GeometryQueries<T>::distanceSquared( const Point<T>* a, const Point<T>* b )
      {
         T ret = 0;
         T tmp;

         if ( a->p[3] == 0 || b->p[3] == 0 )
            return std::numeric_limits<T>::infinity();

         for ( int i = 0; i < 3; ++i )
         {
            tmp = b->p[0]/b->p[3] - a->p[0]/a->p[3];
            ret += tmp*tmp;
         }

         return ret;
      }

      template <class T>
      void GeometryQueries<T>::direction( const Point<T>* a, const Point<T>* b, Point<T>* dir )
      {
         const T* p[] = { &(a->p[0]), &(b->p[0]) };

         if ( p[0][3] == 0 && p[1][3] == 0 )
         {
            dir->setPosition(
               p[1][0] - p[0][0],
               p[1][1] - p[0][1],
               p[1][2] - p[0][2],
               0
            );
         }
         else if ( p[1][3] == 0 )
         {
            dir->setPosition( b );
         }
         else if ( p[0][3] == 0 )
         {
            dir->setPosition( -p[0][0], -p[0][1], -p[0][2], -p[0][3] );
         }
         else
         {
            dir->setPosition(
               p[1][0]/p[1][3] - p[0][0]/p[0][3],
               p[1][1]/p[1][3] - p[0][1]/p[0][3],
               p[1][2]/p[1][3] - p[0][2]/p[0][3],
               0
            );
         }
      }

      template <class T>
      bool GeometryQueries<T>::findNearest( Point<T>* a, Line<T>* el, Point<T>* nearest )
      {
         a->registerUse();
         el->registerUse();
         nearest->registerUse();

         if ( el->points[0]->p[3] == 0 && el->points[1]->p[3] == 0 )
            // Both line end points are at infinity
            return false;

         // Pick one of the endpoints as the origin
         int originIdx = ( el->points[0]->p[3] != 0 ) ? 0 : 1;
         int dirIdx    = 1 - originIdx;

         // Calculate the direction to the other one
         Point<T>* dir = Point<T>::getInstance();

         direction( el->points[originIdx], el->points[dirIdx], dir );

         T normOrigin[4];
         normOrigin[0] = el->points[originIdx]->p[0]/el->points[originIdx]->p[3];
         normOrigin[1] = el->points[originIdx]->p[1]/el->points[originIdx]->p[3];
         normOrigin[2] = el->points[originIdx]->p[2]/el->points[originIdx]->p[3];
         normOrigin[3] = 1;
         
         // Calculate the different between a and the line origin
         T r[] = {
            a->p[0]/a->p[3] - normOrigin[0],
            a->p[1]/a->p[3] - normOrigin[1],
            a->p[2]/a->p[3] - normOrigin[2],
            0
         };

         // Get the pointer to the direction vector
         T* n = &(dir->p[0]);

         T num = - ( r[0]*n[0] + r[1]*n[1] + r[2]*n[2] );
         T den = n[0]*n[0] + n[1]*n[1] + n[2]*n[2];

         T lambda = num / den;

         if ( el->isBounded() )
         {
            if ( lambda < 0 )
            {
               lambda = 0;
            }
            else if ( el->points[dirIdx]->p[3] != 0 )
            {
               // Need to calculate the length of the line segment
               r[0] = el->points[dirIdx]->p[0]/el->points[dirIdx]->p[3] - normOrigin[0];
               r[1] = el->points[dirIdx]->p[1]/el->points[dirIdx]->p[3] - normOrigin[1];
               r[2] = el->points[dirIdx]->p[2]/el->points[dirIdx]->p[3] - normOrigin[2];

               T lenSq = r[0]*r[0] + r[1]*r[1] + r[2]*r[2];

               // Delay the square rooting until the last moment
               if ( lenSq > lambda*lambda )
               {
                  lambda = sqrt( lenSq );
               }
            }
         }

         nearest->setPosition(
            normOrigin[0] + lambda*n[0],
            normOrigin[1] + lambda*n[1],
            normOrigin[2] + lambda*n[2],
            1
         );

         a->unregisterUse();
         el->unregisterUse();
         nearest->unregisterUse();
         dir->unregisterUse();

         return true;
      }

      template <class T>
      bool GeometryQueries<T>::findNearest( Point<T>* a, Sphere<T>* sphere, Point<T>* nearest )
      {
         a->registerUse();
         sphere->registerUse();
         nearest->registerUse();

         Point<T>* center = Point<T>::getInstance( sphere->getCenter() );
         
         T r[3];
         T len = 0;
         for ( int i = 0; i < 3; ++i )
         {
            r[i] = (*center)[i] - (*a)[i];
            len += r[i]*r[i];
         }
         len = sqrt( len );

         nearest->setPosition(
            (*a)[0] + r[0] * sphere->getRadius() / len,
            (*a)[1] + r[1] * sphere->getRadius() / len,
            (*a)[2] + r[2] * sphere->getRadius() / len
         );
         
         a->unregisterUse();
         sphere->unregisterUse();
         center->unregisterUse();
         nearest->unregisterUse();

         return true;
      }

      template <class T>
      bool GeometryQueries<T>::findNearest( Point<T>* a, Plane<T>* plane, Point<T>* nearest )
      {
         a->registerUse();
         plane->registerUse();
         nearest = Point<T>::getInstance();
         
         T num = 0;
         T den = 0;

         for ( int i = 0; i < 4; ++i )
         {
            num -= (*plane)[i] * (*a)[i];
            den -= (*plane)[i] * (*plane)[i];
         }

         num /= den;

         for ( int i = 0; i < 3; ++i )
            nearest->p[i] = a->p[i] + num * (*plane)[i];

         a->unregisterUse();
         plane->unregisterUse();
         nearest->unregisterUse();

         return true;
      }
   }
}

#endif // __PRIMITIVES_QUERIES_H__