#include "SegmentationLUT.h"

using namespace AWT;

AWT::SegmentationLUT::SegmentationLUT( )
{
   // By default, all colours are gray
   for ( int i = 0; i < 768; ++i )
   {
      deltas[i] = 255;
      underCoat[i] = 0;
   }

   cols = 0;
}

unsigned char AWT::SegmentationLUT::getNumberOfColours( ) const
{
   return cols+1;
}

void AWT::SegmentationLUT::setColour( unsigned char label, unsigned char or, unsigned char og, unsigned char ob,
                                      unsigned char ur, unsigned char ug, unsigned char ub )
{
   this->underCoat[3*label + 0] = ur;
   this->underCoat[3*label + 1] = ug;
   this->underCoat[3*label + 2] = ub;

   this->deltas[3*label + 0] = or - ur;
   this->deltas[3*label + 1] = og - ug;
   this->deltas[3*label + 2] = ob - ub;

   if ( cols < label )
      cols = label;
}

void AWT::SegmentationLUT::setColour( unsigned char label, unsigned char or, unsigned char og, unsigned char ob )
{
   this->setColour( label, or, og, ob, or >> 2, og >> 2, ob >> 2 );
}

void AWT::SegmentationLUT::setColour(unsigned char label, unsigned char *overcoat, unsigned char *undercoat)
{
   this->setColour( label, overcoat[0], overcoat[1], overcoat[2], undercoat[0], undercoat[1], undercoat[2] );
}

void AWT::SegmentationLUT::getColour(float v, unsigned char label, unsigned char *out_Colour)
{
   if ( v < 0 ) v = 0.f;
   if ( v > 1 ) v = 1.f;

   out_Colour[0] = this->underCoat[3*label+0] + v * this->deltas[3*label+0];
   out_Colour[1] = this->underCoat[3*label+1] + v * this->deltas[3*label+1];
   out_Colour[2] = this->underCoat[3*label+2] + v * this->deltas[3*label+2];
}