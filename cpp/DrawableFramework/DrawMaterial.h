#ifndef __DRAWMATERIAL_H__
#define __DRAWMATERIAL_H__

#include "ObjectManagement/ManagedObject.h"

#include "Mesh/MeshTypeDefs.h"

namespace AWT
{
   class DrawMaterial : public virtual ManagedObject
   {
   public:
      typedef ManagedAutoPointer<DrawMaterial> P;

      enum TweakType { TWEAK_VERTEX, TWEAK_FACE };

   public:
      virtual bool isOpaque( ) const = 0;

      virtual void prepare( ) = 0;

      virtual void tweak( const TweakType tw, const MeshIndex v ) = 0;

      virtual void unprepare( ) = 0;
   };
}

#endif // __DRAWMATERIAL_H__