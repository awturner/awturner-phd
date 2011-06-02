#define NOMINMAX

#include "../Useful/RunLengthEncode.h"

#include "Useful/PrintMacros.h"

#include <iostream>
#include <fstream>

void usage( )
{
   std::cerr << "OpenCloseSegmentation <RLE file> <label> <0-for-open,1-for-close> <Output RLE file>" << std::endl;
}

int main( int argc, char** argv )
{
   if ( argc < 5 )
   {
      usage( );
      DEBUGLINEANDEXIT( 1 );
   }

   std::ifstream is( argv[1], std::ifstream::binary );

   std::cerr << "Opening " << argv[1] << "..." << std::endl;
   CImg<unsigned char> im = AWT::runLengthDecode<unsigned char>( is );
   std::cerr << "Done." << std::endl;

   std::cerr << "Dimensions: " << im.dimx() << " x " << im.dimy() << " x " << im.dimz() << " x " << im.dimv() << std::endl;

   unsigned char label = static_cast<unsigned char>( atoi( argv[2] ) );

   cimg_forXYZ( im, x, y, z )
      im(x,y,z) = (im(x,y,z)==label)?1:0;
   
   CImg<unsigned char> sel( 3, 3, 3 );
   sel.fill( 1 );

   int repeats = atoi( argv[3] );

   if ( repeats == 0 )
   {
      std::cerr << "  Dilating..." << std::endl;
      im.dilate( sel );
      std::cerr << "  Eroding..." << std::endl;
      im.erode( sel );

      std::cerr << "  Eroding..." << std::endl;
      im.erode( sel );
      std::cerr << "  Dilating..." << std::endl;
      im.dilate( sel );
   }
   else
   {
      std::cerr << "  Eroding..." << std::endl;
      im.erode( sel );
      std::cerr << "  Dilating..." << std::endl;
      im.dilate( sel );

      std::cerr << "  Dilating..." << std::endl;
      im.dilate( sel );
      std::cerr << "  Eroding..." << std::endl;
      im.erode( sel );

   }
   
   std::ofstream os( argv[4], std::ifstream::binary );
   AWT::runLengthEncode<unsigned char>( im, os );
   os.close( );
}