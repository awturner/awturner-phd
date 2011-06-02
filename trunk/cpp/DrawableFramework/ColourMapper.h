#ifndef __COLOURMAPPER_H__
#define __COLOURMAPPER_H__

#include "DrawMaterial.h"

#include "Mesh/Tuples.h"

namespace AWT
{
   template <class T>
   class ColourMapper : public DrawMaterial
   {
   public:
      typedef ManagedAutoPointer<ColourMapper<T>> P;

   protected:
      ColourMapper( );
      virtual ~ColourMapper( );

   public:
      void setData( typename Tuples<T>::P data );
      typename Tuples<T>::P getData( );

      // This is used in the bias/scaling
      void setMin( T v );
      T getMin( ) const;

      // This is used in the bias/scaling
      void setMax( T v );
      T getMax( ) const;

      // This needs to be implemented in an API-specific way
      virtual void tweak( const TweakType tw, const MeshIndex v ) = 0;

   protected:
      struct D;
      D* m_D;

   };
}

#endif // __COLOURMAPPER_H__