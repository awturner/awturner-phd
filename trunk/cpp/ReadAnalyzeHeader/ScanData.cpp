#include "ScanData.h"
#include "AnalyzeHeader.h"

#include "Useful/Exception.h"

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include <fstream>

using namespace boost;

struct AWT::ScanData::D
{
   void load( )
   {
      if ( m_Loaded )
         return;

      if ( m_Data )
         delete m_Data;

      std::string imageFilename = m_Filename + ".img";

      std::ifstream fs( imageFilename.c_str( ), std::ios::binary );

      unsigned int npixels = m_Size[0]*m_Size[1]*m_Size[2];
      
      m_Data = new short[ npixels ];
      fs.read( (char*)m_Data, npixels*sizeof(short) );

      fs.close( );

      m_Loaded = true;
   }

   bool m_Loaded;
   short* m_Data;
   std::string m_Filename;

   unsigned int m_Size[3];
   float m_Spacing[3];
};

AWT::ScanData::ScanData( const std::string& filename )
{
   m_D = new D;

   m_D->m_Loaded = false;
   m_D->m_Data = 0;

   if ( iends_with( filename, ".hdr" ) || iends_with( filename, ".img" ) )
      m_D->m_Filename = filename.substr( 0, filename.length() - 4 );
   else
      m_D->m_Filename = filename;

   if ( !filesystem::exists( m_D->m_Filename + ".hdr" ) )
      AWTEXCEPTIONTHROW( "Could not find header file" );

   if ( !filesystem::exists( m_D->m_Filename + ".img" ) )
      AWTEXCEPTIONTHROW( "Could not find image file" );

   std::string headerFilename = m_D->m_Filename + ".hdr";
   
   dsr header;

   std::ifstream fs( headerFilename.c_str( ), std::ios::binary );
   fs.read( (char*)&header, sizeof(dsr) );
   fs.close( );

   m_D->m_Size[0] = header.dime.dim[1];
   m_D->m_Size[1] = header.dime.dim[2];
   m_D->m_Size[2] = header.dime.dim[3];

   m_D->m_Spacing[0] = header.dime.pixdim[1];
   m_D->m_Spacing[1] = header.dime.pixdim[2];
   m_D->m_Spacing[2] = header.dime.pixdim[3];
}

AWT::ScanData::~ScanData( )
{
   if ( m_D->m_Data )
      delete [] m_D->m_Data;

   delete m_D;
}

AWT::ScanData::P AWT::ScanData::getInstance( const std::string& filename )
{
   AUTOGETINSTANCE( AWT::ScanData, ( filename ) );
}

GETNAMEMACRO( AWT::ScanData );

bool AWT::ScanData::isLoaded( ) const
{
   return m_D->m_Loaded;
}

void AWT::ScanData::load( )
{
   m_D->load( );
}

unsigned int AWT::ScanData::getWidth( ) const
{
   return m_D->m_Size[0];
}

unsigned int AWT::ScanData::getHeight( ) const
{
   return m_D->m_Size[1];
}

unsigned int AWT::ScanData::getDepth( ) const
{
   return m_D->m_Size[1];
}

void AWT::ScanData::getSize( unsigned int size[3] ) const
{
   for ( unsigned int i = 0; i < 3; ++i )
      size[i] = m_D->m_Size[i];
}

void AWT::ScanData::getSpacing( float spacing[3] ) const
{
   for ( unsigned int i = 0; i < 3; ++i )
      spacing[i] = m_D->m_Spacing[i];
}

const short* AWT::ScanData::getData( ) const
{
   if ( !isLoaded() )
      AWTEXCEPTIONTHROW( "Need to load the data first!" );

   return m_D->m_Data;
}

void AWT::ScanData::convertCoordsWorldToScan( const float world[3], float scan[3] ) const
{
   for ( unsigned int i = 0; i < 3; ++i )
      scan[i] = world[i] / m_D->m_Spacing[i];
}

void AWT::ScanData::convertCoordsScanToWorld( const float scan[3], float world[3] ) const
{
   for ( unsigned int i = 0; i < 3; ++i )
      world[i] = scan[i] * m_D->m_Spacing[i];
}
