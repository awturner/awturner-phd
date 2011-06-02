#ifndef __DRAWCOLOUR_H__
#define __DRAWCOLOUR_H__

#include "DrawMaterial.h"

namespace AWT
{
   class DrawColour : public DrawMaterial
   {
   public:
      typedef ManagedAutoPointer<DrawColour> P;

   public:
      virtual void setColour( const float* rgba ) = 0;
      virtual void getColour( float* out_Rgba ) const = 0;

      virtual bool isLighting( ) const = 0;
      virtual void setLighting( const bool v ) = 0;
   };
}

#endif // __DRAWCOLOUR_H__