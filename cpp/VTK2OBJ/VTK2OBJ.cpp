#include "vtkOBJExporter.h"
#include "vtkPolyDataReader.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"

#include <sstream>

int main( int argc, char** argv )
{
   for ( int i = 1; i < argc; ++i )
   {
      vtkPolyDataReader* reader = vtkPolyDataReader::New( );
      reader->SetFileName( argv[i] );

      vtkPolyDataMapper* mapper = vtkPolyDataMapper::New( );
      mapper->SetInputConnection( reader->GetOutputPort( ) );

      vtkActor* actor = vtkActor::New( );
      actor->SetMapper( mapper );

      vtkRenderer* ren = vtkRenderer::New( );
      ren->AddActor( actor );

      vtkRenderWindow* renWin = vtkRenderWindow::New( );
      renWin->AddRenderer( ren );

      vtkOBJExporter* writer = vtkOBJExporter::New( );
      
      writer->SetFilePrefix( argv[i] );
      writer->SetRenderWindow( renWin );

      writer->Write( );

      writer->Delete( );
      renWin->Delete( );
      ren->Delete( );
      actor->Delete( );
      mapper->Delete( );
      reader->Delete( );
   }
}