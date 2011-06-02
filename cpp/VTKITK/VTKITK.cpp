#include "itkImage.h"
#include "itkImageFileReader.h"
//#include "itkImageToVTKImageFilter.h"

#include "Wrapping/WrapITK/ExternalProjects/ItkVtkGlue/src/itkImageToVTKImageFilter.h"

#include "vtkImageViewer.h"
#include "vtkRenderWindowInteractor.h"

int main( int argc, char** argv )
{
   typedef itk::Image<unsigned short,2> ImageType;
   typedef itk::ImageFileReader<ImageType> ReaderType;
   typedef itk::ImageToVTKImageFilter<ImageType> ConnectorType;

   ReaderType::Pointer reader = ReaderType::New( );
   ConnectorType::Pointer connector = ConnectorType::New( );

   reader->SetFileName( argv[1] );
   connector->SetInput( reader->GetOutput( ) );

   vtkImageViewer* viewer = vtkImageViewer::New( );
   vtkRenderWindowInteractor* rwi = vtkRenderWindowInteractor::New( );

   viewer->SetupInteractor( rwi );
   viewer->SetInput( connector->GetOutput( ) );

   viewer->Render( );
   viewer->SetColorWindow( 255 );
   viewer->SetColorLevel( 128 );

   rwi->Start( );

   return 0;
}
