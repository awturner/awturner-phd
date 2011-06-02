#include "CleanedVTKPolyData.h"

#include "vtkPolyData.h"
#include "vtkPolyDataAlgorithm.h"
#include "vtkTriangleFilter.h"
#include "vtkCleanPolyData.h"
#include "vtkPolyDataNormals.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkDataArray.h"

AWT::CleanedVtkPolyData::CleanedVtkPolyData(vtkPolyData *in_Poly)
: m_CleanedData( 0 )
{
   init( in_Poly );
}

AWT::CleanedVtkPolyData::CleanedVtkPolyData(vtkPolyDataAlgorithm *in_PolyAlgo)
: m_CleanedData( 0 )
{
   in_PolyAlgo->Update( );
   init( in_PolyAlgo->GetOutput( ) );
}

AWT::CleanedVtkPolyData::~CleanedVtkPolyData( )
{
   this->m_CleanedData->UnRegister( 0 );
}

void AWT::CleanedVtkPolyData::init(vtkPolyData *in_Poly)
{
   in_Poly->Register( 0 );

   vtkCleanPolyData* clean_a = vtkCleanPolyData::New( );
   vtkCleanPolyData* clean_b = vtkCleanPolyData::New( );
   vtkTriangleFilter* trig   = vtkTriangleFilter::New( );
   vtkPolyDataNormals* norms = vtkPolyDataNormals::New( );

   clean_a->SetInput( in_Poly );
   trig->SetInputConnection( clean_a->GetOutputPort( ) );
   clean_b->SetInputConnection( trig->GetOutputPort( ) );
   norms->SetInputConnection( clean_b->GetOutputPort( ) );

   trig->PassLinesOff( );
   trig->PassVertsOff( );

   try
   {
      vtkDataArray* data = in_Poly->GetPointData( )->GetNormals( );

      norms->SetComputePointNormals( data == 0 );
   }
   catch ( ... )
   {
      norms->ComputePointNormalsOn( );
   }

   try
   {
      vtkDataArray* data = in_Poly->GetCellData( )->GetNormals( );

      norms->SetComputeCellNormals( data == 0 );
   }
   catch ( ... )
   {
      norms->ComputeCellNormalsOn( );
   }

   norms->Update( );

   this->m_CleanedData = vtkPolyData::New( );
   this->m_CleanedData->DeepCopy( norms->GetOutput( ) );

   clean_a->Delete( );
   clean_b->Delete( );
   trig->Delete( );
   norms->Update( );

   in_Poly->UnRegister( 0 );
}
