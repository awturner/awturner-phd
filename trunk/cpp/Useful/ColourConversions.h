#ifndef __COLOURCONVERSIONS_H__
#define __COLOURCONVERSIONS_H__

namespace AWT
{
   class ColourConversions
   {
   public:
      static void rgbToHsv( const double* rgb, double* hsv );
      static void hsvToRgb( const double* hsv, double* rgb );
   };
}

#endif __COLOURCONVERSIONS_H__