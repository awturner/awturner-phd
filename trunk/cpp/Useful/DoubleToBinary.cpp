#include "DoubleToBinary.h"

#include <bitset>

std::ostream& AWT::doubleToBinary( std::ostream& os, double dbl )
{
   unsigned long* ulp = reinterpret_cast<unsigned long*>(&dbl);
   std::bitset<64> bits1(*ulp);
   std::bitset<64> bits2(*(++ulp));
   bits2 <<= 32;    // Shift values left
   bits2 |= bits1;  // Combine this part with other part
   os << bits2;
   return os;
}