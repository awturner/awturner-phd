#ifndef __PRIMITIVES_SPHERE_H__
#define __PRIMITIVES_SPHERE_H__

#include "GeometricPrimitive.h"

#include "Point.h"

namespace AWT
{
   namespace GeometricPrimitives
   {
      template <class T>
      class Sphere : public GeometricPrimitive<T>
      {
         friend class GeometryQueries<T>;

      public:
         static Sphere* getInstance( Point<T>* in_p, const T in_radius );
         
         void getCenter( Point<T>* out_p ) const;
         const Point<T>* getCenter( ) const;

         T getRadius( ) const;

         void setCenter( Point<T>* in_p );

         void setRadius( const T in_radius );

         virtual ModifiedTime getModifiedTime() const;

      protected:
         Sphere( Point<T>* in_p, const T in_radius );
         virtual ~Sphere( );

         Point<T>* center;
         T radius;
      };
   }
}

namespace AWT
{
   namespace GeometricPrimitives
   {
      template <class T>
      Sphere<T>* Sphere<T>::getInstance( Point<T>* in_p, const T in_radius )
      {
         return new Sphere<T>( in_p, in_radius );
      }

      template <class T>
      Sphere<T>::~Sphere()
      {
         center->unregisterUse();
      }

      template <class T>
      Sphere<T>::Sphere( Point<T>* in_p, const T in_radius )
      {
         center = 0;
         setCenter( in_p );

         radius = in_radius;
      }
      
      template <class T>
      ModifiedTime Sphere<T>::getModifiedTime( ) const
      {
         return std::max<ModifiedTime>( this->mtime, center->getModifiedTime() );
      }

      template <class T>
      const Point<T>* Sphere<T>::getCenter( ) const
      {
         return center;
      }

      template <class T>
      void Sphere<T>::getCenter( Point<T>* out_p ) const
      {
         *out_p = *center;
      }

      template <class T>
      T Sphere<T>::getRadius() const
      {
         return radius;
      }

      template <class T>
      void Sphere<T>::setCenter( Point<T>* in_p )
      {
         if ( center != 0 )
            center->unregisterUse();

         center = in_p;
         in_p->registerUse();

         modified();
      }

      template <class T>
      void Sphere<T>::setRadius( const T in_radius )
      {
         if ( radius != in_radius )
         {
            radius = in_radius;
            modified();
         }
      }

   }
}

#endif // __PRIMITIVES_SPHERE_H__