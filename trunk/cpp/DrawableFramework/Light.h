#ifndef __LIGHT_H__
#define __LIGHT_H__

#include "ObjectManagement/ManagedObject.h"

namespace AWT
{
   class Light : public virtual ManagedObject
   {
   public:
      typedef ManagedAutoPointer<Light> P;

   public:
      static P getInstance( );

      void setPosition( const float* position );
      void getPosition( float* out_Position ) const;

      void setAmbient( const float* ambient );
      void getAmbient( float* out_Ambient ) const;

      void setDiffuse( const float* diffuse );
      void getDiffuse( float* out_Diffuse ) const;

      void setSpecular( const float* specular );
      void getSpecular( float* out_Specular ) const;

      virtual std::string getClassName( ) const;

   protected:
      Light( );
      virtual ~Light( );

      float m_Position[4];
      float m_Ambient[4];
      float m_Diffuse[4];
      float m_Specular[4];
   };
}

#endif __LIGHT_H__