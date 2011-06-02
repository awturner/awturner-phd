#ifndef __SPHERICALCUT_H__
#define __SPHERICALCUT_H__

#include "CuttingFunction.h"

namespace AWT
{
   template <class T>
   class SphericalCut : public CuttingFunction<T>
   {
   public:
      typedef ManagedAutoPointer<SphericalCut<T>> P;

   protected:
      SphericalCut( );
      virtual ~SphericalCut( );

   public:
      static P getInstance( );
      virtual std::string getClassName( ) const;

      virtual void getCentre( T* out_Centre ) const;
      virtual void setCentre( const T* in_Centre );

      virtual T getRadius( ) const;
      virtual void setRadius( const T in_Radius );

      virtual bool accept( T* vtx );

   protected:
      struct D;
      D* m_D;

   };
}

#endif // __SPHERICALCUT_H__