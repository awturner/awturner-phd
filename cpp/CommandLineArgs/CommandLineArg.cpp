#include "CommandLineArg.h"

#include <stdio.h>
#include <string>

AWT::CommandLineArg::CommandLineArg( char* name, bool req ) : m_Name( name ), m_Required( req )
{
   m_Set = false;
}

const char* AWT::CommandLineArg::getName( ) const
{
   return m_Name;
}

bool AWT::CommandLineArg::isRequired( ) const
{
   return m_Required;
}

bool AWT::CommandLineArg::isSet( ) const
{
   return m_Set;
}

bool AWT::CommandLineArg::isValid( ) const
{
   return !m_Required || m_Set;
}

template <class T>
AWT::MultiCommandLineArg<T>::MultiCommandLineArg( char* name, int n, bool req ) : CommandLineArg( name, req ), m_N( n )
{
   m_Values   = new T[ m_N ];
   m_SetArray = new bool[ m_N ];

   for ( int i = 0; i < m_N; ++i )
      m_SetArray[i] = false;
}

template <class T>
AWT::MultiCommandLineArg<T>::~MultiCommandLineArg( )
{
   delete m_Values;
   delete m_SetArray;
}

template <class T>
T AWT::MultiCommandLineArg<T>::getValue( const int idx ) const
{
   return m_Values[idx];
}

template <class T>
void AWT::MultiCommandLineArg<T>::parse( char** argv )
{
   for ( int i = 0; i < m_N; ++i )
      if ( parse( argv[i], m_Values[i] ) )
         m_SetArray[i] = true;
}

template <>
bool AWT::MultiCommandLineArg<bool>::parse( char* arg, bool& val )
{
   if ( strcmp( arg, "true" ) == 0 || strcmp( arg, "True" ) == 0 || strcmp( arg, "1" ) == 0 )
   {
      val = true;
      return true;
   }
   else if ( strcmp( arg, "true" ) == 0 || strcmp( arg, "True" ) == 0 || strcmp( arg, "1" ) == 0 )
   {
      val = false;
      return true;
   }

   return false;
}

template <>
bool AWT::MultiCommandLineArg<int>::parse( char* arg, int& val )
{
   int pos = 0;
   while ( arg[pos] != '\0' )
   {
      switch ( arg[pos] )
      {
      case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
         break;
      default:
         return false;
      }
      ++pos;
   }
   val = atoi( arg );
   return true;
}

template <>
bool AWT::MultiCommandLineArg<double>::parse( char* arg, double& val )
{
   val = atof( arg );
   return true;
}

template <>
bool AWT::MultiCommandLineArg<std::string>::parse( char* arg, std::string& val )
{
   val = std::string( arg );
   return true;
}

template <class T>
bool AWT::MultiCommandLineArg<T>::isSet( ) const
{
   for ( int i = 0; i < m_N; ++i )
      if ( !m_SetArray[i] )
         return false;

   return true;
}

template <class T>
bool AWT::MultiCommandLineArg<T>::isValid( ) const
{
   return !isRequired( ) || isSet( );
}

template class AWT::MultiCommandLineArg<bool>;
template class AWT::MultiCommandLineArg<int>;
template class AWT::MultiCommandLineArg<double>;
template class AWT::MultiCommandLineArg<std::string>;

#include <iostream>

void processCommandLineArgs( int argc, char** argv, int& iterations, double* affine )
{
   for ( int i = 1; i < argc; ++i )
   {
      if ( strcmp( "-iterations", argv[i] ) == 0 )
      {
         if ( ++i >= argc )
            return;

         iterations = atoi( argv[i] );
      }
      else if ( strcmp( "-affine", argv[i] ) == 0 )
      {
         int idx = 0;
         while ( idx < 12 && ++i < argc )
         {
            affine[idx++] = atof( argv[i] );
         }

         if ( i >= argc )
            return;
      }
   }
}

#include <signal.h>

void handleSignal( int signal )
{
   std::cerr << "Signal " << signal << " was received." << std::endl;
}

int main( int argc, char** argv )
{
   signal( SIGINT, &handleSignal );

   while ( true )
   {

   }
}