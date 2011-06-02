#include "RunLengthEncode.h"

unsigned long AWT::readRunLength( std::istream& is, unsigned char type )
{
   switch ( type )
   {
   case 0:
      {
         unsigned char rl;
         readFromStream( is, rl );
         return rl;
      }
   case 1:
      {
         unsigned short rl;
         readFromStream( is, rl );
         return rl;
      }
   case 2:
      {
         unsigned int rl;
         readFromStream( is, rl );
         return rl;
      }
   case 3:
      {
         unsigned long rl;
         readFromStream( is, rl );
         return rl;
      }
   }

   AWTEXCEPTIONTHROW("Invalid type!");
}