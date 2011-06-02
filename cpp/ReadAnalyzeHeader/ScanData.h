#ifndef __SCANDATA_H__
#define __SCANDATA_H__

#include "ObjectManagement/ManagedObject.h"

namespace AWT
{
   class ScanData : public ManagedObject
   {
   public:
      typedef ManagedAutoPointer<ScanData> P;

   protected:
      ScanData( const std::string& filename );
      virtual ~ScanData( );

   public:
      static P getInstance( const std::string& filename );
      virtual std::string getClassName( ) const;

      bool isLoaded( ) const;
      void load( );

      unsigned int getWidth( ) const;
      unsigned int getHeight( ) const;
      unsigned int getDepth( ) const;

      void convertCoordsWorldToScan( const float world[3], float scan[3] ) const;
      void convertCoordsScanToWorld( const float scan[3], float world[3] ) const;

      void getSize( unsigned int size[3] ) const;
      void getSpacing( float spacing[3] ) const;

      const short* getData( ) const;

   protected:
      struct D;
      D* m_D;

   };
}

#endif // __SCANDATA_H__