#include "vtkPolyData.h"
#include "vtkMatrix4x4.h"
#include "vtkTransformFilter.h"
#include "vtkPolyDataReader.h"
#include "vtkPolyDataWriter.h"
#include "vtkReverseSense.h"
#include "vtkMatrixToHomogeneousTransform.h"

#include "Useful/PrintMacros.h"

#include <iostream>

void usage( )
{
   std::cerr << "Usage: FlipVTKPolyData <input file 1> <output file 1> [<input file 2> <output file 2> ...]" << std::endl;
}

int main( int argc, char** argv )
{
   if ( argc < 3 )
   {
      usage( );
      DEBUGLINEANDEXIT( 1 );
   }

   int curArg = 1;

   int flipAxis = 0;

   if ( !strcmp( argv[curArg], "-x" ) )
   {
      ++curArg;
      flipAxis = 0;
   }
   else if ( !strcmp( argv[curArg], "-y" ) )
   {
      ++curArg;
      flipAxis = 1;
   }
   else if ( !strcmp( argv[curArg], "-z" ) )
   {
      ++curArg;
      flipAxis = 2;
   }

   vtkMatrix4x4* matrix = vtkMatrix4x4::New( );
   (*matrix)[flipAxis][flipAxis] = -1;

   vtkMatrixToHomogeneousTransform* matToHom = vtkMatrixToHomogeneousTransform::New( );
   matToHom->SetInput( matrix );

   for ( int i = curArg; i < argc; i += 2 )
   {
      vtkPolyDataReader* reader = vtkPolyDataReader::New( );
      reader->SetFileName( argv[i+0] );

      reader->Update( );
      std::cerr << "Polydata read from " << argv[i+0] << std::endl;

      vtkPolyData* poly = reader->GetOutput( );
      double point[3];
      double centroid[] = { 0, 0, 0 };
      for ( int p = 0; p < poly->GetNumberOfPoints( ); ++p )
      {
         poly->GetPoint( p, point );

         centroid[0] += point[0];
         centroid[1] += point[1];
         centroid[2] += point[2];
      }

      centroid[0] /= poly->GetNumberOfPoints( );
      centroid[1] /= poly->GetNumberOfPoints( );
      centroid[2] /= poly->GetNumberOfPoints( );

      std::cerr << "Centroid: " << centroid[0] << " " << centroid[1] << " " << centroid[2] << std::endl;
      (*matrix)[0][3] = -centroid[0];
      (*matrix)[1][3] = -centroid[1];
      (*matrix)[2][3] = -centroid[2];

      (*matrix)[flipAxis][3] *= -1;

      vtkTransformFilter* transformFilter = vtkTransformFilter::New( );
      transformFilter->SetInputConnection( reader->GetOutputPort( ) );
      transformFilter->SetTransform( matToHom );

      std::cerr << *matToHom << std::endl;

      vtkReverseSense* reverse = vtkReverseSense::New( );
      reverse->SetInputConnection( transformFilter->GetOutputPort( ) );

      vtkPolyDataWriter* writer = vtkPolyDataWriter::New( );
      writer->SetInputConnection( reverse->GetOutputPort( ) );
      writer->SetFileName( argv[i+1] );

      writer->Update( );
      std::cerr << "Polydata written to " << argv[i+1] << std::endl;

      writer->Delete( );
      reverse->Delete( );
      transformFilter->Delete( );
      reader->Delete( );
   }

   matrix->Delete( );
   matToHom->Delete( );
}