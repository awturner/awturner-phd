#ifndef __AWTEXCEPTION_H__
#define __AWTEXCEPTION_H__

#include <string>
#include <ostream>
#include <sstream>
#include <exception>

#define AWTEXCEPTIONTHROWCLASS( c, m ) { throw m; }
#define AWTEXCEPTIONTHROW( m ) AWTEXCEPTIONTHROWCLASS(AWT::Exception, m)

#define AWTEXCEPTIONTHROWCLASS_STREAM( c, m ) { std::stringstream ss; ss << m; throw c( ss.str( ), __FILE__, __LINE__ ); }
#define AWTEXCEPTIONTHROW_STREAM( m ) AWTEXCEPTIONTHROWCLASS_STREAM(AWT::Exception, m)

namespace AWT
{
   class Exception : public std::exception
   {
   public:
      Exception( std::string _m, std::string _f, int _l )
         : className( "AWT::Exception" ), message( _m ), file( _f ), line( _l )
      {
      }

      static friend std::ostream& operator<<( std::ostream& os, AWT::Exception& ex )
      {
         os << ex.className << " in file " << ex.file << " at line " << ex.line << ": " << ex.message;
         return os;
      }

      const std::string getClassName( ) const
      {
         return className;
      }

      const std::string getMessage( ) const
      {
         return message;
      }

      const int getLineNumber( ) const
      {
         return line;
      }

   protected:
      const std::string className;
      const std::string message;
      const std::string file;
      const int line;
   };
}

#endif // __AWTEXCEPTION_H__