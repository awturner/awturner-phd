#include "vtkPolyDataReader.h"
#include "vtkCleanPolyData.h"
#include "vtkPolyDataWriter.h"
#include "vtkTriangleFilter.h"

int main( int argc, char** argv )
{
   vtkPolyDataReader* reader = vtkPolyDataReader::New( );
   vtkCleanPolyData*  clean  = vtkCleanPolyData::New( );
   vtkTriangleFilter* trig   = vtkTriangleFilter::New( );
   vtkCleanPolyData*  clean2 = vtkCleanPolyData::New( );
   vtkPolyDataWriter* writer = vtkPolyDataWriter::New( );

   trig->PassLinesOff( );
   trig->PassVertsOff( );

   clean->SetInputConnection( reader->GetOutputPort( ) );
   trig->SetInputConnection( clean->GetOutputPort( ) );
   clean2->SetInputConnection( trig->GetOutputPort( ) );
   writer->SetInputConnection( clean2->GetOutputPort( ) );

   for ( int i = 1; i < argc; i += 2 )
   {
      reader->SetFileName( argv[i] );
      writer->SetFileName( argv[i+1] );

      writer->Update( );
   }

   writer->Delete( );
   clean2->Delete( );
   trig->Delete( );
   clean->Delete( );
   reader->Delete( );
}