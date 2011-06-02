#ifndef __DRAWCONTEXT_H__
#define __DRAWCONTEXT_H__

#include "ObjectManagement/ManagedObject.h"

namespace AWT
{
   class DrawContext : public ManagedObject
   {
   public:
      typedef ManagedAutoPointer<DrawContext> P;

   public:
      virtual void getViewPosition( double* pos ) = 0;
      virtual void getMouseDirection( double* norm ) = 0;

      virtual bool isMouseOver( ) const = 0;

      virtual void makeCurrent( ) = 0;

      virtual void drawText( int x, int y, const std::string& text ) = 0;
      virtual void drawText( double x, double y, double z, const std::string& text ) = 0;
   };
}

#endif // __DRAWCONTEXT_H__