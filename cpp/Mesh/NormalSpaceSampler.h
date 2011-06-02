#ifndef __NORMALSPACESAMPLER_H__
#define __NORMALSPACESAMPLER_H__

#include "MeshSampler.h"

namespace AWT
{
   template <class T>
   class NormalSpaceSampler : public MeshSampler<T>
   {
   public:
      typedef ManagedAutoPointer<NormalSpaceSampler<T>> P;

   protected:
      NormalSpaceSampler( );
      virtual ~NormalSpaceSampler( );

   public:
      static P getInstance( );
      virtual std::string getClassName( ) const;

      virtual void setLatitudeDivisions( const unsigned int d );
      virtual unsigned int getLatitudeDivisions( ) const;

      virtual void setLongitudeDivisions( const unsigned int d );
      virtual unsigned int getLongitudeDivisions( ) const;

      virtual unsigned int getMaximumNumberOfSamples( ) const;

      virtual void setMaximumNumberOfSamples( const unsigned int s );

      virtual SamplePoints::P sampleMesh( typename Mesh<T>::P mesh );

   protected:
      struct D;
      D* m_D;

   };
}

#endif // __NORMALSPACESAMPLER_H__