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
#include "CImgDICOMReader.h"

using namespace AWT;
using namespace cimg_library;

#include "vtkImageAlgorithm.h"
#include "vtkImageData.h"
#include "vtkImageExport.h"
#include "vtkImageCast.h"

#include "vtkCustomDICOMReader/vtkCustomDICOMReader.h"

#include "Useful/VTKImageType.h"

/*
template <class T>
int getVTKImageType( T arg );

#define VTK_TYPE_SPECIALIZED_TEMPLATE( Ty, VTK_Ty ) template <> int getVTKImageType( Ty ) { return VTK_Ty; }

VTK_TYPE_SPECIALIZED_TEMPLATE( float,          VTK_FLOAT );
VTK_TYPE_SPECIALIZED_TEMPLATE( double,         VTK_DOUBLE );
VTK_TYPE_SPECIALIZED_TEMPLATE( int,            VTK_INT );
VTK_TYPE_SPECIALIZED_TEMPLATE( unsigned int,   VTK_UNSIGNED_INT );
VTK_TYPE_SPECIALIZED_TEMPLATE( long,           VTK_LONG );
VTK_TYPE_SPECIALIZED_TEMPLATE( unsigned long,  VTK_UNSIGNED_LONG );
VTK_TYPE_SPECIALIZED_TEMPLATE( short,          VTK_SHORT );
VTK_TYPE_SPECIALIZED_TEMPLATE( unsigned short, VTK_UNSIGNED_SHORT );
VTK_TYPE_SPECIALIZED_TEMPLATE( unsigned char,  VTK_UNSIGNED_CHAR );
VTK_TYPE_SPECIALIZED_TEMPLATE( char,           VTK_CHAR );

#undef VTK_TYPE_SPECIALIZED_TEMPLATE
*/

template <class T>
CImg<T> AWT::loadDICOMDirectory( const char* directory, double* spacing )
{
   vtkImageAlgorithm* dicomData = vtkCustomDICOMReader::LoadDICOM( directory );
   dicomData->Update( );

   vtkImageData* image = dicomData->GetOutput( );
   int extent[6];

   image->GetExtent( extent );
   image->GetSpacing( spacing );

   CImg<T> img( extent[1]-extent[0]+1, extent[3]-extent[2]+1, extent[5]-extent[4]+1, 1 );
   T* ptr = img.ptr( );

   vtkImageCast* cast = vtkImageCast::New( );
   cast->SetOutputScalarType( AWT::getVTKImageType( static_cast<T>( 0 ) ) );

   vtkImageExport* exporter = vtkImageExport::New( );
   exporter->SetInputConnection( dicomData->GetOutputPort( ) );
   exporter->SetExportVoidPointer( ptr );

   exporter->Export( );

   exporter->Delete( );
   dicomData->Delete( );
   cast->Delete( );

   return img;
}

#define INSTANTIATE_MACRO( Ty ) template CImg<Ty> AWT::loadDICOMDirectory<Ty>( const char*, double* );

INSTANTIATE_MACRO( float )
INSTANTIATE_MACRO( double )
INSTANTIATE_MACRO( int )
INSTANTIATE_MACRO( unsigned int )
INSTANTIATE_MACRO( long )
INSTANTIATE_MACRO( unsigned long )
INSTANTIATE_MACRO( short )
INSTANTIATE_MACRO( unsigned short )
INSTANTIATE_MACRO( unsigned char ) 
INSTANTIATE_MACRO( char )

#undef INSTANTIATE_MACRO