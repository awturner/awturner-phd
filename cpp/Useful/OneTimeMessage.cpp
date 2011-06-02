#include "OneTimeMessage.h"

#include "Useful/PrintMacros.h"

struct AWT::OneTimeMessage::D
{
   std::string m_Msg;
   bool        m_Sent;
};

AWT::OneTimeMessage::OneTimeMessage( const std::string& msg )
{
   m_D = new D;
   m_D->m_Msg = msg;
   m_D->m_Sent = false;
}

AWT::OneTimeMessage::~OneTimeMessage( )
{
   delete m_D;
}

void AWT::OneTimeMessage::report( )
{
   if ( !m_D->m_Sent )
   {
      DEBUGMACRO( m_D->m_Msg );
      setSent( true );
   }
}

bool AWT::OneTimeMessage::isSent( ) const
{
   return m_D->m_Sent;
}

void AWT::OneTimeMessage::setSent( const bool s )
{
   if ( s != m_D->m_Sent )
   {
      m_D->m_Sent = s;
   }
}
