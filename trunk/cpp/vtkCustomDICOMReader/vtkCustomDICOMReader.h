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

   std::string selectSeriesUID(const SeriesIdContainer& seriesUID);

   vtkImageAlgorithm* LoadDICOM(FileNamesContainer& fileNames);
   vtkImageAlgorithm* LoadDICOM(const char* directoryName, std::string& seriesIdentifier);
   vtkImageAlgorithm* LoadDICOM(const char* directoryName);
}

#endif // __vtkCustomDICOMReader_H__
