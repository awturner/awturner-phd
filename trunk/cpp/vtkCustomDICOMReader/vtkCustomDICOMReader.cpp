/*
* ################################################################################
* ### MIT License
* ################################################################################
* 
* Copyright (c) 2006-2011 Andy Turner
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/
#include "vtkCustomDICOMReader/vtkCustomDICOMReader.h"

#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkImageSeriesReader.h"
#include "itkImageFileWriter.h"

#include "itkVTKImageExport.h"

#include "vtkImageAlgorithm.h"
#include "vtkImageData.h"
#include "vtkImageViewer.h"
#include "vtkImageImport.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkImageCast.h"

#include <vector>
#include <string>

/**
 * This function will connect the given itk::VTKImageExport filter to
 * the given vtkImageImport filter.
 */
template <typename ITK_Exporter, typename VTK_Importer>
void vtkCustomDICOMReader::ConnectPipelines(ITK_Exporter exporter, VTK_Importer* importer)
{
  importer->SetUpdateInformationCallback(exporter->GetUpdateInformationCallback());
  importer->SetPipelineModifiedCallback(exporter->GetPipelineModifiedCallback());
  importer->SetWholeExtentCallback(exporter->GetWholeExtentCallback());
  importer->SetSpacingCallback(exporter->GetSpacingCallback());
  importer->SetOriginCallback(exporter->GetOriginCallback());
  importer->SetScalarTypeCallback(exporter->GetScalarTypeCallback());
  importer->SetNumberOfComponentsCallback(exporter->GetNumberOfComponentsCallback());
  importer->SetPropagateUpdateExtentCallback(exporter->GetPropagateUpdateExtentCallback());
  importer->SetUpdateDataCallback(exporter->GetUpdateDataCallback());
  importer->SetDataExtentCallback(exporter->GetDataExtentCallback());
  importer->SetBufferPointerCallback(exporter->GetBufferPointerCallback());
  importer->SetCallbackUserData(exporter->GetCallbackUserData());
}

std::string vtkCustomDICOMReader::selectSeriesUID( const SeriesIdContainer& seriesUID )
{
   for (;;) {
      SeriesIdContainer::const_iterator seriesItr = seriesUID.begin( );
      SeriesIdContainer::const_iterator seriesEnd = seriesUID.end( );
      
      std::cout << "Please select a series:" << std::endl;
      int i = 0;
      while ( seriesItr != seriesEnd )
      {
         std::cout << i++ << "\t" << seriesItr->c_str( ) << std::endl;
         seriesItr++;
      }

      std::cin >> i;
      //i = 0;

      if ( i >= 0 && i < seriesUID.size( ) )
      {
         return seriesUID[i];
      }
   }
}

vtkImageAlgorithm* vtkCustomDICOMReader::LoadDICOM( const char* directoryName )
{
   NamesGeneratorType::Pointer nameGenerator = NamesGeneratorType::New( );
   nameGenerator->SetUseSeriesDetails( true );
   nameGenerator->SetDirectory( directoryName );

   const SeriesIdContainer& seriesUID = nameGenerator->GetSeriesUIDs( );
   
   SeriesIdContainer::const_iterator seriesItr = seriesUID.begin( );
   SeriesIdContainer::const_iterator seriesEnd = seriesUID.end( );

   std::string seriesIdentifier;

   switch ( seriesUID.size( ) )
   {
   case 0:
      std::cout << "No series were found!" << std::endl;
      return 0;
   case 1:
      std::cout << "Only 1 series UID found: selecting this." << std::endl;
      seriesIdentifier = seriesUID[0];
      break;
   default:
      seriesIdentifier = selectSeriesUID( seriesUID );
   }

   std::cerr << "Loading series: " << seriesIdentifier << std::endl;

   return LoadDICOM( directoryName, seriesIdentifier );
}

vtkImageAlgorithm* vtkCustomDICOMReader::LoadDICOM( const char* directoryName, std::string& seriesIdentifier )
{
   NamesGeneratorType::Pointer nameGenerator = NamesGeneratorType::New( );
   nameGenerator->SetUseSeriesDetails( true );
   nameGenerator->SetDirectory( directoryName );

   FileNamesContainer fileNames;
   fileNames = nameGenerator->GetFileNames( seriesIdentifier );

   return LoadDICOM( fileNames );
}

vtkImageAlgorithm* vtkCustomDICOMReader::LoadDICOM( FileNamesContainer& fileNames )
{
   ReaderType::Pointer reader = ReaderType::New( );
   ImageIOType::Pointer dicomIO = ImageIOType::New( );

   reader->SetImageIO( dicomIO );
   
   reader->SetFileNames( fileNames );

   try 
   {
      reader->Update( );
   }
   catch ( itk::ExceptionObject &ex )
   {
      std::cerr << ex << std::endl;
      return 0;
   }

   //dicomIO->Print( std::cerr );

   ExportType::Pointer exporter = ExportType::New( );
   exporter->SetInput( reader->GetOutput( ) );

   vtkImageImport* importer = vtkImageImport::New( );
   
   ConnectPipelines<ExportType::Pointer,vtkImageImport>( exporter, importer );
   importer->Update( );
   
   vtkImageCast* cast = vtkImageCast::New( );
   cast->SetInputConnection( importer->GetOutputPort( ) );
   cast->SetOutputScalarType( importer->GetDataScalarType( ) );
   cast->Update( );

   importer->Delete( );

   return cast;
} // LoadDICOM()
