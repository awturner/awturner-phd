#include "vtkImageImport.h"
#include "vtkImageMarchingCubes.h"
#include "vtkContourFilter.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkPolyDataNormals.h"
#include "vtkPolyDataWriter.h"
#include "vtkPLYWriter.h"
#include "vtkPolyDataConnectivityFilter.h"

#include "vtkCommand.h"
#include "vtkAlgorithm.h"
#include "vtkImageThreshold.h"

#include "CImg.h"
#include "Useful/RunLengthEncode.h"

#include "Useful/PrintMacros.h"

#include "Mesh/Mesh.h"
#include "Mesh/MeshBoundaryFinder.h"
#include "Mesh/VTKMeshLoader.h"

using namespace cimg_library;
using namespace AWT;

class MyEventListener : public vtkCommand
{
public:
   virtual void Execute(vtkObject *caller, unsigned long eventId, void *callData)
   {
      vtkAlgorithm* alg = vtkAlgorithm::SafeDownCast( caller );

      std::cerr << vtkCommand::GetStringFromEventId( eventId ) << ": ";
      switch ( eventId )
      {
      case vtkCommand::ProgressEvent:
         std::cerr << ( 100*alg->GetProgress( ) );
         break;
      }

      std::cerr << std::endl;
   }
};

void usage( )
{
   std::cerr << "Usage: SegmentationToVTK <RLEImage> <Label> <x-spacing> <y-spacing> <z-spacing> <type> <OutputVTKFileName>" << std::endl;
   std::cerr << "Usage: SegmentationToVTK <RLEImage> --info" << std::endl;
}

#include <map>

int main( int argc, char** argv )
{
   DEBUGLINE;

   if ( argc < 3 )
   {
      usage( );
      DEBUGLINEANDEXIT( 1 );
   }

   char* inFilename    = argv[1];
   unsigned char label = static_cast<unsigned char>( atoi( argv[2] ) );

   PRINTVBL(inFilename);

   DEBUGLINE;
   if (strcmp(argv[2], "--info") == 0)
   {
      DEBUGLINE;

      std::map<unsigned char,unsigned long> foundLabels;
      for ( int i = 0; i < 256; ++i )
         foundLabels[(unsigned char)i] = 0;

      CImg<unsigned char> rleUnpad = AWT::runLengthDecode<unsigned char>( std::ifstream(inFilename, std::ifstream::binary) );

      DEBUGLINE;
      cimg_forXYZ( rleUnpad, x, y, z )
      {
         ++foundLabels[rleUnpad(x,y,z)];
      }
      DEBUGLINE;

      DEBUGMACRO( "Image dimensions: " << rleUnpad.dimx() << " x " << rleUnpad.dimy() << " x " << rleUnpad.dimz() );
      DEBUGMACRO( "Labels in file: " );
      for ( int i = 0; i < 256; ++i )
         if (foundLabels[i])
            DEBUGMACRO( "Found label " << i << " (" << foundLabels[i] << " pixels)" );

      exit(0);
   }

   if ( argc < 8 )
   {
      usage( );
      DEBUGLINEANDEXIT( 1 );
   }
   char* outFilename = argv[7];
   char  type        = argv[6][0];
   double spacing[3];

   spacing[0] = atof( argv[3] );
   spacing[1] = atof( argv[4] );
   spacing[2] = atof( argv[5] );

   PRINTVBL( inFilename );
   PRINTVBL( outFilename );
   PRINTVBL( 1*label );
   PRINTVEC( spacing, 3 );

   vtkPolyDataWriter* writer = 0;
   std::string name( outFilename );
   
   if ( 0 == name.compare( name.length( )-4, 4, ".vtk" ) )
      writer = vtkPolyDataWriter::New( );
   else if ( 0 == name.compare( name.length( )-4, 4, ".ply" ) )
      writer = vtkPLYWriter::New( );
   else
   {
      DEBUGMACRO( "Unknown output file extension" );
      DEBUGLINEANDEXIT( 1 );
   }

   std::cerr << "Loading RLE image from " << argv[1] << std::endl;
   std::ifstream file( argv[1], std::ifstream::binary );
   CImg<unsigned char> rleUnpad = AWT::runLengthDecode<unsigned char>( file );
   std::cerr << "Loaded." << std::endl;

   CImg<unsigned char> rle( rleUnpad.dimx()+2, rleUnpad.dimy()+2, rleUnpad.dimz()+2, 1 );
   rle.fill( 0 );

   MyEventListener* ev = new MyEventListener( );

  DEBUGMACRO( rleUnpad.dimx() << "\t" << rleUnpad.dimy() << "\t" << rleUnpad.dimz() );

   cimg_forXYZ( rleUnpad, x, y, z )
   {
      if ( rleUnpad( x, y, z ) == label )
         rle( x+1, y+1, z+1 ) = 2;
      else
         rle( x+1, y+1, z+1 ) = 0;
   }
   
   vtkImageImport* import = vtkImageImport::New( );
   import->SetImportVoidPointer( rle.ptr( ) );
   import->SetWholeExtent( 0, rle.dimx()-1, 0, rle.dimy()-1, 0, rle.dimz()-1 );
   import->SetDataExtentToWholeExtent( );
   import->SetDataScalarTypeToUnsignedChar();

   import->SetDataSpacing( spacing );

   std::cerr << "Importing into VTK..." << std::endl;
   import->Update( );
   std::cerr << "Done." << std::endl;

   vtkPolyDataAlgorithm* march;

   if ( type == 'c' )
   {
      vtkContourFilter* filter = vtkContourFilter::New( );
      filter->SetValue( 0, 1 );
      filter->ComputeGradientsOff( );
      filter->ComputeScalarsOff( );
      filter->ComputeNormalsOff( );

      DEBUGMACRO( "Using vtkContourFilter" );
      march = filter;
   }
   else
   {
      vtkImageMarchingCubes* filter = vtkImageMarchingCubes::New( );
      filter->SetValue( 0, 1 );
      filter->ComputeGradientsOff( );
      filter->ComputeScalarsOff( );
      filter->ComputeNormalsOff( );

      DEBUGMACRO( "Using vtkImageMarchingCubes" );
      march = filter;
   }

   march->SetInputConnection( import->GetOutputPort( ) );
   
   march->AddObserver( vtkCommand::AnyEvent, ev );
   std::cerr << "Marching the cubes..." << std::endl;
   march->Update( );
   std::cerr << "Done." << std::endl;
   march->RemoveObserver( ev );

   vtkPolyDataConnectivityFilter* connect = vtkPolyDataConnectivityFilter::New( );
   connect->SetInputConnection( march->GetOutputPort( ) );
   connect->SetExtractionModeToLargestRegion( );

   std::cerr << "Finding largest connected region..." << std::endl;
   connect->Update( );
   std::cerr << "Done.  Found " << connect->GetNumberOfExtractedRegions( ) << " regions." << std::endl;
   
   {
      typedef double T;
      Mesh<T>::P mesh = VTKMeshLoader<T>::convert( connect->GetOutput( ) );
      Tuples<T>::P verts = MeshBoundaryFinder<T>::findBoundaryVertices( mesh );
      PRINTVBL( verts->getNumberOfPoints( ) );
   }

   //writer = vtkPolyDataWriter::New( );
   DEBUGMACRO( "Writing using class " << writer->GetClassName( ) );


   writer->SetInputConnection( connect->GetOutputPort( ) );
   writer->SetFileName( outFilename );
   writer->Update( );

   //connect->Delete( );
   march->Delete( );
   import->Delete( );
   //smooth->Delete( );
   writer->Delete( );

   ev->Delete( );

   return 0;
}