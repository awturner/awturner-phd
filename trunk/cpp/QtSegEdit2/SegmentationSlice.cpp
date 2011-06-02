#include "SegmentationSlice.h"

#include <exception>

#include "Useful/PrintMacros.h"

SegmentationSlice::SegmentationSlice( SegmentationModel* model, const Coord slice, const SliceDirection dir )
: m_Slice( slice ), m_Dir( dir ), m_Model( model )
{
   if ( slice < 0 )
      throw std::exception( "Slice index out of range" );

   bool invalid = false;
   switch ( m_Dir )
   {
   case SLICEDIR_X:
      if ( slice >= model->getWidth( ) )
         invalid = true;
      break;
   case SLICEDIR_Y:
      if ( slice >= model->getHeight( ) )
         invalid = true;
      break;
   case SLICEDIR_Z:
      if ( slice >= model->getDepth( ) )
         invalid = true;
      break;
   }

   if ( invalid )
      throw std::exception( "Slice index out of range" );
}

SegmentationSlice::~SegmentationSlice( )
{

}

ImageType SegmentationSlice::getMin( ) const
{
   return m_Model->getMin( );
}

ImageType SegmentationSlice::getMax( ) const
{
   return m_Model->getMax( );
}

Coord SegmentationSlice::getWidth( ) const
{
   switch ( m_Dir )
   {
   case SLICEDIR_X:
      return m_Model->getDepth( );
   case SLICEDIR_Y:
      return m_Model->getWidth( );
   case SLICEDIR_Z:
      return m_Model->getWidth( );
   }

   throw "Invalid slice direction";
}

Coord SegmentationSlice::getHeight( ) const
{
   switch ( m_Dir )
   {
   case SLICEDIR_X:
      return m_Model->getHeight( );
   case SLICEDIR_Y:
      return m_Model->getDepth( );
   case SLICEDIR_Z:
      return m_Model->getHeight( );
   }

   throw "Invalid slice direction";
}

void SegmentationSlice::getScan( ImageType* buffer ) const
{
   m_Model->getScanSlice( m_Slice, m_Dir, buffer );
}

void SegmentationSlice::getSegmentation( LabelType* buffer ) const
{
   m_Model->getSegmentationSlice( m_Slice, m_Dir, buffer );
}

ImageType SegmentationSlice::getScan( const Coord x, const Coord y ) const
{
   switch ( m_Dir )
   {
   case SLICEDIR_X:
      return m_Model->getScan( m_Slice, y, x );
   case SLICEDIR_Y:
      return m_Model->getScan( x, m_Slice, y );
   case SLICEDIR_Z:
      return m_Model->getScan( x, y, m_Slice );
   }

   throw "Invalid slice direction";
}

LabelType SegmentationSlice::getSegmentation( const Coord x, const Coord y ) const
{
   switch ( m_Dir )
   {
   case SLICEDIR_X:
      return m_Model->getSegmentation( m_Slice, y, x );
   case SLICEDIR_Y:
      return m_Model->getSegmentation( x, m_Slice, y );
   case SLICEDIR_Z:
      return m_Model->getSegmentation( x, y, m_Slice );
   }

   throw "Invalid slice direction";
}

void SegmentationSlice::getScanAndSegmentation( const Coord x, const Coord y, ImageType& im, LabelType& seg )
{
   switch ( m_Dir )
   {
   case SLICEDIR_X:
      return m_Model->getScanAndSegmentation( m_Slice, y, x, im, seg );
   case SLICEDIR_Y:
      return m_Model->getScanAndSegmentation( x, m_Slice, y, im, seg );
   case SLICEDIR_Z:
      return m_Model->getScanAndSegmentation( x, y, m_Slice, im, seg );
   }

   throw "Invalid slice direction";
}

void SegmentationSlice::setSegmentation( const Coord x, const Coord y, const LabelType seg )
{
   switch ( m_Dir )
   {
   case SLICEDIR_X:
      return m_Model->setSegmentation( m_Slice, y, x, seg );
   case SLICEDIR_Y:
      return m_Model->setSegmentation( x, m_Slice, y, seg );
   case SLICEDIR_Z:
      return m_Model->setSegmentation( x, y, m_Slice, seg );
   }

   throw "Invalid slice direction";
}