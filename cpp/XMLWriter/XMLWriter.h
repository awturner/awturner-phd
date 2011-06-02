/*
* ################################################################################
* ### MIT License
* ################################################################################
* 
* Copyright (c) 2006-2011 Andy Turner
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/
#ifndef __XMLWRITER_H__
#define __XMLWRITER_H__

#include <vector>
#include <ostream>

#include "BasicTagTypes.h"

namespace AWT
{
   namespace XML
   {
      class StartTag : public AWT::XML::TextSnippet
      {
      public:
         StartTag( const char* v );
         StartTag( const std::string& v );
      };

      class TagText : public AWT::XML::TextSnippet
      {
      public:
         TagText( const char* v );
         TagText( const std::string& v );
      };

      class CData : public AWT::XML::TextSnippet
      {
      public:
         CData( const char* v );
         CData( const std::string& v );
      };

      class EndTag
      {
      };

      class Attribute : public AWT::XML::TextPair
      {
      public:
         Attribute( const char* name, const char* value );
         Attribute( const std::string& name, const std::string& value );
      };

      class TagWithText : public AWT::XML::TextPair
      {
      public:
         TagWithText( const char* name, const char* value );
         TagWithText( const std::string& name, const std::string& value );
      };

      class XMLWriter
      {
      public:
         XMLWriter( std::ostream& os, bool compact = false );
         ~XMLWriter( );

         XMLWriter& startElement( const char* tagName );
         XMLWriter& startElement( const std::string& tagName );
         XMLWriter& endElement( );

         XMLWriter& addAttribute( const char* attrName, const char* attrValue );
         XMLWriter& addAttribute( const std::string& attrName, const std::string& attrValue );

         XMLWriter& addText( const char* text );
         XMLWriter& addText( const std::string& text );

         XMLWriter& addCData( const char* text );
         XMLWriter& addCData( const std::string& text );

         XMLWriter& addTextTag( const char* tagName, const char* text );
         XMLWriter& addTextTag( const std::string& tagName, const std::string& text );

         void finish( ); 

         static friend XMLWriter& operator<<( XMLWriter& xmlw, const char* st )
         {
            return xmlw.addText( st );
         }

         static friend XMLWriter& operator<<( XMLWriter& xmlw, const std::string& st )
         {
            return xmlw.addText( st );
         }

         static friend XMLWriter& operator<<( XMLWriter& xmlw, CData& st )
         {
            return xmlw.addCData( st.m_Text );
         }
           
         static friend XMLWriter& operator<<( XMLWriter& xmlw, StartTag& st )
         {
            return xmlw.startElement( st.m_Text );
         }

         static friend XMLWriter& operator<<( XMLWriter& xmlw, Attribute& at )
         {
            return xmlw.addAttribute( at.m_V1, at.m_V2 );
         }

         static friend XMLWriter& operator<<( XMLWriter& xmlw, TagText& tt )
         {
            return xmlw.addText( tt.m_Text );
         }

         static friend XMLWriter& operator<<( XMLWriter& xmlw, EndTag& et )
         {
            return xmlw.endElement( );
         }

         static friend XMLWriter& operator<<( XMLWriter& xmlw, TagWithText& twt )
         {
            return xmlw.addTextTag( twt.m_V1, twt.m_V2 );
         }

      protected:
         void writeEscapedString( const std::string& str );
         bool closeTagIfNecessary( );
         void printIndent( );

         bool                     m_StartTagNeedsClosing;
         bool                     m_TextAdded;

         bool                     m_Compact;
         std::ostream*            m_Os;
         std::vector<std::string> m_TagStack;
      };
   }
}

#endif // __XMLWRITER_H__