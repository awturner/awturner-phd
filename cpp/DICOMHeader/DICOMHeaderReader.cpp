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
#include "DICOMHeaderReader.h"

#include <istream>

using namespace AWT;

namespace
{

   template <class T>
   int read( std::istream& in, T& val )
   {
      in.read( (char*)( &val ), sizeof( T ) );
      return in.gcount( );
   }

   template <class T>
   void read( std::istream& in, T* buffer, const unsigned int len )
   {
      for ( int i = 0; i < len; ++i )
         read( in, buffer[i] );
   }

   bool isLongValueRepresentation( char* valueRep )
   {
      char* cmp = ":OB:OW:SQ:UN";

      for ( int i = 0; i < 4; ++i )
      {
         if ( !memcmp( valueRep, &cmp[3*i+1], 2 ) )
         {
            return true;
         }
      }
      return false;
   }

   bool isTagImplicit( DICOMTagIdentifier ge )
   {
      // This is a huge massive assumption that things starting with 
      // 0xFF are implicit.  This will cause problems in the future.
      return ( ge.group & 0xFF00 ) == 0xFF00;
   }

   unsigned int readTagLength( std::istream& fin, bool goLong )
   {
      unsigned int length;
      if ( goLong )
      {
         read( fin, length );

         if ( length == 0xFFFFFFFF )
         {
            //printf("  Unspecified length: treating as zero.\n");
            length = 0;
         }
      }
      else
      {  
         unsigned short tmpLength;
         read( fin, tmpLength );
         length = tmpLength;
      }

      return length;
   }
}

AWT::DICOMTagIdentifier::DICOMTagIdentifier( GroupType _group, ElementType _element )
: group( _group ), element( _element )
{
}

bool AWT::DICOMTagIdentifier::compare( unsigned short g, unsigned short e ) const
{
   return ( g == group ) && ( e == element );
}

bool AWT::DICOMHeaderReader::readStream( std::istream &fin, AWT::DICOMTagIdentifierConsumer *consumer )
{
   // The first 128 bytes are unused
   fin.seekg( 128 );

   char magic[4];

   // The next four bytes are a magic number, which should be "DICM"
   // (no quotes)
   read( fin, magic, 4 );

   if ( memcmp( magic, "DICM", 4 ) != 0 )
   {
      return false;
   }

   // Used to store the explicit value representation
   char valueRep[2];

   // Used to store the data read when we are interested in the tag
   char* data = new char[0];
   // The length of this buffer (avoid recreating it each time if
   // many tags are being read)
   unsigned long dataLength = 0;

   // This will store the group and element IDs
   DICOMTagIdentifier ge( 0, 0 );

   // Use this to determine if we need to run any more
   // (e.g. we have found the tags we are after)
   bool continueReading = true;

   while ( continueReading )
   {
      // Read until we can't read any more
      if ( !read( fin, ge.group   ) ) break;
      if ( !read( fin, ge.element ) ) break;

      unsigned int length;
      // Is the value representation implicit or explicit?
      bool implicit = isTagImplicit( ge );

      if ( implicit )
      {
         // Implicit VRs have a 4-byte length
         length = readTagLength( fin, true );
         valueRep[0] = valueRep[1] = 255;
      }
      else
      {
         // Read the VR
         fin.read( valueRep, 2 );

         // Some VRs' length fields are 4 bytes, with 2 ignored
         // bytes preceding.
         if ( isLongValueRepresentation( valueRep ) )
         {
            fin.seekg( 2, std::ios_base::cur );
            length = readTagLength( fin, true );
         }
         else
         {
            length = readTagLength( fin, false );
         }
      }
      
      // If the consumer wants this tag, then read it and
      // pass it out to the consumer.
      if ( consumer->wantTag( ge ) )
      {
         if ( length > dataLength )
         {
            delete[] data;
            data = new char[dataLength = length];
         }
         fin.read( data, length );

         if ( implicit )
            continueReading = consumer->implicitDataCallback( ge, data, length );
         else
            continueReading = consumer->explicitDataCallback( ge, valueRep, data, length );

      }
      else
      {
         // Otherwise skip it
         fin.seekg( length, std::ios_base::cur );
      }
   }

   delete[] data;

   return true;
}