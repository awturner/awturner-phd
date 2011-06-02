#include "SegmentationModel.h"

#define NOMINMAX

#include "CImg.h"
#include "LuaPlus.h"

#include "Useful/RunLengthEncode.h"
#include "Useful/PrintMacros.h"

#include <fstream>

using namespace cimg_library;
using namespace LuaPlus;

struct SegmentationModel::D
{
   template <class T>
   void copyToBuffer( CImg<T>& img, const Coord slice, const SliceDirection dir, T* buffer )
   {
      switch ( dir )
      {
      case SLICEDIR_Z:
         {
            T* ptr = img.ptr( 0, 0, slice );

            const int numpix = img.dimx() * img.dimy();

            for ( int i = 0; i < numpix; ++i )
               *buffer++ = *ptr++;
         }

         return;
      }

      throw "Not implemented";
   }

   bool loaded;
   CImg<ImageType> scan;
   CImg<LabelType> segmentation;
   ImageType m_Min;
   ImageType m_Max;
};

SegmentationModel::SegmentationModel( const char* modelFile )
{
   m_D = new D;
   m_D->loaded = false;

   LuaStateOwner luaState;
   
   int parseStatus = luaState->DoFile( modelFile );

   PRINTVBL( parseStatus );

   if ( parseStatus )
      return;

   LuaObject objDimensions = luaState->GetGlobal( "dimensions" );
   
   LuaObject objSpacing = luaState->GetGlobal( "spacing" );

   const Coord dims[] = {
      objDimensions.GetByIndex( 1 ).GetInteger( ),
      objDimensions.GetByIndex( 2 ).GetInteger( ),
      objDimensions.GetByIndex( 3 ).GetInteger( ),
   };

   {
      m_D->scan = CImg<ImageType>( dims[0], dims[1], dims[2], 1 );

      std::ifstream fs( luaState->GetGlobal( "scan" ).GetString( ), std::ios::binary );
      if ( !fs.good( ) )
         return;

      fs.read( reinterpret_cast<char*>( m_D->scan.data ), dims[0]*dims[1]*dims[2]*sizeof(ImageType) );
   }

   LuaObject objSeg = luaState->GetGlobal( "segmentation" );

   if ( !objSeg.IsNil( ) )
   {
      std::ifstream fs( luaState->GetGlobal( "segmentation" ).GetString( ), std::ios::binary );

      m_D->segmentation = runLengthDecode<LabelType>( fs );
   }
   else
   {
      m_D->segmentation.resize( dims[0], dims[1], dims[2] );
      m_D->segmentation.fill( 0 );
   }

   // Cache these, they don't change
   m_D->m_Min = m_D->scan.min( );
   m_D->m_Max = m_D->scan.max( );

   PRINTVEC( dims, 3 );
   DEBUGMACRO( "Finished loading" );
   m_D->loaded = true;
}

SegmentationModel::~SegmentationModel( )
{
   delete m_D;
}

const bool SegmentationModel::loaded() const
{
   return m_D->loaded;
}

void SegmentationModel::getScanSlice( const Coord slice, const SliceDirection dir, ImageType* buffer ) const
{
   m_D->copyToBuffer( m_D->scan, slice, dir, buffer );
}

void SegmentationModel::getSegmentationSlice( const Coord slice, const SliceDirection dir, LabelType* buffer ) const
{
   m_D->copyToBuffer( m_D->segmentation, slice, dir, buffer );}

const ImageType SegmentationModel::getScan( const Coord x, const Coord y, const Coord z )
{
   return m_D->scan( x, y, z );
}

const LabelType SegmentationModel::getSegmentation( const Coord x, const Coord y, const Coord z )
{
   return m_D->segmentation( x, y, z );
}

void SegmentationModel::setSegmentation( const Coord x, const Coord y, const Coord z, const LabelType seg )
{
   m_D->segmentation( x, y, z ) = seg;
}

void SegmentationModel::getScanAndSegmentation( const Coord x, const Coord y, const Coord z, ImageType& im, LabelType& seg )
{
   im = m_D->scan( x, y, z );
   seg = m_D->segmentation( x, y, z );
}

const Coord SegmentationModel::getWidth( ) const
{
   return m_D->scan.dimx();
}

const Coord SegmentationModel::getHeight( ) const
{
   return m_D->scan.dimy();
}

const Coord SegmentationModel::getDepth( ) const
{
   return m_D->scan.dimz();
}

const ImageType SegmentationModel::getMin( ) const
{
   return m_D->m_Min;
}

const ImageType SegmentationModel::getMax( ) const
{
   return m_D->m_Max;
}