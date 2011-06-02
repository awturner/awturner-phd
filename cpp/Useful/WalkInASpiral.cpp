#include "WalkInASpiral.h"

#include <algorithm>
#include <cmath>
#include "Useful/Exception.h"

void AWT::walkInASpiral( int& i, int& j )
{
   int set = std::max( abs(i), abs(j) );

   if ( j == -set && i <= set )
      ++i;
   else if ( i == set && j < set )
      ++j;
   else if ( i > -set && j == set )
      --i;
   else if ( i == -set && j > -set )
      --j;
   else
      AWTEXCEPTIONTHROW( "No walk step defined!" );
}