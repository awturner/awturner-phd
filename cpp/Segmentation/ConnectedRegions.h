#ifndef __CONNECTEDREGIONS_H__
#define __CONNECTEDREGIONS_H__

#include <CImg.h>

namespace AWT
{
   cimg_library::CImg<unsigned char> findConnectedRegions( cimg_library::CImg<unsigned char>& img );
   void findConnectedRegions( cimg_library::CImg<unsigned char>& img, cimg_library::CImg<unsigned char>& ret );
}

#endif // __CONNECTEDREGIONS_H__