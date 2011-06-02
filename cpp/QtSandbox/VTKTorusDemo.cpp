#include "VTKTorusDemo.h"

#include "Mesh/VTKMeshLoader.h"

#include "vtkSuperquadric.h"
#include "vtkImplicitBoolean.h"
#include "vtkSampleFunction.h"
#include "vtkImageMarchingCubes.h"
#include "vtkImageThreshold.h"
#include "vtkPolyDataWriter.h"
#include "vtkPolyDataNormals.h"
#include "vtkPolyDataConnectivityFilter.h"

template <class T>
typename AWT::Mesh<T>::P AWT::vtkTorusDemo( )
{
   vtkSuperquadric* superQuadA  = vtkSuperquadric::New( );
   vtkSuperquadric* superQuadB  = vtkSuperquadric::New( );
   vtkImplicitBoolean* implicit = vtkImplicitBoolean::New( );
   vtkSampleFunction* sampFunc  = vtkSampleFunction::New( );
   
   superQuadA->SetToroidal( true );
   superQuadB->SetToroidal( true );

   superQuadB->SetCenter( 1 - 0.5*superQuadA->GetThickness( ), 0, 0 );

   implicit->AddFunction( superQuadA );
   implicit->AddFunction( superQuadB );
   implicit->SetOperationTypeToUnion( );

   sampFunc->SetSampleDimensions( 64, 64, 64 );
   sampFunc->SetImplicitFunction( implicit );
   sampFunc->SetModelBounds( -2.0, 2.0, -1, 1, -1, 1 );

   vtkImageThreshold* thresh = vtkImageThreshold::New( );
   thresh->SetInputConnection( sampFunc->GetOutputPort( ) );
   thresh->ThresholdByLower( 0 );
   thresh->SetOutValue( 0 );
   thresh->SetInValue( 2 );
   thresh->SetOutputScalarTypeToUnsignedChar( );

   vtkImageMarchingCubes* march = vtkImageMarchingCubes::New( );
   march->SetInputConnection( thresh->GetOutputPort( ) );
   march->SetValue( 0, 1 );

   /*
   vtkPolyDataConnectivityFilter* connect = vtkPolyDataConnectivityFilter::New( );
   connect->SetInputConnection( march->GetOutputPort( ) );
   connect->SetExtractionModeToLargestRegion( );

   //vtkPolyDataNormals* norms = vtkPolyDataNormals::New( );
   //norms->SetInputConnection( connect->GetOutputPort( ) );

   vtkPolyDataWriter* writer = vtkPolyDataWriter::New( );
   writer->SetInputConnection( connect->GetOutputPort( ) );
   writer->SetFileName( "doubletorus.vtk" );
   writer->Update( );
   */

   march->Update( );

   Mesh<T>::P ret = VTKMeshLoader<T>::convert( march->GetOutput( ) );

   thresh->Delete( );
   march->Delete( );
   sampFunc->Delete( );
   superQuadA->Delete( );
   superQuadB->Delete( );
   implicit->Delete( );

   return ret;
}

template AWT::Mesh<double>::P AWT::vtkTorusDemo<double>( );