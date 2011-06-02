#ifndef __PLANARCUT_H__
#define __PLANARCUT_H__

#include "CuttingFunction.h"

namespace AWT
{
   template <class T>
   class PlanarCut : public CuttingFunction<T>
   {
   public:
      typedef ManagedAutoPointer<PlanarCut<T>> P;

   protected:
      PlanarCut( );
      virtual ~PlanarCut( );

   public:
      static P getInstance( );
      virtual std::string getClassName( ) const;

      virtual void setPlane( const T* in_Plane );
      virtual void getPlane( T* out_Plane ) const;

      virtual bool accept( T* vtx );

   protected:
      struct D;
      D* m_D;

   };
}

#endif // __PLANARCUT_H__