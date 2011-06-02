#include "AnalyzeHeader.h"

#include "Useful/PrintMacros.h"

#include "ScanData.h"

#include <stdio.h>
#include <stdlib.h>

#include <limits>

#include <CImg.h>

#include "vtkImageImport.h"
#include "vtkPiecewiseFunction.h"
#include "vtkColorTransferFunction.h"
#include "vtkVolumeProperty.h"
#include "vtkVolumeRayCastCompositeFunction.h"
#include "vtkVolumeRayCastMapper.h"
#include "vtkVolumeRayCastIsosurfaceFunction.h"
#include "vtkFiniteDifferenceGradientEstimator.h"
#include "vtkVolume.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkPlane.h"

#include "SimpleVTKAppFramework/SimpleVTKAppFramework.h"

using namespace cimg_library;
using namespace AWT;

int displayPlane;
int displayPlaneMax;
int displayPlaneMin;

void displayVTK( ScanData::P data )
{
   data->load( );

   DEBUGLINE;
   unsigned int size[3];
   float spacing[3];
   double spacingd[3];

   data->getSize( size );
   data->getSpacing( spacing );

   for ( int i = 0; i < 3; ++i )
      spacingd[i] = spacing[i];

   // Copy the data
   const unsigned int npixels = size[0]*size[1]*size[2];
   unsigned short* d = new unsigned short[ npixels ];
   const short* od = data->getData( );

   for ( unsigned int idx = 0; idx < npixels; ++idx )
      d[idx] = static_cast<unsigned short>( od[idx] );

   vtkImageImport* import = vtkImageImport::New( );
   import->SetImportVoidPointer( d );
   import->SetWholeExtent( 0, size[0]-1, 0, size[1]-1, 0, size[2]-1 );
   import->SetDataExtentToWholeExtent( );
   import->SetDataScalarTypeToUnsignedShort();
   import->SetDataSpacing( spacingd );
   DEBUGLINE;
   import->Update( );
   DEBUGLINE;

   vtkPiecewiseFunction* opTransfer = vtkPiecewiseFunction::New( );
   opTransfer->AddPoint( 1200, 0 );
   opTransfer->AddPoint( 2000, 0.4 );

   vtkColorTransferFunction* colTransfer = vtkColorTransferFunction::New( );
   colTransfer->AddRGBPoint( 1000, 0, 0, 0 );
   colTransfer->AddRGBPoint( 1600, 1, 1, 1 );

   vtkVolumeProperty* volProp = vtkVolumeProperty::New( );
   volProp->SetColor( colTransfer );
   volProp->SetScalarOpacity( opTransfer );

   vtkVolumeRayCastCompositeFunction* compFunc = vtkVolumeRayCastCompositeFunction::New( );
   compFunc->SetCompositeMethodToInterpolateFirst( );
      
   vtkFiniteDifferenceGradientEstimator* gradEst = vtkFiniteDifferenceGradientEstimator::New( );

   vtkVolumeRayCastMapper* volMapper = vtkVolumeRayCastMapper::New( );
   volMapper->SetVolumeRayCastFunction( compFunc );
   volMapper->SetGradientEstimator( gradEst );
   volMapper->SetInputConnection( import->GetOutputPort( ) );

   volMapper->SetCroppingRegionPlanes( ( spacing[0]*size[0] ) / 2, std::numeric_limits<double>::infinity( ),
      -std::numeric_limits<double>::infinity( ), std::numeric_limits<double>::infinity( ), 
      -std::numeric_limits<double>::infinity( ), std::numeric_limits<double>::infinity( ) );
   volMapper->CroppingOn( );
   volMapper->SetSampleDistance( 0.5 );

   vtkVolume* volume = vtkVolume::New( );
   volume->SetMapper( volMapper );
   volume->SetProperty( volProp );

   SimpleVTKAppFramework* appFrame = new SimpleVTKAppFramework( );
   appFrame->getRenderer( )->AddVolume( volume );

   appFrame->start( );

   delete appFrame;
   
   volume->Delete( );
   volMapper->Delete( );
   compFunc->Delete( );
   volProp->Delete( );
   colTransfer->Delete( );
   opTransfer->Delete( );
   import->Delete( );
   delete [] d;
}

void changePlane( int delta )
{
   displayPlane += delta;

   displayPlane = std::max( displayPlane, displayPlaneMin );
   displayPlane = std::min( displayPlane, displayPlaneMax );
}

void calculateDisplayPlaneLimits( const CImg<short>& img )
{
   displayPlaneMin = 0;
   displayPlaneMax = img.dimz() - 1;

   while ( displayPlaneMin < displayPlaneMax )
   {
      CImg<short> planeImage = img.get_shared_plane( displayPlaneMin );
      if ( planeImage.min() != planeImage.max() )
         break;

      ++displayPlaneMin;
   }

   while ( displayPlaneMin < displayPlaneMax )
   {
      CImg<short> planeImage = img.get_shared_plane( displayPlaneMax );
      if ( planeImage.min() != planeImage.max() )
         break;

      --displayPlaneMax;
   }

   displayPlane = displayPlaneMin;
}

int main( int argc, char** argv )
{
   if ( argc != 2 )
   {
      printf( "Must provide header and filename!\n" );
      exit( 1 );
   }

   char buffer[255];

   sprintf( buffer, "%s.hdr", argv[1] );

   AWT::ScanData::P scanData = AWT::ScanData::getInstance( buffer );
   
   unsigned int imagesize[3];
   float spacing[3];

   scanData->getSize( imagesize );
   scanData->getSpacing( spacing );

   PRINTVEC( imagesize, 3 );
   PRINTVEC( spacing, 3 );

   scanData->load( );

   DEBUGLINE;
   const short* imageData = scanData->getData( );

   

   DEBUGLINE;
   // Make an image out of the data we've just read
   CImg<short> img( imageData, imagesize[0], imagesize[1], imagesize[2], 1, true  );
   DEBUGLINE;

   // Make an image which has the correct pixel aspect ratio
   float size[] = { imagesize[0]*spacing[0], imagesize[1]*spacing[1] };

   // Make the larger of the two dimensions 512 pixels
   float maxdim = ( size[0] > size[1] ) ? size[0] : size[1];
   size[0] = size[0] / maxdim * 512;
   size[1] = size[1] / maxdim * 512;

   float xscale = ( imagesize[0] - 1.f ) / ( size[0] - 1.f );
   float yscale = ( imagesize[1] - 1.f ) / ( size[1] - 1.f );

   calculateDisplayPlaneLimits( img );
   
   displayVTK( scanData );

   PRINTVEC( size, 2 );

   CImg<unsigned char> dispImage( (int)size[0], (int)size[1] );
   CImgDisplay disp( dispImage, argv[1] );

   cimg::wait( 100 );
   DEBUGLINE;
   PRINTBOOL( disp.is_closed );

   while ( !disp.is_closed )
   {
      if ( disp.wheel )
         changePlane( disp.wheel );
      else if ( disp.key == cimg::keyARROWUP )
         changePlane( 1 );
      else if ( disp.key == cimg::keyARROWDOWN )
         changePlane( -1 );
      else if ( disp.key == cimg::keyPAGEUP )
         changePlane( 10 );
      else if ( disp.key == cimg::keyPAGEDOWN )
         changePlane( -10 );

      disp.wheel = disp.key = 0;

      disp.set_title( "%s: [%d/%d]", argv[1], displayPlane+1, img.dimz() );

      float val;

      cimg_forXY( dispImage, x, y )
      {
         // Need to convert back into the scan's coordinate system
         float xx = ( x - 0.5f ) * xscale + 0.5f;
         float yy = ( y - 0.5f ) * yscale + 0.5f;

         val = img.linear_pix2d( xx, yy, displayPlane );

         val -= 500;
         val /= 1000;

         if ( val < 0 ) val = 0;
         if ( val > 1 ) val = 1;

         dispImage( x, y ) = static_cast<unsigned char>( 255*val );
      }

      disp.display( dispImage );

      cimg::wait( 20 );
   }
}