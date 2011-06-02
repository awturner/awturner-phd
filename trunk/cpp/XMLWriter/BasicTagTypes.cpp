#include "BasicTagTypes.h"

#include <string>

AWT::XML::TextSnippet::TextSnippet( const char* str ) : m_Text( std::string( str ) )
{
}

AWT::XML::TextSnippet::TextSnippet( const std::string& str ) : m_Text( str )
{
}

AWT::XML::TextPair::TextPair( const char* v1, const char* v2 ) : m_V1( std::string( v1 ) ), m_V2( std::string( v2 ) )
{
}

AWT::XML::TextPair::TextPair( const std::string& v1, const std::string& v2 ) : m_V1( v1 ), m_V2( v2 )
{
}