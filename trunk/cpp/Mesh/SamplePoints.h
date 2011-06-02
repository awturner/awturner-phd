#ifndef __SAMPLEPOINTS_H__
#define __SAMPLEPOINTS_H__

#include "ObjectManagement/ManagedObject.h"
#include "MeshTypeDefs.h"

namespace AWT
{
   class SamplePoints : public virtual ManagedObject
   {
   public:
      typedef ManagedAutoPointer<SamplePoints> P;

   protected:
      SamplePoints( MeshIndex npoints = 0 );
   
      virtual ~SamplePoints( );

   public:
      static P getInstance( MeshIndex npoints = 0 );
      
      const MeshIndex operator[]( MeshIndex i ) const;

      MeshIndex getNumberOfSamples( ) const;

      MeshIndex getSampleIndex( MeshIndex i ) const;

      void ensureSize( MeshIndex i );

      void add( MeshIndex i );

      void clear( );

      virtual std::string getClassName( ) const;

   protected:
      struct D;
      D* m_D;
   };
}

#endif // __SAMPLEPOINTS_H__
