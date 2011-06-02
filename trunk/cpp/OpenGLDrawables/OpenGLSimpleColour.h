#ifndef __OPENGLSIMPLECOLOUR_H__
#define __OPENGLSIMPLECOLOUR_H__

#include "DrawableFramework/DrawColour.h"

namespace AWT
{
   class OpenGLSimpleColour : public DrawColour
   {
   public:
      typedef ManagedAutoPointer<OpenGLSimpleColour> P;

   protected:
      OpenGLSimpleColour( float r = 1.f , float g = 1.f , float b = 1.f , float a = 1.f, bool lighting = false );

      virtual ~OpenGLSimpleColour( );

   public:
      static P getInstance( float r = 1.f , float g = 1.f , float b = 1.f , float a = 1.f, bool lighting = false );

      virtual std::string getClassName( ) const;

      virtual bool isOpaque( ) const;

      virtual void prepare( );

      virtual void unprepare( );

      virtual void tweak( const TweakType tw, const MeshIndex v );

      void setColour( const float* col );

      void setColour( const float r, const float g, const float b, const float a = 1.0 );

      void getColour( float* col ) const;

      bool isLighting( ) const;

      void setLighting( const bool v );

   protected:
      struct D;
      D* m_D;
   };
}

#endif // __OPENGLSIMPLECOLOUR_H__