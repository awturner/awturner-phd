#ifndef __OPENGLDRAWAXIS_H__
#define __OPENGLDRAWAXIS_H__

#include "DrawableFramework/DrawAxis.h"

namespace AWT
{
   class OpenGLDrawAxis : public DrawAxis
   {
   public:
      typedef ManagedAutoPointer<OpenGLDrawAxis> P;

   protected:
      OpenGLDrawAxis( double scale );
      virtual ~OpenGLDrawAxis( );

   public:
      static P getInstance( double scale = 1 );
      virtual std::string getClassName( ) const;

      virtual void draw( DrawContext::P context, const bool transparentPass );

      virtual void setVisible( const bool v );

      virtual bool isVisible( ) const;

      virtual void getBounds( double* out_Bounds );

      virtual void setParent( DrawableAssembly* p );
      virtual DrawableAssembly* getParent( );

   protected:
      struct D;
      D* m_D;

   };
}

#endif // __OPENGLDRAWAXIS_H__