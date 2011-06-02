#ifndef __OPENGLTRANSFORMATION_H__
#define __OPENGLTRANSFORMATION_H__

#include "DrawableFramework/Transformation.h"

namespace AWT
{
   template <class T>
   class OpenGLTransformation : public Transformation<T>
   {
   public:
      typedef ManagedAutoPointer<OpenGLTransformation> P;

   protected:
      OpenGLTransformation( Drawable::P d, typename Pose<T>::P pose = 0 );

      virtual ~OpenGLTransformation( );

   public:
      static P getInstance( Drawable::P d, typename Pose<T>::P pose = 0 );

      virtual void draw( DrawContext::P context, const bool transparentPass );

      virtual void setVisible( const bool v );

      virtual bool isVisible( ) const;

      virtual void getBounds( double* out_Bounds );

      virtual void setDrawable( Drawable::P d );

      virtual Drawable::P getDrawable( );

      virtual void setPose( typename Pose<T>::P pose );

      virtual typename Pose<T>::P getPose( );

      virtual std::string getClassName( ) const;

      virtual void setParent( DrawableAssembly* p );

      virtual DrawableAssembly* getParent( );

   protected:
      struct D;
      D* m_D;
   };
}

#endif // __OPENGLTRANSFORMATION_H__