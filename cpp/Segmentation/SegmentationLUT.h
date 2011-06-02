#ifndef __SEGMENTATIONLUT_H__
#define __SEGMENTATIONLUT_H__

namespace AWT
{
   class SegmentationLUT
   {
   public:
      SegmentationLUT( );

      void setColour( unsigned char label, unsigned char or, unsigned char og, unsigned char ob,
                                           unsigned char ur, unsigned char ug, unsigned char ub );

      void setColour( unsigned char label, unsigned char or, unsigned char og, unsigned char ob );

      void setColour( unsigned char label, unsigned char* overcoat, unsigned char* undercoat );

      void getColour( float v, unsigned char label, unsigned char* out_Colour );

      unsigned char getNumberOfColours( ) const;

   protected:
      unsigned char cols;
      unsigned char deltas[768];
      unsigned char underCoat[768];
   };
}

#endif // __SEGMENTATIONLUT_H__