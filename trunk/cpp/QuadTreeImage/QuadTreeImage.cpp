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