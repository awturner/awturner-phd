#define NOMINMAX

#include <algorithm>
#include <limits>
#include <iostream>

#include "CImg.h"

#include "../Useful/Derivatives.h"



using namespace cimg_library;

#define LABEL_OUTSIDE  3
#define LABEL_OUTER2   2
#define LABEL_OUTER    1
#define LABEL_ACTIVE   0
#define LABEL_INNER   -1
#define LABEL_INNER2  -2
#define LABEL_INSIDE  -3

#define LABEL_UNVISITED 127

struct Point3
{
   int x, y, z;
   Point3* next;

   Point3( int x, int y, int z, Point3* next = 0 )
   {
      this->x = x;
      this->y = y;
      this->z = z;
      this->next = next;
   }

   ~Point3( )
   {
      if ( this->next )
         delete this->next;
   }
};

Point3* pointStack = 0;

Point3* getPointInstance( int x, int y, int z, Point3* next = 0 )
{
   if ( pointStack == 0 )
      return new Point3( x, y, z, next );

   Point3* ret = pointStack;
   pointStack = pointStack->next;

   ret->x = x;
   ret->y = y;
   ret->z = z;
   ret->next = next;

   return ret;
}

void recycle( Point3* p )
{
   p->next = pointStack;
   pointStack = p;
}

#define OVER3x3x3( Im, x, y, z ) \
   for ( int zz = std::max( 0, z-1 ); zz <= std::min( Im.dimz()-1, z+1 ); ++zz ) \
      for ( int yy = std::max( 0, y-1 ); yy <= std::min( Im.dimy()-1, y+1 ); ++yy ) \
         for ( int xx = std::max( 0, x-1 ); xx <= std::min( Im.dimx()-1, x+1 ); ++xx )


void calculateLabels( const CImg<float>& phi, CImg<char>& labelImage, Point3** points = 0 )
{
   // Create a flag image so that we know if a pixel has been labelled
   labelImage.fill( LABEL_UNVISITED );

   if ( points == 0 )
      points = new Point3*[5];

   for ( int i = 0; i < 5; ++i )
      points[i] = 0;

   cimg_forXYZ( phi, x, y, z )
   {
      if ( phi(x,y,z) < 0.5 && phi(x,y,z) >= -0.5 )
      {
         // Add it to the list, and label in the image
         points[2] = getPointInstance( x, y, z, points[2] );
         labelImage( x, y, z ) = 2;
      }
   }

   int listOrder[] = { 2, 3, 1, 4, 0 };

   for ( int listOrderIndex = 0; listOrderIndex < 5; ++listOrderIndex )
   {
      int el = listOrder[listOrderIndex];

      for ( Point3* listPtr = points[el]; listPtr != 0; listPtr = listPtr->next )
      {
         OVER3x3x3( phi, listPtr->x, listPtr->y, listPtr->z )
         {
            if ( labelImage(xx,yy,zz) == LABEL_UNVISITED )
            {
               if ( phi(xx,yy,zz) > 0 )
               {
                  if ( el < 4 )
                  {
                     points[el+1] = getPointInstance( xx, yy, zz, points[el+1] );
                     labelImage(xx,yy,zz) = el+1;
                  }
               }
               else
               {
                  if ( el > 0 )
                  {
                     points[el-1] = getPointInstance( xx, yy, zz, points[el-1] );
                     labelImage(xx,yy,zz) = el-1;
                  }
               }
            }
         }
      }
   }


   /*
   CImg<unsigned char> labelShow( phi.dimx(), phi.dimy(), 1, 3 );
   labelShow.fill( 0 );

   unsigned char cols[] = { 255, 0, 0, 0, 255, 0, 0, 0, 255, 255, 255, 0, 255, 0, 255, 0, 255, 255 };

   for ( int el = 0; el < 5; ++el )
   {
      for ( Point3* listPtr = points[el]; listPtr != 0; listPtr = listPtr->next )
      {
         labelShow( listPtr->x, listPtr->y, listPtr->z, 0 ) = cols[3*el + 0];
         labelShow( listPtr->x, listPtr->y, listPtr->z, 1 ) = cols[3*el + 1];
         labelShow( listPtr->x, listPtr->y, listPtr->z, 2 ) = cols[3*el + 2];
      }

   }
   */

   /*
   CImgDisplay disp( labelImage );
   while ( !disp.is_closed )
      cimg::wait( 20 );
   */

   //exit( 99 );
}

std::ostream& operator <<( std::ostream& os, Point3& p )
{
   os << "[" << p.x << " " << p.y << " " << p.z << "]";
   return os;
}

template <class T>
T smearedDelta( T phi, T eps )
{
   phi = abs( phi );
   if ( phi > eps )
      return 0;

   return ( 1 + cos( atan(1.0)*4 * phi/eps ) ) / ( 2 * eps );
}

void sparseFieldUpdate( CImg<float>& phi, const CImg<unsigned char>& image, CImg<char>& labelImage, CImg<float>& tempPhi )
{
   Point3** points = new Point3*[5];
   calculateLabels( phi, labelImage, points );

   int x, y, z;
   float spacing[] = { 1, 1, 1 };

   float maxAbsDelta = 0;

   // Firstly, update the active set according to the equation
   for ( Point3* listPtr = points[2]; points[2] != 0; listPtr = points[2] )
   {
      x = listPtr->x; y = listPtr->y; z = listPtr->z;

      // Do the interpolation to make sure that we don't get bouncing
      float gx, gy, gz;
      float ga, gb;

      if ( phi.dimx() == 1 )
         gx = 0;
      else if ( x > 0 && x == phi.dimx()-1 )
         gx = phi(x,y,z) - phi(x-1,y,z);
      else if ( x == 0 && x != phi.dimx()-1 )
         gx = phi(x+1,y,z) - phi(x,y,z);
      else
      {
         ga = phi(x,y,z) - phi(x-1,y,z);
         gb = phi(x+1,y,z) - phi(x,y,z);

         gx = ( abs(ga) > abs(gb) ) ? ga : gb;
      }

      if ( phi.dimy() == 1 )
         gy = 0;
      else if ( y > 0 && y == phi.dimy()-1 )
         gx = phi(x,y,z) - phi(x,y-1,z);
      else if ( y == 0 && y != phi.dimy()-1 )
         gx = phi(x,y+1,z) - phi(x,y,z);
      else
      {
         ga = phi(x,y,z) - phi(x,y-1,z);
         gb = phi(x,y+1,z) - phi(x,y,z);

         gy = ( abs(ga) > abs(gb) ) ? ga : gb;
      }

      if ( phi.dimz() == 1 )
         gz = 0;
      else if ( z > 0 && z == phi.dimz()-1 )
         gz = phi(x,y,z) - phi(x,y,z-1);
      else if ( z == 0 && z != phi.dimz()-1 )
         gz = phi(x,y,z+1) - phi(x,y,z);
      else
      {
         ga = phi(x,y,z) - phi(x,y,z-1);
         gb = phi(x,y,z+1) - phi(x,y,z);

         gz = ( abs(ga) > abs(gb) ) ? ga : gb;
      }

      // Pick the maximum
      if ( abs(gx) > std::max( abs(gy), abs(gz) ) )
         gy = gz = 0;
      else if ( abs(gy) > std::max( abs(gx), abs(gz) ) )
         gx = gz = 0;
      else if ( abs(gz) > std::max( abs(gx), abs(gy) ) )
         gx = gy = 0;
      else if ( abs(gx) == abs(gy) && abs(gx) == abs(gz) )
      {}
      else if ( abs(gx) == abs(gy) )
         gz = 0;
      else if ( abs(gx) == abs(gz) )
         gy = 0;
      else if ( abs(gy) == abs(gz) )
         gz = 0;
      
      ga = gx*gx + gy*gy + gz*gz;

      gx = -phi(x,y,z) * gx / ga;
      gy = -phi(x,y,z) * gy / ga;
      gz = -phi(x,y,z) * gz / ga;

      float phiAtPoint = image.linear_pix2d( x+gx, y+gy, z+gz );
      float outwards = - (phiAtPoint*phiAtPoint);
      float inwards  =   (1-phiAtPoint)*(1-phiAtPoint);
      
      float curv = imageCurvature( phi, x, y, z, spacing );
      //float outwards = ( image(x,y,z)!=0 ) ? -1 : 0;
      //float inwards  = ( image(x,y,z)==0 ) ?  1 : 0;

      float nu = 0;
      float alpha1 = 20;
      float alpha2 = 20;

      tempPhi( x, y, z ) = smearedDelta<float>( phiAtPoint, 1.5 ) * ( nu*curv + alpha1*outwards + alpha2*inwards );

      maxAbsDelta = std::max<float>( maxAbsDelta, abs( tempPhi(x,y,z) ) );
      
      // Disconnect this so that we can recycle it
      // without worrying about it storing some random pointer
      points[2] = listPtr->next;
      recycle( listPtr );
   }
   
   std::cerr << maxAbsDelta << std::endl;

   if ( maxAbsDelta > 0.9 )
      tempPhi = tempPhi / maxAbsDelta * 0.9;

   // At the moment, tempPhi just holds the deltas, so we need to add them
   // to get the update
   tempPhi += phi;

   // Now we need to update the peripheral sets
   for ( int bandDistance = 1; bandDistance <= 2; ++bandDistance )
   {
      int set;
      
      float bestPhi;

      // Do the outer set first
      set = 2 - bandDistance;
      for ( Point3* listPtr = points[set]; points[set] != 0; listPtr = points[set] )
      {
         bestPhi = -std::numeric_limits<float>::infinity();
         OVER3x3x3( phi, listPtr->x, listPtr->y, listPtr->z )
         {
            if ( labelImage(xx,yy,zz) == set+1 )
               bestPhi = std::max( bestPhi, tempPhi(xx,yy,zz)-1 );
         }

         tempPhi( listPtr->x, listPtr->y, listPtr->z ) = bestPhi;
         
         // Disconnect this so that we can recycle it
         // without worrying about it storing a dangling pointer
         points[set] = listPtr->next;
         recycle( listPtr );
      }
      
      // Do the inner set second
      set = 2 + bandDistance;
      for ( Point3* listPtr = points[set]; points[set] != 0; listPtr = points[set] )
      {
         x = listPtr->x; y = listPtr->y; z = listPtr->z;
         
         bestPhi = std::numeric_limits<float>::infinity();
         
         OVER3x3x3( phi, listPtr->x, listPtr->y, listPtr->z )
         {
            if ( labelImage(xx,yy,zz) == set-1 )
            {
               bestPhi = std::min( bestPhi, tempPhi(xx,yy,zz)+1 );
            }
         }

         tempPhi( listPtr->x, listPtr->y, listPtr->z ) = bestPhi;
         
         // Disconnect this so that we can recycle it
         // without worrying about it storing a dangling pointer
         points[set] = listPtr->next;
         recycle( listPtr );
      }
   }
}


int main( int argc, char** argv )
{
   // This is the image that we are trying to segment
   CImg<unsigned char> image( "c:/docume~1/aturner/desktop/brainposter/slice-risk.bmp" );
   image = image.get_channel( 0 );
   //image = 255 * abs( image == 165 );
   
   cimg_forXY( image, x, y )
      if ( image(x,y) == 165 )
         image(x,y) = 1;
      else
         image(x,y) = 0;

   CImgDisplay imDisp( image );

   CImg<float> phi( image.dimx(), image.dimy(), 1, 1 );
   
   float cx = phi.dimx() / 2.f;
   float cy = phi.dimy() / 2.f;
   float cz = phi.dimz() / 2.f;

   // This is our initial guess of the level set function
   cimg_forXYZ( phi, x, y, z )
      phi(x,y,z) = sqrt( (x-cx)*(x-cx) + (y-cy)*(y-cy) + (z-cz)*(z-cz) ) - 80;

   CImg<char> labelImage( phi.dimx(), phi.dimy(), phi.dimz(), 1 );

   CImg<float> tempPhi( phi );

   CImg<unsigned char> outputImage( labelImage.dimx(), labelImage.dimy(), labelImage.dimz(), 3 );
   outputImage.fill( 0 );

   CImgDisplay disp( outputImage );

   while ( !disp.is_closed )
   {
      sparseFieldUpdate( phi, image, labelImage, tempPhi );
      phi = tempPhi;

      const float minn = phi.min();
      const float maxx = phi.max();

      cimg_forXY( outputImage, x, y )
         outputImage(x,y) = ( phi(x,y) - minn ) / ( maxx - minn );

      CImgList<> points;
      CImgList<> primitives;
      phi.marching_squares( 0, points, primitives );
         
      const unsigned char red[] = { 255, 0, 0 };

      for ( unsigned int i = 0; i < primitives.size; ++i )
      {
         unsigned int x0 = static_cast<unsigned int>( points[primitives[i](0,0)](0,0) + 0.5f );
         unsigned int y0 = static_cast<unsigned int>( points[primitives[i](0,0)](0,1) + 0.5f );

         unsigned int x1 = static_cast<unsigned int>( points[primitives[i](0,1)](0,0) + 0.5f );
         unsigned int y1 = static_cast<unsigned int>( points[primitives[i](0,1)](0,1) + 0.5f );

         outputImage.draw_line( x0, y0, x1, y1, red );
      }

      disp.display( outputImage );
   }
}