#ifndef __DRAWABLE_H__
#define __DRAWABLE_H__

#include "ObjectManagement/ManagedObject.h"
#include "DrawContext.h"

namespace AWT
{
   class Mutex;

   class DrawableAssembly;

   class DrawContext;

   class Drawable : public virtual ManagedObject
   {
   protected:
      Drawable( );
      virtual ~Drawable( );

   public:
      typedef ManagedAutoPointer<Drawable> P;

   public:
      virtual void draw( DrawContext::P context, const bool transparentPass ) = 0;

      virtual void setVisible( const bool v ) = 0;

      virtual bool isVisible( ) const = 0;

      virtual void getBounds( double* out_Bounds ) = 0;

      virtual void setParent( DrawableAssembly* p ) = 0;
      virtual DrawableAssembly* getParent( ) = 0;

      //static bool acquireMutexIfNotLocked( void* obj );
      //static void acquireMutex( void* obj );
      //static void releaseMutex( void* obj );

      virtual ModifiedTime getTimeObjectModified( );

      virtual void setTimeObject( ManagedObject::P obj );
      virtual ManagedObject::P getTimeObject( );

   protected:
      struct D;
      D* m_D;
      //static Mutex* ms_Mutex;
   };
}

#endif __DRAWABLE_H__