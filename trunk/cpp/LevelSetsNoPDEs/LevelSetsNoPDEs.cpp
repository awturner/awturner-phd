#define NOMINMAX
#include "CImg.h"

#include <iostream>
#include <algorithm>

#include "SimpleLinkedList.h"

using namespace cimg_library;

const int C_OUT =  3;
const int L_OUT =  1;
const int L_IN  = -1;
const int C_IN  = -3;

typedef cimg_library::CImg<int> Image;

Image getInitialPhi( const int w, const int h, const int d )
{
   Image ret( w, h, d );

   int fg;

   ret.fill( C_IN );

   fg = L_IN;
   ret.draw_rectangle( 128, 128, 384, 384, &fg );

   fg = L_OUT;
   ret.draw_rectangle( 129, 129, 383, 383, &fg );

   fg = C_OUT;
   ret.draw_rectangle( 130, 130, 382, 382, &fg );

   return ret;
}

Image getF( const char* filename )
{
   Image ret( filename );
   //Image ret( 512, 512 );

   cimg_forXYZ( ret, x, y, z )
   {
      ret(x,y,z) -= 96;
   }

   return ret;
}

bool converged( const Image& phi, const Image& F, const int x, const int y, const int z )
{
   for ( int zz = std::max(0,z-1); zz <= std::min(z+1,phi.dimz()-1); ++zz )
      for ( int yy = std::max(0,y-1); yy <= std::min(y+1,phi.dimy()-1); ++yy )
         for ( int xx = std::max(0,x-1); xx <= std::min(x+1,phi.dimx()-1); ++xx )
         {
            if ( xx == x && yy == y && zz == z )
               continue;

            // If phi is different sign but force is the same sign...
            if ( phi(xx,yy,zz)*phi(x,y,z) < 0 && F(xx,yy,zz)*F(x,y,z)> 0 )
            {
               return true;
            }
         }

   return false;
}

void checkIn( Image& phi, const Image& F, const int x, const int y, const int z )
{
   phi(x,y,z) = L_IN;

   for ( int zz = std::max(0,z-1); zz <= std::min(z+1,phi.dimz()-1); ++zz )
      for ( int yy = std::max(0,y-1); yy <= std::min(y+1,phi.dimy()-1); ++yy )
         for ( int xx = std::max(0,x-1); xx <= std::min(x+1,phi.dimx()-1); ++xx )
         {
            // Go over the neighbourhood, but avoid self
            if ( xx == x && yy == y && zz == z )
               continue;

            if ( phi(xx,yy,zz) == C_OUT )
               phi(xx,yy,zz) = L_OUT;
         }
}

void checkOut( Image& phi, const Image& F, const int x, const int y, const int z )
{
   phi(x,y,z) = L_OUT;

   for ( int zz = std::max(0,z-1); zz <= std::min(z+1,phi.dimz()-1); ++zz )
      for ( int yy = std::max(0,y-1); yy <= std::min(y+1,phi.dimy()-1); ++yy )
         for ( int xx = std::max(0,x-1); xx <= std::min(x+1,phi.dimx()-1); ++xx )
         {
            // Go over the neighbourhood, but avoid self
            if ( xx == x && yy == y && zz == z)
               continue;

            if ( phi(xx,yy,zz) == C_IN )
               phi(xx,yy,zz) = L_IN;
         }
}

bool isIn( Image& phi, const int x, const int y, const int z, int set )
{
   return phi(x,y,z) == set;
}

float weightedAverage( const Image& im, const Image& k, const int x, const int y, const int z )
{
   float num = 0;
   float den = 0;

   for ( int zz = std::max( 0, z-2 ); zz <= std::min( im.dimz()-1, z+2 ); ++zz )
   {
      int kz = zz - (z-2);
      for ( int yy = std::max( 0, y-2 ); yy <= std::min( im.dimy()-1, y+2 ); ++yy )
      {
         int ky = yy - (y-2);
         for ( int xx = std::max( 0, x-2 ); xx <= std::min( im.dimx()-1, x+2 ); ++xx )
         {
            int kx = xx - (x-2);

            num += k(kx,ky,kz)*im(xx,yy,zz);
            den += k(kx,ky,kz);
         }
      }
   }

   return num / den;
}

#define cimg_forXYZ_equalto( phi, x, y, z, val ) \
   cimg_forXYZ( phi, x, y, z ) \
      if ( phi(x,y,z) == val )

int main( int argc, char** argv )
{
   Image F   = getF( argv[1] );
   Image phi = getInitialPhi( F.dimx(), F.dimy(), F.dimz() );

   bool changed = true;

   Image kern( 5, 5, 5 );
   cimg_forXYZ( kern, x, y, z )
   {
      kern(x,y,z) = static_cast<int>( exp( -(x-2.f)*(x-2.f) / 8 ) * exp( -(y-2.f)*(y-2.f) / 8 ) * exp( -(z-2.f)*(z-2.f) / 8 ) );
   }

   CImgDisplay disp( phi, "Boundary", 3 );

   Image oldPhi( phi );

   while ( !disp.is_closed && changed )
   {
      changed = false;

      bool flag;

      cimg_forXYZ_equalto( phi, x, y, z, L_OUT )
      {
         if ( F(x,y,z) > 0 && converged( phi, F, x, y, z ) )
         {
            checkIn(phi,F,x,y,z);
            changed = true;
         }
      }

      cimg_forXYZ_equalto( phi, x, y, z, L_IN )
      {
         flag = false;

         for ( int zz = std::max(0,z-1); zz <= std::min(z+1,phi.dimz()-1); ++zz )
            for ( int yy = std::max(0,y-1); yy <= std::min(y+1,phi.dimy()-1); ++yy )
               for ( int xx = std::max(0,x-1); xx <= std::min(x+1,phi.dimx()-1); ++xx )
               {
                  // Go over the neighbourhood, but avoid self
                  if ( xx == x && yy == y && zz == z)
                     continue;

                  if ( phi(xx,yy,zz) >= 0 )
                  {
                     flag = true;
                     break;
                  }
               }

         if ( !flag )
         {
            phi(x,y,z) = C_IN;
            changed = true;
         }
      }

      cimg_forXYZ_equalto( phi, x, y, z, L_IN )
      {
         if ( F(x,y,z) < 0 && converged( phi, F, x, y, z ) )
         {
            checkOut(phi,F,x,y,z);
            changed = true;
         }
      }

      cimg_forXYZ_equalto( phi, x, y, z, L_OUT )
      {
         flag = false;
         for ( int zz = std::max(0,z-1); zz <= std::min(z+1,phi.dimz()-1); ++zz )
            for ( int yy = std::max(0,y-1); yy <= std::min(y+1,phi.dimy()-1); ++yy )
               for ( int xx = std::max(0,x-1); xx <= std::min(x+1,phi.dimx()-1); ++xx )
               {
                  if ( xx == x && yy == y && zz == z)
                     continue;

                  if ( phi(xx,yy,zz) <= 0 )
                  {
                     flag = true;
                     break;
                  }
               }

         if ( !flag )
         {
            phi(x,y,z) = C_OUT;
            changed = true;
         }
      }

      // Smoothing phase
      
      for ( int i = 0; i < 0; ++i )
      {
         cimg_forXYZ_equalto( phi, x, y, z, L_OUT )
         {
            if ( weightedAverage( phi, kern, x, y, z ) < 0 )
            {
               checkIn(phi,F,x,y,z);
               changed = true;
            }
         }

         cimg_forXYZ_equalto( phi, x, y, z, L_IN )
         {
            flag = false;

            for ( int zz = std::max(0,z-1); zz <= std::min(z+1,phi.dimz()-1); ++zz )
               for ( int yy = std::max(0,y-1); yy <= std::min(y+1,phi.dimy()-1); ++yy )
                  for ( int xx = std::max(0,x-1); xx <= std::min(x+1,phi.dimx()-1); ++xx )
                  {
                     // Go over the neighbourhood, but avoid self
                     if ( xx == x && yy == y && zz == z)
                        continue;

                     if ( phi(xx,yy,zz) >= 0 )
                     {
                        flag = true;
                        break;
                     }
                  }

            if ( !flag )
            {
               phi(x,y,z) = C_IN;
               changed = true;
            }
         }

         cimg_forXYZ_equalto( phi, x, y, z, L_IN )
         {
            if ( weightedAverage( phi, kern, x, y, z ) > 0 )
            {
               checkOut(phi,F,x,y,z);
               changed = true;
            }
         }
         
         cimg_forXYZ_equalto( phi, x, y, z, L_OUT )
         {
            flag = false;
            for ( int zz = std::max(0,z-1); zz <= std::min(z+1,phi.dimz()-1); ++zz )
               for ( int yy = std::max(0,y-1); yy <= std::min(y+1,phi.dimy()-1); ++yy )
                  for ( int xx = std::max(0,x-1); xx <= std::min(x+1,phi.dimx()-1); ++xx )
                  {
                     if ( xx == x && yy == y && zz == z)
                        continue;

                     if ( phi(xx,yy,zz) <= 0 )
                     {
                        flag = true;
                        break;
                     }
                  }

            if ( !flag )
            {
               phi(x,y,z) = C_OUT;
               changed = true;
            }
         }
      }

      changed = false;
      cimg_forXYZ( phi, x, y, z )
      {
         if ( phi(x,y,z) != oldPhi(x,y,z) )
            changed = true;
      }
      oldPhi = phi;

   }

   CImg<> phiFloat( phi );
   disp.display( phiFloat );
   
   std::cerr << "Finished!" << std::endl;

   while ( !disp.is_closed )
      cimg::wait( 20 );
}