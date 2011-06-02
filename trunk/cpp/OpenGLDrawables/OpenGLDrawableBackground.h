#ifndef __OPENGLDRAWABLEBACKGROUND_H__
#define __OPENGLDRAWABLEBACKGROUND_H__

#include "DrawableFramework/DrawableBackground.h"
#include "OpenGLDrawDisplayList.h"

namespace AWT
{
   class DrawMaterial;

   class OpenGLDrawableBackground : public virtual OpenGLDrawDisplayList
   {
   public:
      typedef ManagedAutoPointer<OpenGLDrawableBackground> P;

   protected:
      OpenGLDrawableBackground( );
      virtual ~OpenGLDrawableBackground( );

   public:
      static P getInstance( );

      virtual std::string getClassName( ) const;

      virtual void getTopColor( float* col ) const;

      virtual void setTopColor( const float* col );

      virtual void getBottomColor( float* col ) const;

      virtual void setBottomColor( const float* col );

      virtual DrawMaterial::P getMaterial( );
      virtual void setMaterial( DrawMaterial::P mat );

   protected:
      struct D;
      D* m_D;

      virtual ModifiedTime getChildModifiedTime( );

      virtual void buildList( AWT::DrawContext::P context );

      virtual void updateBounds( );

      virtual double getBoundImpl( unsigned int bound );
   };
}

#endif // __OPENGLDRAWABLEBACKGROUND_H__