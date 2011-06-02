#ifndef __SEGMENTATIONMODEL_H__
#define __SEGMENTATIONMODEL_H__

typedef unsigned int Coord;
typedef short ImageType;
typedef unsigned char LabelType;

enum SliceDirection
{
   SLICEDIR_X,
   SLICEDIR_Y,
   SLICEDIR_Z
};

class SegmentationModel
{
public:
   SegmentationModel( const char* modelFile );

   virtual ~SegmentationModel( );

   const bool loaded( ) const;

   void getScanSlice( const Coord slice, const SliceDirection dir, ImageType* buffer ) const;
   void getSegmentationSlice( const Coord slice, const SliceDirection dir, LabelType* buffer ) const;

   const ImageType getScan( const Coord x, const Coord y, const Coord z );

   const LabelType getSegmentation( const Coord x, const Coord y, const Coord z );
   void setSegmentation( const Coord x, const Coord y, const Coord z, const LabelType seg );

   void getScanAndSegmentation( const Coord x, const Coord y, const Coord z, ImageType& im, LabelType& seg );

   const Coord getWidth( ) const;
   const Coord getHeight( ) const;
   const Coord getDepth( ) const;

   const ImageType getMin( ) const;
   const ImageType getMax( ) const;

protected:
   struct D;
   D* m_D;
};

#endif // __SEGMENTATIONMODEL_H__