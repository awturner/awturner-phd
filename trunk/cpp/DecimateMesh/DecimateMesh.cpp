#include "vtkPolyDataReader.h"
#include "vtkQuadricDecimation.h"
#include "vtkDecimatePro.h"
#include "vtkPolyDataWriter.h"
#include "vtkPolyData.h"
#include "vtkPolyDataNormals.h"

#include <iostream>

#include "Useful/PrintMacros.h"

int main(int argc, char** argv)
{
   if ( argc < 4 )
   {
      std::cerr << "Usage: DecimateMesh <input filename> <decimation level [0..1]> <output filename>" << std::endl;
      DEBUGLINEANDEXIT( 1 );
   }

   vtkPolyDataReader* reader = vtkPolyDataReader::New( );
   reader->SetFileName( argv[1] );

#ifdef _USEQUADRIC
   vtkQuadricDecimation* decim = vtkQuadricDecimation::New( );
#else
   vtkDecimatePro* decim = vtkDecimatePro::New( );
   decim->PreserveTopologyOn( );
   decim->SplittingOff( );
#endif

   PRINTVBL( decim->GetClassName( ) );

   decim->SetInputConnection( reader->GetOutputPort( ) );
   decim->SetTargetReduction( atof( argv[2] ) );

   /*
   vtkPolyDataNormals* norm = vtkPolyDataNormals::New( );
   norm->SetInputConnection( decim->GetOutputPort( ) );
   norm->SplittingOff( );
   */

   vtkPolyDataWriter* writer = vtkPolyDataWriter::New( );
   writer->SetInputConnection( decim->GetOutputPort( ) );
   writer->SetFileName( argv[3] );

   writer->Update( );

   std::cerr << "Original number of cells: " << reader->GetOutput( )->GetNumberOfCells( ) << std::endl;
   std::cerr << "Final number of cells: " << decim->GetOutput( )->GetNumberOfCells( ) << std::endl;

   std::cerr << "Achieved decimation: " << (1.0-(decim->GetOutput( )->GetNumberOfCells( )*1.0)/(reader->GetOutput( )->GetNumberOfCells( )+0.0)) << std::endl;

   //norm->Delete( );
   writer->Delete( );
   decim->Delete( );
   reader->Delete( );
}