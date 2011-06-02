#ifndef __DRAWABLECONTROLPOINTS_H__
#define __DRAWABLECONTROLPOINTS_H__

#include "NonRigidAlignment/ControlPoint.h"
#include "NonRigidAlignment/ControlPointSet.h"

#include "OpenGLDrawableImpl.h"
#include "OpenGLHeaders.h"


namespace AWT
{
   class DrawMaterial;

   template <class T>
   class DrawableControlPoints : public OpenGLDrawableImpl
   {
   public:
      typedef ManagedAutoPointer<DrawableControlPoints> P;

   protected:
      DrawableControlPoints( typename ControlPointSet<T>::P cps );

      ~DrawableControlPoints( );

   public:
      static P getInstance( typename ControlPointSet<T>::P cps );

      virtual std::string getClassName( ) const;

      virtual void getBounds( double* out_Bounds );

      virtual void drawImpl( DrawContext::P context );

      virtual void setDrawThreshold( const T in_Val );

      virtual T getDrawThreshold( ) const;

      virtual DrawMaterial::P getMaterial( );
      virtual void setMaterial( DrawMaterial::P mat );

   protected:
      struct D;
      D* m_D;
   };
}

#endif // __DRAWABLECONTROLPOINTS_H__