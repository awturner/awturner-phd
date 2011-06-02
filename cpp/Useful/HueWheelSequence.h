#ifndef __HUEWHEELSEQUENCE_H__
#define __HUEWHEELSEQUENCE_H__

#include "ColourSequence.h"
#include "ColourConversions.h"

namespace AWT
{
   class HueWheelSequence : public ColourSequence
   {
   public:
      typedef ManagedAutoPointer<HueWheelSequence> P;

   protected:
      HueWheelSequence( const unsigned int ncolours, const double saturation = 1, const double value = 1, const double alpha = 1 );

      virtual ~HueWheelSequence( );

   public:
      static HueWheelSequence::P getInstance(const unsigned int ncolours, const double saturation = 1, const double value = 1, const double alpha = 1 );

      virtual std::string getClassName( ) const;

      virtual void nextColour( float* colour );

      virtual void nextColour( double* colour );

   protected:
      struct D;
      D* m_D;
   };
}

#endif // __HUEWHEELSEQUENCE_H__