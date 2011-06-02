#ifndef __DrawDeformableMeshFitting_H__
#define __DrawDeformableMeshFitting_H__


#include "DeformableMeshFitting/DeformableMeshFittingListener.h"

#include "DrawableFramework/Drawable.h"


namespace AWT
{
   template <class T>
   class DeformableMeshFitting;

   template <class T>
   class DrawableFactory;

   class DrawableAssembly;

   template <class T>
   class DrawDeformableMeshFitting : public Drawable/*, public DeformableMeshFittingListener<T>*/
   {
   public:
      typedef ManagedAutoPointer<DrawDeformableMeshFitting> P;

   protected:
      DrawDeformableMeshFitting( typename DeformableMeshFitting<T>::P dmf, typename DrawableFactory<T>::P fact );
      virtual ~DrawDeformableMeshFitting( );

   public:
      static  P getInstance( typename DeformableMeshFitting<T>::P dmf, typename DrawableFactory<T>::P fact );

      void draw( DrawContext::P context, const bool transparentPass );

      void setVisible( const bool v );

      bool isVisible( ) const;

      void getBounds( double* out_Bounds );

      virtual std::string getClassName( ) const;

      virtual void correspondencesFound( typename DeformableMeshFitting<T>::P dmf );

      virtual void setParent( DrawableAssembly* assm );
      virtual DrawableAssembly* getParent( );

   protected:
      struct D;
      D* m_D;
   };
}

#endif // __DrawDeformableMeshFitting_H__