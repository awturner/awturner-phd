#ifndef __DRAWABLENONRIGIDALIGNMENT_H__
#define __DRAWABLENONRIGIDALIGNMENT_H__

#include "DrawableFramework/Drawable.h"

namespace AWT
{
   template <class T>
   class NonRigidAlignment;

   class DrawableAssembly;

   template <class T>
   class DrawableNonRigidAlignment : public Drawable
   {
   public:
      typedef ManagedAutoPointer<DrawableNonRigidAlignment> P;

   protected:
      DrawableNonRigidAlignment( typename NonRigidAlignment<T>::P nra );
      virtual ~DrawableNonRigidAlignment( );

   public:
      static P getInstance( typename NonRigidAlignment<T>::P nra );

      virtual bool isVisible( ) const;

      virtual void setVisible( const bool v );

      virtual void getBounds( double* out_Bounds );

      virtual void draw( DrawContext::P context, const bool transparentPass );

      virtual std::string getClassName( ) const;

      virtual void fixedMeshSet( typename NonRigidAlignment<T>::P nra );

      virtual void movingMeshSet( typename NonRigidAlignment<T>::P nra );

      virtual void correspondencesFound( typename NonRigidAlignment<T>::P nra );

      virtual void rigidParameterUpdate( typename NonRigidAlignment<T>::P nra );

      virtual void nonRigidParameterUpdate( typename NonRigidAlignment<T>::P nra );

      virtual void controlPointsChanged( typename NonRigidAlignment<T>::P nra );

      virtual void alignmentStart( typename NonRigidAlignment<T>::P nra );

      virtual void alignmentComplete( typename NonRigidAlignment<T>::P nra );

      virtual void setParent( DrawableAssembly* assm );
      virtual DrawableAssembly* getParent( );

   protected:
      struct D;
      D* m_D;
   };
}

#endif // __DRAWABLENONRIGIDALIGNMENT_H__