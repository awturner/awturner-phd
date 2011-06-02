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
#ifndef __PRIMITIVE_GEOMETRICPRIMITIVE_H__
#define __PRIMITIVE_GEOMETRICPRIMITIVE_H__

#include "Useful/Exception.h"
#include "ObjectManagement/ReferenceCountedObject.h"
#include "ObjectManagement/ModifiedTimeObject.h"

#include <limits>

namespace AWT
{
   typedef unsigned long long ModifiedTime;

   namespace GeometricPrimitives
   {
      template <class T>
      class GeometricPrimitive : public ReferenceCountedObject, public ModifiedTimeObject<T>
      {
      public:
         static bool checkValue( const T& in_Val );
         static bool checkVector( const T* in_Vec, unsigned int N );

         virtual void refresh( );

      protected:
         GeometricPrimitive();
      };
   }
}

template <class T>
AWT::GeometricPrimitives::GeometricPrimitive<T>::GeometricPrimitive()
{
   modified();
}

template <class T>
void AWT::GeometricPrimitives::GeometricPrimitive<T>::refresh()
{
}

template <class T>
bool AWT::GeometricPrimitives::GeometricPrimitive<T>::checkValue(const T &in_Val)
{
   return ( in_Val == in_Val ) && ( abs(in_Val) != std::numeric_limits<T>::infinity() );
}

template <class T>
bool AWT::GeometricPrimitives::GeometricPrimitive<T>::checkVector( const T* in_Vec, unsigned int N )
{
   for (  unsigned int i = 0; i < N; ++i )
      if ( !checkValue(in_Vec[i]) )
         return false;

   return true;
}

#endif // __PRIMITIVE_GEOMETRICPRIMITIVE_H__