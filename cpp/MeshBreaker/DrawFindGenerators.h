#ifndef __DRAWFINDGENERATORS_H__
#define __DRAWFINDGENERATORS_H__

#include "ObjectManagement/ManagedObject.h"

#include "OpenGLDrawables/OpenGLDrawableImpl.h"

#include "FindGenerators.h"

namespace AWT
{
   template <class T>
   class DrawFindGenerators : public OpenGLDrawableImpl
   {
   public:
      typedef ManagedAutoPointer<DrawFindGenerators> P;

   protected:
      DrawFindGenerators( typename FindGenerators<T>::P mb );
      virtual ~DrawFindGenerators( );

   public:
      static P getInstance( typename FindGenerators<T>::P mb  );
      virtual std::string getClassName( ) const;

      virtual DrawMaterial::P getMaterial( );
      virtual void setMaterial( DrawMaterial::P mat );

      virtual void getBounds( double* bounds );

   protected:
      virtual void drawImpl( DrawContext::P context );

      struct D;
      D* m_D;

   };
}

#endif // __DRAWFINDGENERATORS_H__