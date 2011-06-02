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
#include "QuadTreeImage.h"
#include "OctreeImage.h"

using namespace AWT;
typedef unsigned int T;

class PrintImageVisitor : public ImageVisitor<T>
{
protected:
   PrintImageVisitor( )
   {
   }

   virtual ~PrintImageVisitor( )
   {
   }

public:
   typedef ManagedAutoPointer<PrintImageVisitor> P;

   static P getInstance( )
   {
      AUTOGETINSTANCE( PrintImageVisitor, ( ) );
   }

   std::string getClassName( ) const;

   virtual bool visit( const unsigned int x, const unsigned int y, const unsigned int z, const T value )
   {
      //DEBUGMACRO( "I(" << x << "," << y << "," << z << ") = " << value );
      --valueCount;
      return true;
   }

   static unsigned int valueCount;
};

unsigned int PrintImageVisitor::valueCount = 0;

GETNAMEMACRO( PrintImageVisitor );

int main( int argc, char** argv )
{
   const unsigned int w = 512;
   const unsigned int h = 512;
   const unsigned int d = 96;

   OctreeImage<T>::P im = OctreeImage<T>::getInstance( w, h, d );

   im->fill( 0 );
   
   DEBUGMACRO( "Setting values..." );
   for ( unsigned int i = 0; i < w*h; ++i )
   {
      const unsigned int x = rand()%w;
      const unsigned int y = rand()%h;
      const unsigned int z = rand()%d;

      //DEBUGMACRO( x << ", " << y << ", " << z );

      if ( im->getValue( x, y, z ) == 0 )
         ++PrintImageVisitor::valueCount;

      im->setValue( x, y, z, 1 );
   }
   DEBUGMACRO( "Done." );

   PRINTVBL( PrintImageVisitor::valueCount );

   //std::cout << im << std::endl;

   DEBUGMACRO( "Visiting..." );
   im->visit( 1, 1, PrintImageVisitor::getInstance( ) );
   DEBUGMACRO( "Done." );

   PRINTVBL( PrintImageVisitor::valueCount );

   PRINTVBL( Octree<T>::instanceCount );
   DEBUGMACRO( "Tidying up..." );
}