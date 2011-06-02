#include "vtkSTLReader.h"
#include "vtkPolyData.h"

#include <iostream>
#include <fstream>

#include "Useful/PrintMacros.h"

int main( int argc, char** argv )
{
   vtkSTLReader* stlReader = vtkSTLReader::New( );
   stlReader->SetFileName( argv[1] );

   stlReader->Update( );

   vtkPolyData* poly = stlReader->GetOutput( );

   double ps[3];
   for ( int i = 0; i < poly->GetNumberOfPoints(); ++i )
   {
      poly->GetPoint( i, ps );

      std::cout << (-ps[0]) << " " << ps[1] << " " << ps[2];
      std::cout << " 0 0 0" << std::endl;
   }

   stlReader->Delete( );
}