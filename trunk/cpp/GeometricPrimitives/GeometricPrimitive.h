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