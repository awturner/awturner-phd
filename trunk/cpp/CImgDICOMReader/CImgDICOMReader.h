#ifndef __DICOMREADER_H__
#define __DICOMREADER_H__

#include <CImg.h>

namespace AWT
{
   template <class T>
   cimg_library::CImg<T> loadDICOMDirectory( const char* directory, double* spacing );
}

#endif __DICOMREADER_H__