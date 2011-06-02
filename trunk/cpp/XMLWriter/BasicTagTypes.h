#ifndef __BASICTAGTYPES_H__
#define __BASICTAGTYPES_H__

#include <string>

namespace AWT
{
   namespace XML
   {
      class TextSnippet
      {
      public:
         TextSnippet( const char* str );
         TextSnippet( const std::string& str );

         const std::string m_Text;
      };

      class TextPair
      {
      public:
         TextPair( const char* v1, const char* v2 );
         TextPair( const std::string& v1, const std::string& v2 );

         const std::string m_V1;
         const std::string m_V2;
      };
   }
}

#endif // __BASICTAGTYPES_H__