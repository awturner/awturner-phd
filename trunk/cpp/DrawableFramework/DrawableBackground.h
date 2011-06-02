#ifndef __DRAWABLEBACKGROUND_H__
#define __DRAWABLEBACKGROUND_H__

#include "Drawable.h"

namespace AWT
{
   class DrawableBackground : public virtual Drawable
   {
   public:
      typedef ManagedAutoPointer<DrawableBackground> P;

   protected:
      DrawableBackground( );

      virtual ~DrawableBackground( );

   public:

      virtual std::string getClassName( ) const;

      virtual bool isVisible( ) const;

      virtual void setVisible( const bool vis );

      virtual void getTopColor( float* col ) const;

      virtual void setTopColor( const float* col );

      virtual void getBottomColor( float* col ) const;

      virtual void setBottomColor( const float* col );

      virtual void getBounds( double* out_Bounds );

   protected:
      struct D;
      D* m_D;
   };
}

#endif // __DRAWABLEBACKGROUND_H__