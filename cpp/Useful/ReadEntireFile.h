#ifndef __READENTIREFILE_H__
#define __READENTIREFILE_H__

#include <string>

namespace AWT
{
   class ReadEntireFile
   {
   protected:
      ReadEntireFile( );
      virtual ~ReadEntireFile( );

   public:
      static std::string read( const std::string& filename );

   };
}

#endif // __READENTIREFILE_H__