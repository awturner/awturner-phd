#include "ReadEntireFile.h"

#include "Useful/Exception.h"

#include <fstream>
#include <sstream>

AWT::ReadEntireFile::ReadEntireFile( )
{
}

AWT::ReadEntireFile::~ReadEntireFile( )
{
}

std::string AWT::ReadEntireFile::read( const std::string& filename )
{
   std::ifstream is( filename.c_str( ) );

   if ( is.bad( ) )
      AWTEXCEPTIONTHROW_STREAM( "Could not read from " << filename );

   std::stringstream sstr;
   char ch;

   while ( true )
   {
      is.get( ch );

      if ( !is.good( ) )
         break;

      sstr << ch;
   }

   is.close( );

   return sstr.str( );
}