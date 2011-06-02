#ifndef __COLOURSEQUENCE_H__
#define __COLOURSEQUENCE_H__

#include "ObjectManagement/ManagedObject.h"

namespace AWT
{
   class ColourSequence : public ManagedObject
   {
   public:
      typedef ManagedAutoPointer<ColourSequence> P;

   public:
      virtual void nextColour( double* colour ) = 0;
      virtual void nextColour( float* colour ) = 0;
   };
}

#endif // __COLOURSEQUENCE_H__