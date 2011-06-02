#ifndef __ONETIMEMESSAGE_H__
#define __ONETIMEMESSAGE_H__

#include <string>

namespace AWT
{
   class OneTimeMessage
   {
   public:
      OneTimeMessage( const std::string& msg );
      virtual ~OneTimeMessage( );

      virtual void report( );

      virtual bool isSent( ) const;
      virtual void setSent( const bool s );

   protected:
      struct D;
      D* m_D;

   };
}

#endif // __ONETIMEMESSAGE_H__