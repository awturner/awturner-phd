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
#ifndef __PRIMITIVES_LINE_H__
#define __PRIMITIVES_LINE_H__

#include "GeometricPrimitive.h"
#include "Point.h"

namespace AWT
{
   namespace GeometricPrimitives
   {
      template <class T>
      class Line : public GeometricPrimitive<T>
      {
         friend class GeometryQueries<T>;

      public:
         static Line* getInstance( Point<T>* in_a, Point<T>* in_b, const bool in_bounded );

         const Point<T>* getPoint1( ) const;
         void            getPoint1( Point<T>* out_p ) const;

         const Point<T>* getPoint2( ) const;
         void            getPoint2( Point<T>* out_p ) const;

         bool isBounded( ) const;

         virtual ModifiedTime getModifiedTime() const;

      protected:
         Line( Point<T>* in_origin, Point<T>* in_direction, const bool in_bounded );
         virtual ~Line( );

         Point<T>* points[2];
         
         const bool bounded;
      };
   }
}

namespace AWT
{
   namespace GeometricPrimitives
   {
      template <class T>
      Line<T>* Line<T>::getInstance( Point<T>* in_origin, Point<T>* in_direction, const bool in_bounded )
      {
         return new Line<T>( in_origin, in_direction, in_bounded );
      }

      template <class T>
      Line<T>::Line( Point<T>* in_a, Point<T>* in_b, const bool in_bounded )
         : bounded( in_bounded )
      {
         // Make sure that these don't get deleted
         in_a->registerUse();
         in_b->registerUse();

         points[0] = in_a;
         points[1] = in_b;
      }

      template <class T>
      Line<T>::~Line()
      {
         // They can be deleted hereafter
         points[0]->unregisterUse();
         points[1]->unregisterUse();
      }

      template <class T>
      ModifiedTime Line<T>::getModifiedTime( ) const
      {
         ModifiedTime ret = this->mtime;

         ret = std::max<ModifiedTime>( ret, points[0]->getModifiedTime() );
         ret = std::max<ModifiedTime>( ret, points[1]->getModifiedTime() );

         return ret;
      }

      template <class T>
      bool Line<T>::isBounded() const
      {
         return bounded;
      }

      template <class T>
      const Point<T>* Line<T>::getPoint1() const
      {
         return points[0];
      }

      template <class T>
      void Line<T>::getPoint1( Point<T>* out_p ) const
      {
         out_p = points[0];
      }

      template <class T>
      const Point<T>* Line<T>::getPoint2() const
      {
         return points[1];
      }

      template <class T>
      void Line<T>::getPoint2( Point<T>* out_p ) const
      {
         out_p = points[1];
      }
   }
}

#endif // __PRIMITIVES_LINE_H__