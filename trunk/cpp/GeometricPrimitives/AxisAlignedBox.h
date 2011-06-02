#ifndef __PRIMITIVES_AXISALIGNEDBOX_H__
#define __PRIMITIVES_AXISALIGNEDBOX_H__

#include "GeometricPrimitive.h"

namespace AWT
{
   namespace GeometricPrimitives
   {
      template <class T>
      class AxisAlignedBox : public GeometricPrimitive<T>
      {
         friend class GeometryQueries<T>;

      public:
         static AxisAlignedBox* getInstance( const T* in_bounds );

         void getBounds( T* out_bounds ) const;
         void setBounds( const T* in_bounds );
         
         T getMinimumBound( const unsigned int axis ) const;
         T getMaximumBound( const unsigned int axis ) const;

         void setMinimumBound( const unsigned int axis, T value );
         void setMaximumBound( const unsigned int axis, T value );

      protected:
         AxisAlignedBox( const T* in_bounds );
         virtual ~AxisAlignedBox( );

         T bounds[6];
      };
   }
}

namespace AWT
{
   namespace GeometricPrimitives
   {
      template <class T>
      AxisAlignedBox<T>* AxisAlignedBox<T>::getInstance( const T* in_bounds )
      {
         return new AxisAlignedBox( in_bounds );
      }

      template <class T>
      void AxisAlignedBox<T>::getBounds( T* out_bounds ) const
      {
         for ( int i = 0; i < 6; ++i)
            out_bounds[i] = bounds[i];
      }

      template <class T>
      void AxisAlignedBox<T>::setBounds(const T in_bounds[])
      {
         for ( int i = 0; i < 6; ++i)
            bounds[i] = in_bounds[i];

      }

      template <class T>
      T AxisAlignedBox<T>::getMinimumBound( const unsigned int axis ) const
      {
         return bounds[3*axis+0];
      }

      template <class T>
      T AxisAlignedBox<T>::getMaximumBound( const unsigned int axis ) const
      {
         return bounds[3*axis+1];
      }

      template <class T>
      void AxisAlignedBox<T>::setMinimumBound( const unsigned int axis, T value )
      {
         bounds[3*axis+0] = value;
         modified();
      }

      template <class T>
      void AxisAlignedBox<T>::setMaximumBound( const unsigned int axis, T value )
      {
         bounds[3*axis+1] = value;
         modified();
      }

      template <class T>
      AxisAlignedBox<T>::AxisAlignedBox( const T* in_bounds )
      {
         for ( int i = 0; i < 6; ++i )
            bounds[i] = in_bounds[i];
      }

      template <class T>
      AxisAlignedBox<T>::~AxisAlignedBox( )
      {
      }
   }
}
#endif // __PRIMITIVES_AXISALIGNEDBOX_H__