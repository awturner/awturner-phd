#ifndef __vtkCustomDICOMReader_H__
#define __vtkCustomDICOMReader_H__

#include <string>
#include <vector>


namespace itk
{
   template <class Pt, unsigned int Dim>
   class Image;

   template <class ImTy>
   class ImageSeriesReader;

   class GDCMImageIO;
   class GDCMSeriesFileNames;

   template <class ImTy>
   class VTKImageExport;
}

class vtkImageAlgorithm;

namespace vtkCustomDICOMReader
{
   typedef signed short                      PixelType;
   const unsigned int                        Dimension = 3;
   typedef itk::Image<PixelType,Dimension>   ImageType;
   typedef itk::ImageSeriesReader<ImageType> ReaderType;
   typedef itk::GDCMImageIO                  ImageIOType;
   typedef itk::GDCMSeriesFileNames          NamesGeneratorType;
   typedef std::vector<std::string>          SeriesIdContainer;
   typedef std::vector<std::string>          FileNamesContainer;
   typedef std::vector<std::string>          SeriesIdContainer;
   typedef itk::VTKImageExport<ImageType>    ExportType;

   template <typename ITK_Exporter, typename VTK_Importer>
   void ConnectPipelines(ITK_Exporter exporter, VTK_Importer* importer);

   std::string selectSeriesUID( const SeriesIdContainer& seriesUID );

   vtkImageAlgorithm* LoadDICOM( FileNamesContainer& fileNames );
   vtkImageAlgorithm* LoadDICOM( const char* directoryName, std::string& seriesIdentifier );
   vtkImageAlgorithm* LoadDICOM( const char* directoryName );
}

#endif // __vtkCustomDICOMReader_H__
