#ifndef __OPENGLCOLOURMAPPER_H__
#define __OPENGLCOLOURMAPPER_H__

#include "DrawableFramework/ColourMapper.h"

namespace AWT
{
   template <class T>
   class OpenGLColourMapper : public ColourMapper<T>
   {
   public:
      typedef ManagedAutoPointer<OpenGLColourMapper<T>> P;

   protected:
      OpenGLColourMapper( );
      virtual ~OpenGLColourMapper( );

   public:
      static P getInstance( );
      virtual std::string getClassName( ) const;

      virtual bool isOpaque( ) const;

      virtual void prepare( );

      virtual void unprepare( );

      virtual void tweak( const TweakType tw, const MeshIndex v );

   protected:
      struct D;
      D* m_D;

   };
}

#endif // __OPENGLCOLOURMAPPER_H__