#ifndef __DRAWABLEASSEMBLY_H__
#define __DRAWABLEASSEMBLY_H__

#include "Drawable.h"

namespace AWT
{
   class DrawableAssembly : public Drawable
   {
   public:
      typedef ManagedAutoPointer<DrawableAssembly> P;

   protected:
      DrawableAssembly( );
      virtual ~DrawableAssembly( );
   public:
      static P getInstance( );
      std::string getClassName( ) const;

      virtual void draw( DrawContext::P context, const bool transparentPass );

      virtual void setVisible( const bool v );

      virtual bool isVisible( ) const;

      virtual void getBounds( double* out_Bounds );

      virtual void setParent( DrawableAssembly* p );
      virtual DrawableAssembly* getParent( );

      virtual void add( const std::string& name, Drawable::P drawable );
      virtual void remove( Drawable::P drawable );
      virtual void clear( );

      virtual unsigned int getNumberOfChildren( ) const;

      virtual Drawable::P getChild( const unsigned int i );
      virtual std::string getChildName( const unsigned int i );

   protected:
      struct D;
      D* m_D;
   };
}

#endif // __DRAWABLEASSEMBLY_H__