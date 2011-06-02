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
#include "XMLWriter.h"

#include <string>

namespace AWT
{
   namespace XML
   {
      StartTag::StartTag( const char* v ) : TextSnippet( v ) {}
      StartTag::StartTag( const std::string& v ) : TextSnippet( v ) {}

      TagText::TagText( const char* v ) : TextSnippet( v ) {}
      TagText::TagText( const std::string& v ) : TextSnippet( v ) {}

      CData::CData( const char* v ) : TextSnippet( v ) {}
      CData::CData( const std::string& v ) : TextSnippet( v ) {}

      Attribute::Attribute( const char* v1, const char* v2 ) : TextPair( v1, v2 ) {}
      Attribute::Attribute( const std::string& v1, const std::string& v2 ) : TextPair( v1, v2 ) {}

      TagWithText::TagWithText( const char* v1, const char* v2 ) : TextPair( v1, v2 ) {}
      TagWithText::TagWithText( const std::string& v1, const std::string& v2 ) : TextPair( v1, v2 ) {}

      XMLWriter::XMLWriter( std::ostream& os, bool compact )
      {
         m_Os = &os;
         m_Compact = compact;

         m_StartTagNeedsClosing = false;
         m_TextAdded            = false;

         os << "<?xml version=\"1.0\"?>" << std::endl;
      }

      XMLWriter::~XMLWriter( )
      {
         finish( );
      }

      void XMLWriter::finish( )
      {
         while ( !m_TagStack.empty( ) )
            endElement( );
      }

      void XMLWriter::printIndent( )
      {
         for ( int i = 0; i < m_TagStack.size( ); ++i )
            *m_Os << "  ";
      }

      XMLWriter& XMLWriter::startElement( const char* tagName )
      {
         return startElement( std::string( tagName ) );
      }

      bool XMLWriter::closeTagIfNecessary( )
      {
         if ( m_StartTagNeedsClosing )
         {
            *m_Os << ">";
            m_StartTagNeedsClosing = false;

            if ( !m_Compact )
               *m_Os << std::endl;

            return true;
         }
         else
         {
            return false;
         }
      }

      XMLWriter& XMLWriter::startElement(const std::string &tagName)
      {
         closeTagIfNecessary( );

         if ( !m_Compact )
            printIndent( );

         *m_Os << "<" << tagName;

         m_TagStack.push_back( tagName );
         
         m_StartTagNeedsClosing = true;
         m_TextAdded            = false;

         return *this;
      }

      XMLWriter& XMLWriter::addAttribute( const char* attrName, const char* attrValue )
      {
         return addAttribute( std::string( attrName ), std::string( attrValue ) );
      }

      XMLWriter& XMLWriter::addAttribute( const std::string& attrName, const std::string& attrValue )
      {
         if ( !m_StartTagNeedsClosing )
            throw "Can't push attributes";

         *m_Os << " " << attrName << "=\"";
         writeEscapedString( attrValue );
         *m_Os << "\"";

         return *this;
      }

      XMLWriter& XMLWriter::addText( const char* text )
      {
         return addText( std::string( text ) );
      }

      XMLWriter& XMLWriter::addText( const std::string& text )
      {
         closeTagIfNecessary( );

         writeEscapedString( text );
         m_TextAdded = true;

         return *this;
      }

      using std::hex;

      void XMLWriter::writeEscapedString( const std::string& str )
      {
         const char* cstr = str.c_str( );

         for ( int i = 0, imax = str.length( ); i < imax; ++i )
         {
            switch ( cstr[i] )
            {
            case '"':
               *m_Os << "&quot;";
               break;
            case '&':
               *m_Os << "&amp;";
               break;
            case '<':
               *m_Os << "&lt;";
               break;
            case '>':
               *m_Os << "&gt;";
               break;
            default:
               if ( cstr[i] < 32 )
               {
                  *m_Os << "&#x" << std::hex << (cstr[i]<0 ? (cstr[i]+256) : cstr[i]) << std::dec << ";";
               }
               else
               {
                  *m_Os << cstr[i];
               }
            }
         }
      }

      XMLWriter& XMLWriter::endElement( )
      {
         if ( m_TagStack.empty( ) )
            throw "No tags to end!";

         std::string tagName = m_TagStack.back( );
         m_TagStack.pop_back( );

         if ( m_StartTagNeedsClosing )
         {
            *m_Os << "/>";

            if ( !m_Compact )
               *m_Os << std::endl;

            m_StartTagNeedsClosing = false;
         }
         else
         {
            if ( !m_TextAdded )
            {
               if ( !m_Compact )
                  printIndent( );
            }
            else
            {
               m_TextAdded = false;
            }

            *m_Os << "</" << tagName << ">";

            if ( !m_Compact )
               *m_Os << std::endl;
         }

         return *this;
      }

      XMLWriter& XMLWriter::addTextTag( const char* tagName, const char* text )
      {
         return addTextTag( std::string( tagName ), std::string( text ) );
      }

      XMLWriter& XMLWriter::addTextTag( const std::string& tagName, const std::string& text )
      {
         startElement( tagName );
         addText( text );
         endElement( );

         return *this;
      }

      XMLWriter& XMLWriter::addCData(const char *text)
      {
         char* ptr = const_cast<char*>( text );
         int state = 0;

         closeTagIfNecessary( );

         *m_Os << "<![CDATA[";

         while ( *ptr != '\0' )
         {
            // Check for the illegal end tag "]]>"
            // this splits the tag between two CData tags,
            // thus making it legal again.
            if ( *ptr == ']' )
            {
               if ( *(ptr+1) == ']' && *(ptr+2) == '>' )
               {
                  *m_Os << "]]]]><![CDATA[>";
                  ptr += 2;
               }
               else
               {
                  *m_Os << *ptr;
               }
            }
            else
            {
               *m_Os << *ptr;
            }

            ++ptr;
         }

         *m_Os << "]]>";

         return *this;
      }

      XMLWriter& XMLWriter::addCData(const std::string &text)
      {
         return addCData( text.c_str( ) );
      }
   }
}