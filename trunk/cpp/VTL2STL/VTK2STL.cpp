#include "vtkPolyDataReader.h"
#include "vtkPLYReader.h"
#include "vtkSTLReader.h"
#include "vtkSTLWriter.h"
#include "vtkPLYWriter.h"

#include <sstream>

int main( int argc, char** argv )
{
   char suffix[5];
   vtkPolyDataWriter* writer;

   if ( strcmp( argv[1], "ply" ) == 0 )
   {
      strcpy_s( suffix, argv[1] );
      writer = vtkPLYWriter::New( );
      writer->SetFileTypeToBinary( );
   }
   else if ( strcmp( argv[1], "stl" ) == 0 )
   {
      strcpy_s( suffix, argv[1] );
      writer = vtkSTLWriter::New( );
      writer->SetFileTypeToBinary( );
   }
   else if ( strcmp( argv[1], "vtk" ) == 0 )
   {
      strcpy_s( suffix, argv[1] );
      writer = vtkPolyDataWriter::New( );
   }
   else
   {
      std::cerr << "Invalid type: " << argv[1] << "; exiting" << std::endl;
      exit( 1 );
   }

   for ( int i = 2; i < argc; ++i )
   {
      std::stringstream sstr;
      sstr << argv[i] << "." << suffix;
      writer->SetFileName( sstr.str( ).c_str( ) );

      std::string name( argv[i] );
      if ( 0 == name.compare( name.length( )-4, 4, ".vtk" ) )
      {
         vtkPolyDataReader* reader = vtkPolyDataReader::New( );
         reader->SetFileName( argv[i] );

         std::cerr << "Reading " << name << "..." << std::endl;
         reader->Update( );
         std::cerr << "Done." << std::endl;

         writer->SetInputConnection( reader->GetOutputPort( ) );

         std::cerr << "Writing " << sstr.str( ) << "..." << std::endl;
         writer->Update( );
         std::cerr << "Done." << std::endl;

         reader->Delete( );
      }
      else
      {
         vtkPolyDataAlgorithm* reader;

         if ( 0 == name.compare( name.length( )-4, 4, ".ply" ) )
         {
            reader = vtkPLYReader::New( );
            dynamic_cast<vtkPLYReader*>( reader )->SetFileName( argv[i] );
         }
         else if ( 0 == name.compare( name.length( )-4, 4, ".stl" ) )
         {
            reader = vtkSTLReader::New( );
            dynamic_cast<vtkSTLReader*>( reader )->SetFileName( argv[i] );
         }

         std::cerr << "Reading " << name << "..." << std::endl;
         reader->Update( );
         std::cerr << "Done." << std::endl;

         writer->SetInputConnection( reader->GetOutputPort( ) );

         std::cerr << "Writing " << sstr.str( ) << "..." << std::endl;
         writer->Update( );
         std::cerr << "Done." << std::endl;

         reader->Delete( );
      }
   }

   writer->Delete( );
}