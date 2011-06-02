#ifndef __SEGMENTATIONSLICE_H__
#define __SEGMENTATIONSLICE_H__

#include "SegmentationModel.h"

class SegmentationSlice
{
public:
   SegmentationSlice( SegmentationModel* model, const Coord slice, const SliceDirection dir = SLICEDIR_Z );
   virtual ~SegmentationSlice( );

   Coord getWidth( ) const;
   Coord getHeight( ) const;

   ImageType getMin( ) const;
   ImageType getMax( ) const;

   void getScan( ImageType* buffer ) const;
   void getSegmentation( LabelType* buffer ) const;

   ImageType getScan( const Coord x, const Coord y ) const;
   LabelType getSegmentation( const Coord x, const Coord y ) const;

   void getScanAndSegmentation( const Coord x, const Coord y, ImageType& im, LabelType& seg );

   void setSegmentation( const Coord x, const Coord y, const LabelType seg );

protected:
   SegmentationModel* m_Model;
   const SliceDirection m_Dir;
   const Coord m_Slice;
};

#endif // __SEGMENTATIONSLICE_H__