#if 0
#ifndef __OPENGLDRAWABLEASSEMBLY_H__
#define __OPENGLDRAWABLEASSEMBLY_H__

#include "DrawableFramework/DrawableAssembly.h"

namespace AWT
{
   class OpenGLDrawableAssembly : public virtual DrawableAssembly
   {
   public:
      typedef ManagedAutoPointer<OpenGLDrawableAssembly> P;

   protected:
      OpenGLDrawableAssembly( );
      virtual ~OpenGLDrawableAssembly( );

   public:
      static P getInstance( );
      std::string getClassName( ) const;

      virtual void add( const std::string& name, Drawable::P drawable );
      virtual void remove( Drawable::P drawable );
      virtual void clear( );

      virtual void setVisible( const bool v );
      virtual bool isVisible( ) const;

      virtual void getBounds( double* out_Bounds );

      virtual void draw( DrawContext::P context, const bool transparentPass );

      virtual unsigned int getNumberOfChildren( ) const;
      virtual Drawable::P getChild( const unsigned int i );
      virtual std::string getChildName( const unsigned int i );

      virtual void setParent( DrawableAssembly* p );
      virtual DrawableAssembly* getParent( );

   protected:
      struct D;
      D* m_D;
   };
}

#endif // __OPENGLDRAWABLEASSEMBLY_H__
#endif