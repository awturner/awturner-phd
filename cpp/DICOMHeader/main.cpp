#include <fstream>
#include <iostream>

#include "DICOMHeaderReader.h"

#include "Useful/PrintMacros.h"

using namespace AWT;

class MyDICOMTagIdentifierConsumer : public DICOMTagIdentifierConsumer
{
public:
   MyDICOMTagIdentifierConsumer( GroupType group, ElementType el )
   {
      this->m_Group = group;
      this->m_Element = el;
   }

   ~MyDICOMTagIdentifierConsumer( )
   {
      delete[] m_Data;
      delete[] m_VR;
   }

   bool wantTag( AWT::DICOMTagIdentifier ge )
   {
      return ge.compare( m_Group, m_Element );
   }

   bool explicitDataCallback( AWT::DICOMTagIdentifier ge, char* vr, char* buffer, int length )
   {
      if ( wantTag( ge ) )
      {
         m_Data = new char[length+1];
         m_VR   = new char[2];

         memcpy( m_Data, buffer, length );
         memcpy( m_VR, vr, 2 );

         m_Data[length] = 0;

         printf("%s\n", m_Data);
         return false;
      }
      
      return true;
   }

protected:
   GroupType m_Group;
   ElementType m_Element;

   char* m_VR;
   char* m_Data;
};

int main( int argc, char** argv )
{
   std::ifstream fin;

   bool groupSet   = false;
   bool elementSet = false;
   bool fileSet    = false;
   bool showUsage  = false;

   AWT::GroupType group;
   AWT::ElementType element;
   int fileArgIndex = -1;

   for ( int a = 1; a < argc; ++a )
   {
      if ( memcmp( argv[a], "-ge", 3 ) == 0 )
      {
         if ( groupSet || elementSet || a+1 >= argc )
         {
            showUsage = true;
            break;
         }

         if ( sscanf( argv[++a], "%x,%x", &group, &element ) )
         {
            groupSet = true;
            elementSet = true;
         }
      }
      else if ( memcmp( argv[a], "-g", 2 ) == 0 )
      {
         if ( groupSet || a+1 >= argc )
         {
            showUsage = true;
            break;
         }

         if ( 0 != sscanf( argv[++a], "%x", &group ) )
            groupSet = true;
      }
      else if ( memcmp( argv[a], "-e", 2 ) == 0 )
      {
         if ( elementSet || a+1 >= argc )
         {
            showUsage = true;
            break;
         }

         if ( 0 != sscanf( argv[++a], "%x", &element ) )
            elementSet = true;
      }
      else
      {
         if ( fileSet || a >= argc )
         {
            showUsage = true;
            break;
         }
         
         fileArgIndex = a;
         fileSet = true;
      }
   }

   if ( showUsage || !(groupSet && elementSet && fileSet) )
   {
      std::cerr << "Usage: DICOMHeaderReader.exe [-g <Hex Group ID> -e <Hex Element ID>|-ge <Hex Group ID>,<Hex Element ID>] <filename>" << std::endl;
      DEBUGLINEANDEXIT( 1 );
   }

   //std::cerr << "Reading file " << argv[fileArgIndex] << std::endl;
   fin.open( argv[fileArgIndex], std::ifstream::binary );
   
   if ( fin.fail( ) )
   {
      std::cerr << "Could not open file " << argv[fileArgIndex] << std::endl;
      DEBUGLINEANDEXIT( 2 );
   }

   AWT::DICOMTagIdentifierConsumer* consumer = new MyDICOMTagIdentifierConsumer( group, element );

   AWT::DICOMHeaderReader::readStream( fin, consumer );

   delete consumer;
}